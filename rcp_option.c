/*
********************************************************************
* RabbitControl - a protocol for remote control.
* https://rabbitcontrol.cc
*
* Copyright (C) 2024, Ingo Randolf
*
* This file is part of RabbitControl for C (rcp-c).
*
* rcp-c is free software: you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License,
* or (at your option) any later version.
*
* rcp-c is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with rcp-c. If not, see <https://www.gnu.org/licenses/>.
*********************************************************************
*/

#include "rcp_option.h"

#include <string.h>

#include "rcp_memory.h"
#include "rcp_logging.h"
#include "rcp_endian.h"
#include "rcp_string.h"
#include "rcp_infodata.h"
#include "rcp_parameter.h"
#include "rcp_vector2.h"

#if defined(RCP_OPTION_DEBUG_LOG) || defined(RCP_ALL_DEBUG)
#define RCP_OPTION_DEBUG(...) RCP_DEBUG(__VA_ARGS__)
#else
#define RCP_OPTION_DEBUG(...)
#endif

#if defined(RCP_OPTION_MALLOC_DEBUG_LOG) || defined(RCP_ALL_DEBUG)
#define RCP_OPTION_MALLOC_DEBUG(...) RCP_DEBUG(__VA_ARGS__)
#else
#define RCP_OPTION_MALLOC_DEBUG(...)
#endif

typedef enum rcp_option_data_type
{
    RCP_NONE = 0,
    RCP_BOOL,
    RCP_I8,
    RCP_I16,
    RCP_I32,
    RCP_I64,
    RCP_F32,
    RCP_F64,
    RCP_PTR,
    RCP_TINY_STRING,
    RCP_SHORT_STRING,
    RCP_LONG_STRING,
    RCP_LANGUAGE_STRING,
    RCP_INFO_DATA,
    RCP_PARAMETER_DATA,
    RCP_STRINGLIST,
    RCP_VECTOR2_F32
} rcp_option_data_type;

typedef enum rcp_option_flags
{
    RCP_FLAG_PTR_DATA = 0x01,
    RCP_FLAG_OWNS_DATA = 0x02,
    RCP_FLAG_OWNS_PTR_DATA = RCP_FLAG_PTR_DATA | RCP_FLAG_OWNS_DATA,
    RCP_FLAG_DATA_CHANGED = 0x04,
    RCP_FLAG_DATA_SIZE_PREFIXED = 0x08
} rcp_option_flags;

union rcp_option_value
{
    void* data;
    char* str; // zero-terminated c-string
    rcp_language_str* lng_str;
    bool b;
    int8_t i8;
    int16_t i16;
    int32_t i32;
    int64_t i64;
    float f;
    double d;
    rcp_infodata* info_data;
    rcp_parameter* parameter_data;
    rcp_stringlist* string_list;
    // vector
    rcp_vector2* vector2;
    // widget data
};

struct rcp_option
{
    rcp_option* next;

    union rcp_option_value data;
    size_t data_size; // size when serialized (this is not necessarily the size of the data)
    rcp_option_data_type data_type;
    char prefix;

    unsigned char flags; // flags

#ifdef RCP_OPTION_USE_EXTERNAL_GET_SET
    // external fetch func
    void (*externalGetCb)(void** out_data, size_t* out_size);
    bool (*externalSetCb)(void* data, size_t size);
#endif
};


// changed
#define RCP_OPTION_SET_CHANGED(x) (x->flags |= RCP_FLAG_DATA_CHANGED)
#define RCP_OPTION_UNSET_CHANGED(x) (x->flags &= ~RCP_FLAG_DATA_CHANGED)
#define RCP_OPTION_IS_CHANGED(x) (x->flags & RCP_FLAG_DATA_CHANGED)
// ptr data
#define RCP_OPTION_OWNS_DATA(x) (x->flags & RCP_FLAG_OWNS_DATA)
#define RCP_OPTION_IS_PTR(x) (x->flags & RCP_FLAG_PTR_DATA)


rcp_option* rcp_option_create(char prefix)
{
    if (prefix == RCP_TERMINATOR) return NULL;

    rcp_option* opt = (rcp_option*)RCP_CALLOC(1, sizeof(rcp_option));

    if (opt)
    {
        RCP_OPTION_MALLOC_DEBUG("*** option [0x%02x]: %p\n", prefix, opt);

        opt->prefix = prefix;
    }

    return opt;
}

rcp_option* rcp_option_get_create(rcp_option** options, char prefix)
{
    if (options == NULL) return NULL;

    // check if option with prefix exists
    rcp_option* opt = rcp_option_get(*options, prefix);
    if (opt != NULL) return opt;

    // no option with prefix
    // need to create option with prefix
    opt = rcp_option_create(prefix);
    if (opt != NULL)
    {
        opt->next = *options;
        *options = opt;
    }

    return opt;
}

rcp_option* rcp_option_get(rcp_option* options, char prefix)
{
    if (options == NULL) return NULL;

    // check if option with prefix exists
    rcp_option* opt = options;
    while (opt != NULL)
    {
        if (opt->prefix == prefix)
        {
            return opt;
        }
        opt = opt->next;
    }

    return NULL;
}


static void _copy_option_data(rcp_option* dst, rcp_option* src)
{
    if (dst == NULL) return;
    if (src == NULL) return;
    if (dst == src) return;

    RCP_OPTION_DEBUG("%s\n", __FUNCTION__);

    // free data - set changed
    rcp_option_free_data(dst);

    // datatypes are the same!
    dst->data_type = src->data_type;

    if (src->data_type == RCP_TINY_STRING)
    {
        rcp_option_copy_string(dst, src->data.str, TINY_STRING);
    }
    else if (src->data_type == RCP_SHORT_STRING)
    {
        rcp_option_copy_string(dst, src->data.str, SHORT_STRING);
    }
    else if (src->data_type == RCP_LONG_STRING)
    {
        rcp_option_copy_string(dst, src->data.str, LONG_STRING);
    }
    else if (src->data_type == RCP_LANGUAGE_STRING)
    {
        rcp_language_str* dst_lng_str = rcp_langstr_copy(src->data.lng_str);
        if (dst_lng_str != NULL)
        {
            // set
            dst->data.lng_str = dst_lng_str;
            dst->data_size = src->data_size;
            dst->flags |= RCP_FLAG_OWNS_PTR_DATA;
            RCP_OPTION_SET_CHANGED(dst);
        }
    }
    else if (src->data_type == RCP_INFO_DATA)
    {
        RCP_OPTION_DEBUG("infodata should not be owned by the option!\n");
    }
    else if (src->data_type == RCP_PARAMETER_DATA)
    {
        RCP_OPTION_DEBUG("parameter should not be owned by the option!\n");
    }
    else if (src->data_type == RCP_PTR)
    {
        dst->data.data = RCP_MALLOC(src->data_size);

        if (dst->data.data)
        {
            RCP_OPTION_MALLOC_DEBUG("*** option data: %p [%d]\n", dst->data.data, src->data_size);

            memcpy(dst->data.data, src->data.data, src->data_size);

            dst->data_size = src->data_size;
            dst->flags |= RCP_FLAG_OWNS_PTR_DATA;

            if (src->flags & RCP_FLAG_DATA_SIZE_PREFIXED)
            {
                dst->flags |= RCP_FLAG_DATA_SIZE_PREFIXED;
            }

            RCP_OPTION_SET_CHANGED(dst);
        }
        else
        {
            RCP_ERROR("could not malloc option data!\n");
        }
    }
    else if (src->data_type == RCP_VECTOR2_F32)
    {
        dst->data.vector2 = rcp_vector2_create();

        if (dst->data.vector2 != NULL)
        {
            memcpy(dst->data.vector2, src->data.vector2, 2 * sizeof(float));

            dst->data_size = 2 * sizeof(float);
            dst->flags |= RCP_FLAG_OWNS_PTR_DATA;
            RCP_OPTION_SET_CHANGED(dst);
        }
    }
    else
    {
        RCP_ERROR("this data should not be owned!\n");
    }
}

rcp_option* rcp_option_add_or_update(rcp_option** options, rcp_option* src)
{
    if (options == NULL) return NULL;
    if (src == NULL) return NULL;

    // check if already exists
    rcp_option* opt = *options;
    while (opt)
	{
        if (opt->prefix == src->prefix)
		{
            if (opt->data_type != src->data_type)
            {
                RCP_ERROR("option - datatype missmatch: %d - %d", opt->data_type, src->data_type);
                return NULL;
            }

            RCP_OPTION_DEBUG("%s - updating option: %d\n", __FUNCTION__, opt->prefix);

            if (RCP_OPTION_OWNS_DATA(src))
			{
                RCP_OPTION_DEBUG("src is owning the data! - opt owning %d\n", RCP_OPTION_OWNS_DATA(opt));
                _copy_option_data(opt, src);
            }
#ifdef RCP_OPTION_USE_EXTERNAL_GET_SET
            else if (opt->externalSetCb != NULL)
            {
                // TODO: use extnernal set
                RCP_OPTION_DEBUG("TODO: use external SET");
            }
#endif
            else if (opt->data.data != src->data.data)
            {
                // not owned data
                // just copy that union
                memcpy(&opt->data, &src->data, sizeof(union rcp_option_value));

                opt->data_size = src->data_size;
                opt->flags = src->flags;
                RCP_OPTION_SET_CHANGED(opt);
            }

            return opt;
        }

        opt = opt->next;
    }

    // no option found
    // create new option and add it to parameter
    RCP_OPTION_DEBUG("rcp_option_add_or_update - creating new option: %d\n", src->prefix);

    rcp_option* new_opt = (rcp_option*)RCP_CALLOC(1, sizeof(rcp_option));

    if (new_opt != NULL)
    {
        RCP_OPTION_MALLOC_DEBUG("*** option own[0x%02x]: %p\n", src->prefix, new_opt);

        if (RCP_OPTION_OWNS_DATA(src))
        {
            new_opt->prefix = src->prefix;
            new_opt->data_size = src->data_size;
            new_opt->data_type = src->data_type;
            new_opt->flags = src->flags;

#ifdef RCP_OPTION_USE_EXTERNAL_GET_SET
            new_opt->externalGetCb = src->externalGetCb;
            new_opt->externalSetCb = src->externalSetCb;
#endif

            // copy option data
            _copy_option_data(new_opt, src);
        }
        else
        {
            // copy option
            memcpy(new_opt, src, sizeof(rcp_option));
        }

        // add option to chain
        new_opt->next = *options;
        *options = new_opt;
        RCP_OPTION_SET_CHANGED(new_opt);

        return new_opt;
    }

    RCP_ERROR("could not create new otion\n");
    return NULL;
}


void rcp_option_free_chain(rcp_option* opt)
{
    rcp_option* next;
    while (opt != NULL)
    {
        next = opt->next;
        rcp_option_free(opt);
        opt = next;
    }
}


void rcp_option_free(rcp_option* opt)
{
    if (opt != NULL)
    {
        rcp_option_free_data(opt);

        RCP_OPTION_MALLOC_DEBUG("+++ option[0x%02x]: %p\n", opt->prefix, opt);
        RCP_FREE(opt);
    }
}


void rcp_option_free_data(rcp_option* opt)
{
    if (opt == NULL) return;

    if (opt->data_type != RCP_NONE)
    {
        if (RCP_OPTION_OWNS_DATA(opt))
        {
            RCP_OPTION_DEBUG("OWNING DATA! - %d - datatype: %d\n", opt->prefix, opt->data_type);

            if (opt->data_type == RCP_LANGUAGE_STRING)
            {
                rcp_langstr_free_chain(opt->data.lng_str);
            }
            else if (opt->data_type == RCP_INFO_DATA)
            {
                rcp_infodata_free(opt->data.info_data);
            }
            else if (opt->data_type == RCP_PARAMETER_DATA)
            {
                rcp_parameter_free(opt->data.parameter_data);
            }
            else if (opt->data_type == RCP_TINY_STRING
                     || opt->data_type == RCP_SHORT_STRING
                     || opt->data_type == RCP_LONG_STRING
                     || opt->data_type == RCP_PTR)
            {
                if (opt->data.data != NULL)
                {
                    // just free that pointer data
                    RCP_OPTION_MALLOC_DEBUG("+++ option data: %p\n", opt->data.data);
                    RCP_FREE(opt->data.data);
                }
            }
            else if (opt->data_type == RCP_STRINGLIST)
            {
                rcp_stringlist_free(opt->data.string_list);
            }
            else if (opt->data_type == RCP_VECTOR2_F32)
            {
                rcp_vector2_free(opt->data.vector2);
            }
        }

        opt->data.data = NULL;
        opt->data_type = RCP_NONE;
        opt->data_size = 0;
        opt->flags = RCP_FLAG_DATA_CHANGED;
    }
}

rcp_option* rcp_option_get_next(rcp_option* opt)
{
    if (opt == NULL) return NULL;

    return opt->next;
}


// changed flag
bool rcp_option_is_changed(rcp_option* opt)
{
    if (opt == NULL) return false;

    return RCP_OPTION_IS_CHANGED(opt);
}

void rcp_option_set_changed(rcp_option* opt, bool state)
{
    if (opt == NULL) return;

    if (state)
    {
        RCP_OPTION_SET_CHANGED(opt);
    }
    else
    {
        RCP_OPTION_UNSET_CHANGED(opt);
    }
}


//
char rcp_option_get_prefix(rcp_option* opt)
{
    if (opt == NULL) return 0;
    return opt->prefix;
}


// info: used only in parameter_get_value_size
// for the UPDATEVALUE command we need to know the data_size only
size_t rcp_option_get_data_size(rcp_option* opt)
{
    if (opt == NULL) return 0;

#ifdef RCP_OPTION_USE_EXTERNAL_GET_SET
    RCP_OPTION_DEBUG("option_get_size: %p\n", opt->externalGetCb);

    if (opt->externalGetCb != NULL)
    {
        // only get size
        size_t size;
        opt->externalGetCb(NULL, &size);

        RCP_OPTION_DEBUG("size from cb: %ld\n", size);

        if (opt->data_type == RCP_TINY_STRING)
        {
            return TINY_STRING + size;
        }
        else if (opt->data_type == RCP_SHORT_STRING)
        {
            return SHORT_STRING + size;
        }
        else if (opt->data_type == RCP_LONG_STRING)
        {
            return LONG_STRING + size;
        }

        return size;
    }
#endif

    if (opt->data_type == RCP_PTR &&
        opt->flags & RCP_FLAG_DATA_SIZE_PREFIXED)
    {
        return opt->data_size + sizeof(uint32_t);
    }

    return opt->data_size;
}

size_t rcp_option_write_value(rcp_option* opt, char* data, size_t size)
{
    if (opt == NULL) return 0;
    if (data == NULL) return 0;
    if (size == 0) return 0;

    // copy data    
    if (RCP_OPTION_IS_PTR(opt))
    {
        if (opt->data_type == RCP_TINY_STRING)
        {
            return rcp_write_tiny_string(data, size, opt->data.str);
        }
        else if (opt->data_type == RCP_SHORT_STRING)
        {
            return rcp_write_short_string(data, size, opt->data.str);
        }
        else if (opt->data_type == RCP_LONG_STRING)
        {
            return rcp_write_long_string(data, size, opt->data.str);
        }
        else if (opt->data_type == RCP_VECTOR2_F32)
        {
            return rcp_vector2_write(opt->data.vector2, data, size);
        }
        else if (opt->data_type == RCP_PTR)
        {
            if (opt->flags & RCP_FLAG_DATA_SIZE_PREFIXED)
            {
                // write length prefix
                _rcp_store32(data, (uint32_t)opt->data_size);

                // write data
                memcpy((char*)data + sizeof(uint32_t), opt->data.data, opt->data_size);

                return opt->data_size + sizeof(uint32_t);
            }
            else
            {
                // write data
                memcpy((char*)data, opt->data.data, opt->data_size);
            }
        }        
        else
        {
            // these are not values for parameters - ignore
            // NOTE: an option can write these:
//            LANGUAGE_STRING
//            INFO_DATA
//            PARAMETER_DATA
//            STRINGLIST

            RCP_OPTION_DEBUG("not handled option: %d\n", opt->prefix);
            return 0;
        }

    }
    else
    {
        if (opt->data_size == 1)
        {
            memcpy(data, &opt->data.i8, 1);
        }
        else if (opt->data_size == 2)
        {
            _rcp_store16(data, (uint16_t)opt->data.i16);
        }
        else if (opt->data_size == 4)
        {
            _rcp_store32(data, (uint32_t)opt->data.i32);
        }
        else if (opt->data_size == 8)
        {
            _rcp_store64(data, (uint64_t)opt->data.i64);
        }
        else
        {
            RCP_ERROR("unsupported data_size: %d\n", opt->data_size);
            return 0;
        }
    }

    return opt->data_size;
}

bool rcp_option_set_bool(rcp_option* opt, bool value)
{
    if (opt == NULL) return false;

    if (opt->data_type == RCP_BOOL &&
            opt->data.b == value)
    {
        RCP_OPTION_UNSET_CHANGED(opt);
        return false;
    }

    opt->data.b = value;
    opt->data_type = RCP_BOOL;
    opt->data_size = sizeof(bool);
    RCP_OPTION_SET_CHANGED(opt);
    return true;
}

bool rcp_option_set_i8(rcp_option* opt, int8_t value)
{
    if (opt == NULL) return false;

    if (opt->data_type == RCP_I8 &&
            opt->data.i8 == value)
    {
        RCP_OPTION_UNSET_CHANGED(opt);
        return false;
    }

    opt->data.i8 = value;
    opt->data_type = RCP_I8;
    opt->data_size = sizeof(int8_t);
    RCP_OPTION_SET_CHANGED(opt);
    return true;
}

bool rcp_option_set_i16(rcp_option* opt, int16_t value)
{
    if (opt == NULL) return false;

    if (opt->data_type == RCP_I16 &&
            opt->data.i16 == value)
    {
        RCP_OPTION_UNSET_CHANGED(opt);
        return false;
    }

    opt->data.i16 = value;
    opt->data_type = RCP_I16;
    opt->data_size = sizeof(int16_t);
    RCP_OPTION_SET_CHANGED(opt);
    return true;
}

bool rcp_option_set_i32(rcp_option* opt, int32_t value)
{
    if (opt == NULL) return false;

    if (opt->data_type == RCP_I32 &&
            opt->data.i32 == value)
    {
        RCP_OPTION_UNSET_CHANGED(opt);
        return false;
    }

    opt->data.i32 = value;
    opt->data_type = RCP_I32;
    opt->data_size = sizeof(int32_t);
    RCP_OPTION_SET_CHANGED(opt);
    return true;
}

bool rcp_option_set_i64(rcp_option* opt, int64_t value)
{
    if (opt == NULL) return false;

    if (opt->data_type == RCP_I64 &&
            opt->data.i64 == value)
    {
        RCP_OPTION_UNSET_CHANGED(opt);
        return false;
    }

    opt->data.i64 = value;
    opt->data_type = RCP_I64;
    opt->data_size = sizeof(int64_t);
    RCP_OPTION_SET_CHANGED(opt);
    return true;
}

bool rcp_option_set_f32(rcp_option* opt, float value)
{
    if (opt == NULL) return false;

    if (opt->data_type == RCP_F32 &&
            opt->data.f == value)
    {
        RCP_OPTION_UNSET_CHANGED(opt);
        return false;
    }

    opt->data.f = value;
    opt->data_type = RCP_F32;
    opt->data_size = sizeof(float);
    RCP_OPTION_SET_CHANGED(opt);
    return true;
}

bool rcp_option_set_f64(rcp_option* opt, double value)
{
    if (opt == NULL) return false;

    if (opt->data_type == RCP_F64 &&
            opt->data.d == value)
    {
        RCP_OPTION_UNSET_CHANGED(opt);
        return false;
    }

    opt->data.d = value;
    opt->data_type = RCP_F64;
    opt->data_size = sizeof(double);
    RCP_OPTION_SET_CHANGED(opt);
    return true;
}


// vector
bool rcp_option_set_vector2f(rcp_option* opt, float x, float y)
{
    if (opt == NULL) return false;

    if (opt->data_type == RCP_VECTOR2_F32 &&
        rcp_vector2_get_f_x(opt->data.vector2) == x &&
        rcp_vector2_get_f_y(opt->data.vector2) == y)
    {
        RCP_OPTION_UNSET_CHANGED(opt);
        return false;
    }

    if (opt->data_type != RCP_VECTOR2_F32)
    {
        rcp_option_free_data(opt);
    }

    if (opt->data.vector2 == NULL)
    {
        // create vector
        opt->data.vector2 = rcp_vector2_create();
        opt->flags |= RCP_FLAG_OWNS_PTR_DATA;
        opt->data_size = 2 * sizeof(float);
    }

    rcp_vector2_set_f(opt->data.vector2, x, y);
    opt->data_type = RCP_VECTOR2_F32;
    RCP_OPTION_SET_CHANGED(opt);
    return true;
}


// set external data
bool rcp_option_set_data(rcp_option* opt, void* data, size_t size, bool sizeprefixed)
{
    if (opt == NULL) return false;
    if (data == NULL) return false;
    if (size == 0) return false;

    // compare data
    if (opt->data_type == RCP_PTR)
    {
        if (opt->data.data == data) return false;

        // compare actual data?
//        if (opt->data_size == size)
//        {
//            // compare actual data?
//            if (memcmp(opt->data.data, data, size) == 0)
//            {
//                return false;
//            }
//        }
    }

    // free data - set changed
    rcp_option_free_data(opt);

    // external data needs to be freed externally
    opt->data.data = data;
    opt->data_type = RCP_PTR;
    opt->data_size = size;
    opt->flags |= RCP_FLAG_PTR_DATA | RCP_FLAG_DATA_CHANGED;

    if (sizeprefixed)
    {
        opt->flags |= RCP_FLAG_DATA_SIZE_PREFIXED;
    }

    return true;
}

bool rcp_option_copy_data(rcp_option* opt, const void* data, size_t size, bool sizeprefixed)
{
    if (opt == NULL) return false;
    if (data == NULL) return false;
    if (size == 0) return false;

    // compare data
    if (opt->data_type == RCP_PTR)
    {
        if (opt->data.data == data) return false;

        // compare actual data?
//        if (opt->data_size == size)
//        {
//            // compare actual data?
//            if (memcmp(opt->data.data, data, size) == 0)
//            {
//                return false;
//            }
//        }
    }

    // free data - set changed
    rcp_option_free_data(opt);

    // copy data
    void* cpy = RCP_MALLOC(size);
    if (cpy != NULL)
    {
        RCP_OPTION_MALLOC_DEBUG("*** user data: %p\n", cpy);

        memcpy(cpy, data, size);

        // data owned by option
        opt->data.data = cpy;        
        opt->data_size = size;
        opt->data_type = RCP_PTR;
        opt->flags |= RCP_FLAG_OWNS_PTR_DATA | RCP_FLAG_DATA_CHANGED;

        if (sizeprefixed)
        {
            opt->flags |= RCP_FLAG_DATA_SIZE_PREFIXED;
        }

        return true;
    }

    RCP_OPTION_DEBUG("could not allocate memory for data: %d\n", size);
    return false;
}

static void _set_string_type(rcp_option* opt, rcp_string_types type)
{
    if (opt == NULL) return;

    switch (type)
    {
    case TINY_STRING:
        opt->data_type = RCP_TINY_STRING;
        break;
    case SHORT_STRING:
        opt->data_type = RCP_SHORT_STRING;
        break;
    case LONG_STRING:
        opt->data_type = RCP_LONG_STRING;
        break;
    }
}

/* rcp_option_move_string
 *  move str into option:
 *  the option "owns" the data and attempts to free it on rcp_option_free()
 */
bool rcp_option_move_string(rcp_option* opt, const char* str, rcp_string_types type)
{
    if (opt == NULL) return false;
    if (str == NULL) return false;

#ifdef RCP_OPTION_USE_EXTERNAL_GET_SET
    if (opt->externalSetCb != NULL)
    {
        // use external set
        RCP_OPTION_DEBUG("set string - call external set");
        return opt->externalSetCb((void*)str, strlen(str));
    }
#endif

    if (opt->data_type == RCP_TINY_STRING ||
            opt->data_type == RCP_SHORT_STRING ||
            opt->data_type == RCP_LONG_STRING)
    {
        if (opt->data.str != NULL &&
                strcmp(opt->data.str, str) == 0)
        {
            RCP_OPTION_UNSET_CHANGED(opt);
            return false;
        }
    }

    // free data
    rcp_option_free_data(opt);

    opt->data.str = (char*)str;

    _set_string_type(opt, type);
    opt->data_size = type + strlen(str);
    opt->flags |= RCP_FLAG_OWNS_PTR_DATA;
    RCP_OPTION_SET_CHANGED(opt);
    return true;
}

/* rcp_option_copy_string
 *  copy str into option:
 *  the option "owns" the data and attempts to free it on rcp_option_free()
 */
bool rcp_option_copy_string(rcp_option* opt, const char* data, rcp_string_types type)
{
    if (opt == NULL) return false;
    if (data == NULL) return false;

#ifdef RCP_OPTION_USE_EXTERNAL_GET_SET
    if (opt->externalSetCb != NULL)
    {
        // use external set
        bool result = opt->externalSetCb((void*)data, strlen(data));

        _set_string_type(opt, type);

        result ? RCP_OPTION_SET_CHANGED(opt) : RCP_OPTION_UNSET_CHANGED(opt);

        return result;
    }
#endif

    if (opt->data_type == RCP_TINY_STRING ||
            opt->data_type == RCP_SHORT_STRING ||
            opt->data_type == RCP_LONG_STRING)
    {
        if (opt->data.str != NULL &&
                strcmp(opt->data.str, data) == 0)
        {
            RCP_OPTION_UNSET_CHANGED(opt);
            return false;
        }
    }


    // free data
    rcp_option_free_data(opt);

    // get length of string to copy
    size_t str_len = strlen(data);
    if (str_len > 0)
    {
        // try to alloc memory
        opt->data.str = (char*)RCP_CALLOC(1, str_len + 1);
        if (opt->data.str != NULL)
        {
            // memory successfully allocated
            RCP_OPTION_MALLOC_DEBUG("*** string: %p\n", opt->data.str);

            // copy string
            strncpy(opt->data.str, data, str_len);

            RCP_OPTION_SET_CHANGED(opt);
        }
        else
        {
            RCP_ERROR("could not calloc for string: %lu\n", str_len);
            return false;
        }
    }

    // setup option
    _set_string_type(opt, type);
    opt->data_size = type + str_len;
    opt->flags |= RCP_FLAG_OWNS_PTR_DATA;

    return true;
}

bool rcp_option_move_langstr(rcp_option* opt, rcp_language_str* lng_str)
{
    if (opt == NULL) return false;
    if (lng_str == NULL) return false;

    if (opt->data_type == RCP_LANGUAGE_STRING)
    {
        rcp_langstr_set_next(lng_str, opt->data.lng_str);
    }
    else
    {
        // this is not a language string
        // free data in case
        rcp_option_free_data(opt);
    }

    opt->data.lng_str = lng_str;
    opt->data_type = RCP_LANGUAGE_STRING;
    opt->data_size = rcp_langstr_get_chain_size(lng_str);
    opt->flags |= RCP_FLAG_OWNS_PTR_DATA;
    RCP_OPTION_SET_CHANGED(opt);
    return true;
}

bool rcp_option_copy_any_language(rcp_option* opt, const char* str, rcp_string_types type)
{
    if (opt == NULL) return false;
    if (str == NULL) return false;

    rcp_language_str* lng_str = rcp_option_get_langstr(opt);

    // get any string from langstrings...
    if (lng_str != NULL)
    {
        while (lng_str)
        {
            if (rcp_langstr_is_code(lng_str, RCP_LANGUAGE_CODE_ANY))
            {
                break;
            }

            lng_str = rcp_langstr_get_next(lng_str);
        }

        if (lng_str && rcp_langstr_get_string(lng_str) != NULL)
        {
            // we got any-string -> compare
            if (strcmp(rcp_langstr_get_string(lng_str), str) == 0)
            {
                // same string... leave it
                RCP_OPTION_UNSET_CHANGED(opt);
                return false;
            }
        }
    }

    if (lng_str == NULL)
    {
        lng_str = rcp_langstr_create(RCP_LANGUAGE_CODE_ANY);

        if (lng_str == NULL)
        {
            return false;
        }

        // add to chain
        rcp_option_move_langstr(opt, lng_str); // full transfer
    }


    // copy text
    rcp_langstr_copy_string(lng_str, str, type);

    // update size
    opt->data_size = rcp_langstr_get_chain_size(lng_str);

    RCP_OPTION_SET_CHANGED(opt);
    return true;
}

bool rcp_option_get_bool(rcp_option* opt)
{
    if (opt == NULL) return false;
    if (opt->data_type != RCP_BOOL) return false;

    return opt->data.b;
}

int8_t rcp_option_get_i8(rcp_option* opt)
{
    if (opt == NULL) return 0;
    if (opt->data_type != RCP_I8) return 0;

    return opt->data.i8;
}

int16_t rcp_option_get_i16(rcp_option* opt)
{
    if (opt == NULL) return 0;
    if (opt->data_type != RCP_I16) return 0;

    return opt->data.i16;
}

int32_t rcp_option_get_i32(rcp_option* opt)
{
    if (opt == NULL) return 0;
    if (opt->data_type != RCP_I32) return 0;

    return opt->data.i32;
}

int64_t rcp_option_get_i64(rcp_option* opt)
{
    if (opt == NULL) return 0;
    if (opt->data_type != RCP_I64) return 0;

    return opt->data.i64;
}

float rcp_option_get_float(rcp_option* opt)
{
    if (opt == NULL) return 0.;
    if (opt->data_type != RCP_F32) return 0.;

    return opt->data.f;
}

double rcp_option_get_double(rcp_option* opt)
{
    if (opt == NULL) return 0.;
    if (opt->data_type != RCP_F64) return 0.;

    return opt->data.d;
}

// vector
float rcp_option_get_vector2f_x(rcp_option* opt)
{
    if (opt == NULL) return 0.;
    if (opt->data_type != RCP_VECTOR2_F32) return 0.;

    return rcp_vector2_get_f_x(opt->data.vector2);
}

float rcp_option_get_vector2f_y(rcp_option* opt)
{
    if (opt == NULL) return 0.;
    if (opt->data_type != RCP_VECTOR2_F32) return 0.;

    return rcp_vector2_get_f_y(opt->data.vector2);
}


void rcp_option_get_data(rcp_option* opt, void** out_data, size_t* out_size)
{
    if (opt == NULL) return;
    if (opt->data_type != RCP_PTR) return;

    *out_data = opt->data.data;
    *out_size = opt->data_size;
}

const char* rcp_option_get_string(rcp_option* opt, rcp_string_types type)
{
    if (opt == NULL) return NULL;
    if (type == TINY_STRING && opt->data_type != RCP_TINY_STRING) return NULL;
    if (type == SHORT_STRING && opt->data_type != RCP_SHORT_STRING) return NULL;
    if (type == LONG_STRING && opt->data_type != RCP_LONG_STRING) return NULL;

#ifdef RCP_OPTION_USE_EXTERNAL_GET_SET
    if (opt->externalGetCb != NULL)
    {
        // use external get
        char* data = NULL;
        size_t size = 0;
        opt->externalGetCb((void*)&data, &size);

        if (data != NULL)
        {
            rcp_option_free_data(opt);

            rcp_option_set_data(opt, data, size - 4 + type);

            // expect data to be 0-terminated string
            RCP_OPTION_DEBUG("from external get: %ld - %s\n", size, data);
        }
    }
#endif

    // return str
    return opt->data.str;
}

rcp_language_str* rcp_option_get_langstr(rcp_option* opt)
{
    if (opt == NULL) return NULL;
    if (opt->data_type != RCP_LANGUAGE_STRING) return NULL;
    return opt->data.lng_str;
}

const char* rcp_option_get_any_language(rcp_option* opt)
{
    if (opt == NULL) return NULL;

    // get any language string
    rcp_language_str* lng_str = rcp_option_get_langstr(opt);
    while (lng_str)
    {
        if (rcp_langstr_is_code(lng_str, RCP_LANGUAGE_CODE_ANY))
        {
            break;
        }

        lng_str = rcp_langstr_get_next(lng_str);
    }

    if (lng_str)
    {
        return rcp_langstr_get_string(lng_str);
    }

    return NULL;
}

// no transfer, external data
void rcp_option_set_infodata(rcp_option* opt, rcp_infodata* data)
{
    if (opt == NULL) return;
    if (data == NULL) return;

    rcp_option_free_data(opt);

    opt->data.info_data = data;
    opt->data_type = RCP_INFO_DATA;
    opt->data_size = rcp_infodata_get_size(data);
    opt->flags |= RCP_FLAG_PTR_DATA;
    RCP_OPTION_SET_CHANGED(opt);
}

// no transfer
rcp_infodata* rcp_option_get_infodata(rcp_option* opt)
{
    if (opt == NULL) return NULL;
    if (opt->data_type != RCP_INFO_DATA) return NULL;

    return opt->data.info_data;
}

// full transfer
void rcp_option_put_infodata(rcp_option* opt, rcp_infodata* data)
{
    if (opt == NULL) return;
    if (data == NULL) return;

    rcp_option_set_infodata(opt, data);

    // take ownership
    opt->flags |= RCP_FLAG_OWNS_DATA;
}

// full transfer
rcp_infodata* rcp_option_take_infodata(rcp_option* opt)
{
    if (opt == NULL) return NULL;
    if (opt->data_type != RCP_INFO_DATA) return NULL;

    // remove ownership
    opt->flags &= ~RCP_FLAG_OWNS_DATA;

    return opt->data.info_data;
}


// no transfer, external data
void rcp_option_set_parameter(rcp_option* opt, rcp_parameter* data)
{
    if (opt == NULL) return;
    if (data == NULL) return;

    rcp_option_free_data(opt);

    opt->data.parameter_data = data;
    opt->data_type = RCP_PARAMETER_DATA;
//    opt->data_size = parameter_size(data); // -- done in rcp_option_size
    opt->flags |= RCP_FLAG_PTR_DATA;
    RCP_OPTION_SET_CHANGED(opt);
}

// no transfer
rcp_parameter* rcp_option_get_parameter(rcp_option* opt)
{
    if (opt == NULL) return NULL;
    if (opt->data_type != RCP_PARAMETER_DATA) return NULL;

    return opt->data.parameter_data;
}

// full transfer
void rcp_option_put_parameter(rcp_option* opt, rcp_parameter* data)
{
    if (opt == NULL) return;
    if (data == NULL) return;

    rcp_option_set_parameter(opt, data);

    // take ownership
    opt->flags |= RCP_FLAG_OWNS_DATA;
}

// full transfer
rcp_parameter* rcp_option_take_parameter(rcp_option* opt)
{
    if (opt == NULL) return NULL;
    if (opt->data_type != RCP_PARAMETER_DATA) return NULL;

    // remove ownership
    opt->flags &= ~RCP_FLAG_OWNS_DATA;

    return opt->data.parameter_data;
}


void rcp_option_put_stringlist(rcp_option* opt, rcp_stringlist* list)
{
    if (opt == NULL) return;
    if (list == NULL) return;

    rcp_option_free_data(opt);

    if (list)
    {
        opt->data.string_list = list;
        opt->data_type = RCP_STRINGLIST;
        opt->data_size = rcp_stringlist_get_size(list);
        opt->flags |= RCP_FLAG_OWNS_PTR_DATA;
        RCP_OPTION_SET_CHANGED(opt);
    }
}

// no transfer
rcp_stringlist* rcp_option_get_stringlist(rcp_option* opt)
{
    if (opt == NULL) return NULL;
    if (opt->data_type != RCP_STRINGLIST) return NULL;

    return opt->data.string_list;
}


//
void rcp_option_log(rcp_option* opt, const char* prefix_str, bool isunsigned)
{
#ifdef RCP_LOG_INFO
    if (opt == NULL) return;

    switch (opt->data_type)
    {
    case RCP_BOOL:
        RCP_INFO("\toption: 0x%02x - %s: %u\n", opt->prefix, prefix_str != NULL ? prefix_str : "", (uint8_t)opt->data.b);
        break;
    case RCP_I8:
        if (isunsigned)
        {
            RCP_INFO("\toption: 0x%02x - %s: %u\n", opt->prefix, prefix_str != NULL ? prefix_str : "", (uint8_t)opt->data.i8);
        }
        else
        {
            RCP_INFO("\toption: 0x%02x - %s: %d\n", opt->prefix, prefix_str != NULL ? prefix_str : "", opt->data.i8);
        }
        break;
    case RCP_I16:
        if (isunsigned)
        {
            RCP_INFO("\toption: 0x%02x - %s: %u\n", opt->prefix, prefix_str != NULL ? prefix_str : "", (uint16_t)opt->data.i16);
        }
        else
        {
            RCP_INFO("\toption: 0x%02x - %s: %d\n", opt->prefix, prefix_str != NULL ? prefix_str : "", opt->data.i16);
        }
        break;
    case RCP_I32:
        if (isunsigned)
        {
            RCP_INFO("\toption: 0x%02x - %s: %u\n", opt->prefix, prefix_str != NULL ? prefix_str : "", (uint32_t)opt->data.i32);
        }
        else
        {
            RCP_INFO("\toption: 0x%02x - %s: %d\n", opt->prefix, prefix_str != NULL ? prefix_str : "", opt->data.i32);
        }
        break;
    case RCP_I64:
        if (isunsigned)
        {
            RCP_INFO("\toption: 0x%02x - %s: %llu\n", opt->prefix, prefix_str != NULL ? prefix_str : "", (uint64_t)opt->data.i64);
        }
        else
        {
            RCP_INFO("\toption: 0x%02x - %s: %llu\n", opt->prefix, prefix_str != NULL ? prefix_str : "", opt->data.i64);
        }
        break;
    case RCP_F32:
        RCP_INFO("\toption: 0x%02x - %s: %f\n", opt->prefix, prefix_str != NULL ? prefix_str : "", opt->data.f);
        break;
    case RCP_F64:
        RCP_INFO("\toption: 0x%02x - %s: %f\n", opt->prefix, prefix_str != NULL ? prefix_str : "", opt->data.d);
        break;
    case RCP_TINY_STRING:
    case RCP_SHORT_STRING:
    case RCP_LONG_STRING:
        RCP_INFO("\toption: 0x%02x - %s: %s\n", opt->prefix, prefix_str != NULL ? prefix_str : "", opt->data.str);
        break;
    case RCP_LANGUAGE_STRING:
        rcp_langstr_log_chain(opt->data.lng_str);
        break;
    case RCP_INFO_DATA:
        rcp_infodata_log(opt->data.info_data);
        break;
    case RCP_PARAMETER_DATA:
        rcp_parameter_log(opt->data.parameter_data);
        break;
    case RCP_STRINGLIST:
        RCP_INFO("\toption: 0x%02x - %s:\n", opt->prefix, prefix_str != NULL ? prefix_str : "");
        rcp_stringlist_log(opt->data.string_list);
        break;
    case RCP_VECTOR2_F32:
        RCP_INFO("\toption: 0x%02x - %s: %f,%f\n", opt->prefix, prefix_str != NULL ? prefix_str : "", rcp_vector2_get_f_x(opt->data.vector2), rcp_vector2_get_f_y(opt->data.vector2));
        break;
    case RCP_PTR:
    default:
    {
        RCP_INFO("\toption (data): 0x%02x - %s: ", opt->prefix, prefix_str != NULL ? prefix_str : "");

        for (size_t i=0; i<opt->data_size; i++)
        {
            RCP_INFO_ONLY("0x%02x ", *(char*)(opt->data.data + i));
        }
        RCP_INFO_ONLY("\n");
    }

        break;
    }
#endif
}


size_t rcp_option_get_size(rcp_option* opt, bool force)
{
    if (opt == NULL) return 0;
    if (!force && !RCP_OPTION_IS_CHANGED(opt)) return 0;

    size_t size = 1; // prefix

    // ask option datatypes which might have changed
    if (opt->data_type == RCP_PARAMETER_DATA)
    {
        size += rcp_parameter_get_size(opt->data.parameter_data, force);
    }
    else if (opt->data_type == RCP_INFO_DATA)
    {
        size += rcp_infodata_get_size(opt->data.info_data);
    }
#ifdef RCP_OPTION_USE_EXTERNAL_GET_SET
    else if (opt->externalGetCb != NULL)
    {
        size_t ext_size;
        opt->externalGetCb(NULL, &ext_size);

        RCP_OPTION_DEBUG("size from cb: %ld - %d\n", ext_size, opt->data_type);

        if (opt->data_type == RCP_TINY_STRING)
        {
            size += TINY_STRING + ext_size;
        }
        else if (opt->data_type == RCP_SHORT_STRING)
        {
            size += SHORT_STRING + ext_size;
        }
        else if (opt->data_type == RCP_LONG_STRING)
        {
            size += LONG_STRING + ext_size;
        }
        else
        {
            size += ext_size;
        }
    }
#endif
    else
    {
        size += opt->data_size;

        if (opt->flags & RCP_FLAG_DATA_SIZE_PREFIXED)
        {
            size += sizeof(uint32_t);
        }
    }

    RCP_OPTION_DEBUG("option size [0x%02x]: %lu\n", opt->prefix, size);

    return size;
}


size_t rcp_option_write(rcp_option* opt, char* data, size_t size, bool force)
{
    if (opt == NULL
            || (!force && !RCP_OPTION_IS_CHANGED(opt)))
    {
        return 0;
    }

    size_t written = 0;

    *data = opt->prefix;
    written += 1;

    if (written >= size)
    {
        RCP_OPTION_DEBUG("offset >= data_size!\n");
        return 0;
    }

    data += 1;
    size -= 1;

#ifdef RCP_OPTION_USE_EXTERNAL_GET_SET
    if (opt->externalGetCb != NULL)
    {
        // NOTE: no ownership over external data
        void *ext_data = NULL;
        size_t ext_size = 0;
        opt->externalGetCb(&ext_data, &ext_size);

        if (ext_data != NULL)
        {
            if (opt->data_type == RCP_TINY_STRING)
            {
                written += rcp_write_tiny_string(data, size, (char*)ext_data);
            }
            else if (opt->data_type == RCP_SHORT_STRING)
            {
                written += rcp_write_short_string(data, size, (char*)ext_data);
            }
            else if (opt->data_type == RCP_LONG_STRING)
            {
                written += rcp_write_long_string(data, size, (char*)ext_data);
            }
            else
            {
                written += rcp_option_store_value(opt, data);
            }
        }
        else
        {
            RCP_ERROR("could not get external data!\n");
        }
    }
    else
    {
#endif
        if (opt->data_type == RCP_LANGUAGE_STRING)
        {
            size_t written_len = rcp_langstr_write(rcp_option_get_langstr(opt), data, size);
            if (written_len == 0)
            {
                return 0;
            }
            written += written_len;
        }
        else if (opt->data_type == RCP_INFO_DATA)
        {
            size_t written_len = rcp_infodata_write(opt->data.info_data, data, size);
            if (written_len == 0)
            {
                return 0;
            }
            written += written_len;
        }
        else if (opt->data_type == RCP_PARAMETER_DATA)
        {
            size_t written_len = rcp_parameter_write(opt->data.parameter_data, data, size, force);
            if (written_len == 0)
            {
                return 0;
            }
            written += written_len;
        }
        else if (opt->data_type == RCP_STRINGLIST)
        {
            size_t written_len = rcp_stringlist_write(opt->data.string_list, data, size);
            if (written_len == 0)
            {
                return 0;
            }
            written += written_len;
        }
        else
        {
            size_t written_len = rcp_option_write_value(opt, data, size);
            if (written_len == 0)
            {
                return 0;
            }
            written += written_len;
        }
#ifdef RCP_OPTION_USE_EXTERNAL_GET_SET
    }
#endif

    RCP_OPTION_UNSET_CHANGED(opt);

    return written;
}


#ifdef RCP_OPTION_USE_EXTERNAL_GET_SET
void rcp_option_set_external_cb(rcp_option *opt, void (*getCb)(void** out_data, size_t* out_size), bool (*setCb)(void* data, size_t size))
{
    if (opt == NULL) return;

    opt->externalGetCb = getCb;
    opt->externalSetCb = setCb;
}
#endif
