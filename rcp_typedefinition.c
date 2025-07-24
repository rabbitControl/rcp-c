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

#include "rcp_typedefinition.h"

#include <string.h>

#include "rcp_endian.h"
#include "rcp_memory.h"
#include "rcp_parser.h"
#include "rcp_string.h"
#include "rcp_logging.h"
#include "rcp_option.h"

#if defined(RCP_TYPEDEFINITION_DEBUG_LOG) || defined(RCP_ALL_DEBUG)
#define RCP_TYPEDEFINITION_DEBUG(...) RCP_DEBUG(__VA_ARGS__)
#else
#define RCP_TYPEDEFINITION_DEBUG(...)
#endif

#if defined(RCP_TYPEDEFINITION_MALLOC_DEBUG_LOG) || defined(RCP_ALL_DEBUG)
#define RCP_TYPEDEFINITION_MALLOC_DEBUG(...) RCP_DEBUG(__VA_ARGS__)
#else
#define RCP_TYPEDEFINITION_MALLOC_DEBUG(...)
#endif

struct rcp_typedefinition
{
    // mandatory
    rcp_datatype type_id;

    // options
    rcp_option* options;
};


struct rcp_typedefinition_array
{
    rcp_typedefinition default_typedefinition;

    // other mandatory fields
    rcp_typedefinition element_type;
};

struct rcp_typedefinition_custom
{
    rcp_typedefinition default_typedefinition;

    // other mandatory fields
    uint32_t size;
};



rcp_typedefinition* rcp_typedefinition_create(rcp_datatype type_id)
{
    if (type_id == DATATYPE_CUSTOMTYPE)
    {
        rcp_typedefinition_custom* td = RCP_CALLOC(1, sizeof(rcp_typedefinition_custom));

        if (td != NULL)
        {
            RCP_TYPEDEFINITION_MALLOC_DEBUG("*** type definition custom: %p\n", td);

            td->default_typedefinition.type_id = DATATYPE_CUSTOMTYPE;

            return &td->default_typedefinition;
        }

        return NULL;
    }

    rcp_typedefinition* td = (rcp_typedefinition*)RCP_CALLOC(1, sizeof(rcp_typedefinition));

    if (td != NULL)
    {
        RCP_TYPEDEFINITION_MALLOC_DEBUG("*** type definition: %p\n", td);

        td->type_id = type_id;
    }

    return td;
}

void rcp_typedefinition_free(rcp_typedefinition* typedefinition)
{
    if (typedefinition)
    {
        rcp_option_free_chain(typedefinition->options);

        RCP_TYPEDEFINITION_MALLOC_DEBUG("+++ typedefinition: %p\n", typedefinition);
        RCP_FREE(typedefinition);
    }

}

rcp_datatype rcp_typedefinition_get_type_id(rcp_typedefinition* typedefinition)
{
    if (typedefinition == NULL) return 0;

    return typedefinition->type_id;
}


bool rcp_typedefinition_has_option(rcp_typedefinition* typedefinition, char prefix)
{
    if (typedefinition)
    {
        return rcp_option_get(typedefinition->options, prefix) != NULL;
    }

    return false;
}

// set options
bool rcp_typedefinition_set_option_bool(rcp_typedefinition* typedefinition, char prefix, bool value)
{
    if (typedefinition == NULL) return false;
    rcp_option* opt = rcp_option_get_create(&typedefinition->options, prefix);
	return rcp_option_set_bool(opt, value);
}

bool rcp_typedefinition_set_option_i8(rcp_typedefinition* typedefinition, char prefix, int8_t value)
{
    if (typedefinition == NULL) return false;
    rcp_option* opt = rcp_option_get_create(&typedefinition->options, prefix);
	return rcp_option_set_i8(opt, value);
}

bool rcp_typedefinition_set_option_i16(rcp_typedefinition* typedefinition, char prefix, int16_t value)
{
    if (typedefinition == NULL) return false;
    rcp_option* opt = rcp_option_get_create(&typedefinition->options, prefix);
	return rcp_option_set_i16(opt, value);
}

bool rcp_typedefinition_set_option_i32(rcp_typedefinition* typedefinition, char prefix, int32_t value)
{
    if (typedefinition == NULL) return false;
    rcp_option* opt = rcp_option_get_create(&typedefinition->options, prefix);
	return rcp_option_set_i32(opt, value);
}

bool rcp_typedefinition_set_option_f32(rcp_typedefinition* typedefinition, char prefix, float value)
{
    if (typedefinition == NULL) return false;
    rcp_option* opt = rcp_option_get_create(&typedefinition->options, prefix);
	return rcp_option_set_f32(opt, value);
}


bool rcp_typedefinition_set_option_v2f32(rcp_typedefinition* typedefinition, char prefix, float x, float y)
{
    if (typedefinition == NULL) return false;
    rcp_option* opt = rcp_option_get_create(&typedefinition->options, prefix);
    return rcp_option_set_vector2f(opt, x, y);
}

// copy
bool rcp_typedefinition_set_option_string_tiny(rcp_typedefinition* typedefinition, char prefix, const char* value)
{
    if (typedefinition == NULL) return false;
    rcp_option* opt = rcp_option_get_create(&typedefinition->options, prefix);
	return rcp_option_copy_string(opt, value, TINY_STRING);
}

// copy
bool rcp_typedefinition_set_option_stringlist(rcp_typedefinition* typedefinition, char prefix, int count, va_list args)
{
    if (typedefinition == NULL) return false;

    // put locally created stringlist
    rcp_option_put_stringlist(rcp_option_get_create(&typedefinition->options, prefix),
                              rcp_stringlist_create_args(count, args));

    return true;
}

// no copy
bool rcp_typedefinition_set_option_data(rcp_typedefinition* typedefinition, char prefix, const char* data, size_t size, bool sizeprefixed)
{
    if (typedefinition == NULL) return false;
    rcp_option* opt = rcp_option_get_create(&typedefinition->options, prefix);
    return rcp_option_set_data(opt, (void*)data, size, sizeprefixed);
}

// get options
bool rcp_typedefinition_get_option_bool(rcp_typedefinition* typedefinition, char prefix, bool defaultValue)
{
    if (typedefinition != NULL)
    {
        rcp_option* opt = rcp_option_get(typedefinition->options, prefix);
        if (opt != NULL)
        {
            return rcp_option_get_bool(opt);
        }
    }

    return defaultValue;
}

int8_t rcp_typedefinition_get_option_i8(rcp_typedefinition* typedefinition, char prefix, int8_t defaultValue)
{
    if (typedefinition != NULL)
    {
        rcp_option* opt = rcp_option_get(typedefinition->options, prefix);
        if (opt != NULL)
        {
            return rcp_option_get_i8(opt);
        }
    }

    return defaultValue;
}

int16_t rcp_typedefinition_get_option_i16(rcp_typedefinition* typedefinition, char prefix, int16_t defaultValue)
{
    if (typedefinition != NULL)
    {
        rcp_option* opt = rcp_option_get(typedefinition->options, prefix);
        if (opt != NULL)
        {
            return rcp_option_get_i16(opt);
        }
    }

    return defaultValue;
}

int32_t rcp_typedefinition_get_option_i32(rcp_typedefinition* typedefinition, char prefix, int32_t defaultValue)
{
    if (typedefinition != NULL)
    {
        rcp_option* opt = rcp_option_get(typedefinition->options, prefix);
        if (opt != NULL)
        {
            return rcp_option_get_i32(opt);
        }
    }

    return defaultValue;
}

float rcp_typedefinition_get_option_f32(rcp_typedefinition* typedefinition, char prefix, float defaultValue)
{
    if (typedefinition != NULL)
    {
        rcp_option* opt = rcp_option_get(typedefinition->options, prefix);
        if (opt != NULL)
        {
            return rcp_option_get_float(opt);
        }
    }

    return defaultValue;
}


float rcp_typedefinition_get_option_v2f32_x(rcp_typedefinition* typedefinition, char prefix, float defaultValue)
{
    if (typedefinition != NULL)
    {
        rcp_option* opt = rcp_option_get(typedefinition->options, prefix);
        if (opt != NULL)
        {
            return rcp_option_get_vector2f_x(opt);
        }
    }

    return defaultValue;
}

float rcp_typedefinition_get_option_v2f32_y(rcp_typedefinition* typedefinition, char prefix, float defaultValue)
{
    if (typedefinition != NULL)
    {
        rcp_option* opt = rcp_option_get(typedefinition->options, prefix);
        if (opt != NULL)
        {
            return rcp_option_get_vector2f_y(opt);
        }
    }

    return defaultValue;
}

void rcp_typedefinition_custom_set_size(rcp_typedefinition_custom* typedefinition, uint32_t size)
{
    if (typedefinition != NULL)
    {
        typedefinition->size = size;
    }
}

uint32_t rcp_typedefinition_custom_get_size(rcp_typedefinition_custom* typedefinition)
{
    if (typedefinition != NULL)
    {
        return typedefinition->size;
    }

    return 0;
}


// no transfer
const char* rcp_typedefinition_get_option_string_tiny(rcp_typedefinition* typedefinition, char prefix)
{
    if (typedefinition != NULL)
    {
        rcp_option* opt = rcp_option_get(typedefinition->options, prefix);
        if (opt != NULL)
        {
            return rcp_option_get_string(opt, TINY_STRING);
        }
    }

    return NULL;
}

// no transfer
rcp_stringlist* rcp_typedefinition_get_option_stringlist(rcp_typedefinition* typedefinition, char prefix)
{
    if (typedefinition != NULL)
    {
        rcp_option* opt = rcp_option_get(typedefinition->options, prefix);
        if (opt != NULL)
        {
            return rcp_option_get_stringlist(opt);
        }
    }

    return NULL;
}

void rcp_typedefinition_get_option_data(rcp_typedefinition* typedefinition, char prefix, const char** out_data, size_t* out_size)
{
    if (*out_data != NULL)
    {
        RCP_TYPEDEFINITION_DEBUG("get option data: out_data != NULL");
    }

    *out_data = NULL;
    *out_size = 0;

    if (typedefinition != NULL)
    {
        rcp_option* opt = rcp_option_get(typedefinition->options, prefix);
        if (opt != NULL)
        {
            return rcp_option_get_data(opt, (void**)out_data, out_size);
        }
    }
}



//
char* rcp_typedefinition_parse_number_value(rcp_typedefinition* typedefinition, char* data, size_t* size, rcp_option* opt)
{
    if (typedefinition == NULL) return data;

    RCP_TYPEDEFINITION_DEBUG("parse_number_value: %d\n", typedefinition->type_id);

    switch (typedefinition->type_id)
    {
    case DATATYPE_INVALID:
    case DATATYPE_MAX_:
        break;

    case DATATYPE_BOOLEAN:
    case DATATYPE_ENUM:
    {
        rcp_option_free_data(opt);
        int8_t val;
        data = rcp_read_i8(data, size, &val);
        if (data == NULL) return NULL;

        rcp_option_set_bool(opt, (val > 0));
        return data;
    }

    case DATATYPE_INT8:
    case DATATYPE_UINT8:
    {
        rcp_option_free_data(opt);
        int8_t val;
        data = rcp_read_i8(data, size, &val);
        if (data == NULL) return NULL;

        rcp_option_set_i8(opt, val);
        return data;
    }

    case DATATYPE_INT16:
    case DATATYPE_UINT16:
    {
        rcp_option_free_data(opt);
        int16_t val;
        data = rcp_read_i16(data, size, &val);
        if (data == NULL) return NULL;

        rcp_option_set_i16(opt, val);
        return data;
    }

    case DATATYPE_INT32:
    case DATATYPE_UINT32:
    case DATATYPE_RGB:
    case DATATYPE_IPV4:
    {
        rcp_option_free_data(opt);
        int32_t val;
        data = rcp_read_i32(data, size, &val);
        if (data == NULL) return NULL;

        rcp_option_set_i32(opt, val);
        return data;
    }

    case DATATYPE_INT64:
    case DATATYPE_UINT64:
    {
        rcp_option_free_data(opt);
        int64_t val;
        data = rcp_read_i64(data, size, &val);
        if (data == NULL) return NULL;

        rcp_option_set_i64(opt, val);
        return data;
    }
			
	case DATATYPE_FLOAT32:
    {
		rcp_option_free_data(opt);
		float val;
		data = rcp_read_f32(data, size, &val);
        if (data == NULL) return NULL;

		rcp_option_set_f32(opt, val);
		return data;
	}

    case DATATYPE_FLOAT64:
    {
		rcp_option_free_data(opt);
		double val;
		data = rcp_read_f64(data, size, &val);
        if (data == NULL) return NULL;

		rcp_option_set_f64(opt, val);
		return data;
	}

    case DATATYPE_VECTOR2F32:
    {
        rcp_option_free_data(opt);
        float x, y;

        data = rcp_read_f32(data, size, &x);
        if (data == NULL) return NULL;

        data = rcp_read_f32(data, size, &y);
        if (data == NULL) return NULL;

        rcp_option_set_vector2f(opt, x, y);
        return data;
    }

    case DATATYPE_VECTOR2I32:
    case DATATYPE_VECTOR3F32:
    case DATATYPE_VECTOR3I32:
    case DATATYPE_VECTOR4F32:
    case DATATYPE_VECTOR4I32:
        // TODO
        break;

    }

    RCP_TYPEDEFINITION_DEBUG("wrong type or not implemented: did not read value!");
    return NULL;
}

char* rcp_typedefinition_parse_string_value(rcp_typedefinition* typedefinition, char* data, size_t* size, rcp_option* opt)
{
    if (typedefinition == NULL) return data;

    RCP_TYPEDEFINITION_DEBUG("parse_string_value: %d\n", typedefinition->type_id);

    if (typedefinition->type_id == DATATYPE_STRING)
    {
        rcp_option_free_data(opt);
        char* string_value = NULL;
        uint32_t str_len = 0;
        data = rcp_read_long_string(data, size, &string_value, &str_len);
        if (data == NULL) return NULL;

        rcp_option_move_string(opt, string_value, LONG_STRING);
        return data;
    }
    else if (typedefinition->type_id == DATATYPE_ENUM)
    {
        rcp_option_free_data(opt);
        char* string_value = NULL;
        uint8_t str_len = 0;
        data = rcp_read_tiny_string(data, size, &string_value, &str_len);
        if (data == NULL) return NULL;

        rcp_option_move_string(opt, string_value, TINY_STRING);
        return data;
    }

    RCP_TYPEDEFINITION_DEBUG("wrong type: did not read value!");
    return NULL;
}

static char* rcp_typedefinition_parse_stringlist_value(rcp_typedefinition* typedefinition, char* data, size_t* size, rcp_option* opt)
{
    if (typedefinition == NULL) return data;

    RCP_TYPEDEFINITION_DEBUG("parse_stringlist_value: %d\n", typedefinition->type_id);

    // INFO: we know it is a enum-type...

    rcp_option_free_data(opt);

    char* string_value = NULL;
    int count = 0;

    rcp_stringlist* list = rcp_stringlist_create(0);

    do
    {
        uint8_t str_len = 0;
        data = rcp_read_tiny_string(data, size, &string_value, &str_len);

        if (string_value != NULL)
        {
            count++;

            // append_put string
            rcp_stringlist_append_put(list, string_value);
        }

        if (data == NULL) return NULL;

    } while (string_value != NULL);

    if (count > 0)
    {
        rcp_option_put_stringlist(opt, list);
    }
    else
    {
        rcp_stringlist_free(list);
    }

    return data;
}


// return data
char* parse_number_type_option(rcp_typedefinition* typedefinition, char* data, size_t* size, rcp_number_options number_option)
{
    if (typedefinition == NULL) return NULL;

    RCP_TYPEDEFINITION_DEBUG("parse_number_type_option: %d\n", number_option);

    rcp_option* opt;

    switch (number_option)
    {
    case NUMBER_OPTIONS_DEFAULT:
        opt = rcp_option_get_create(&typedefinition->options, NUMBER_OPTIONS_DEFAULT);
        return rcp_typedefinition_parse_number_value(typedefinition, data, size, opt);

    case NUMBER_OPTIONS_MINIMUM:
        opt = rcp_option_get_create(&typedefinition->options, NUMBER_OPTIONS_MINIMUM);
        return rcp_typedefinition_parse_number_value(typedefinition, data, size, opt);

    case NUMBER_OPTIONS_MAXIMUM:
        opt = rcp_option_get_create(&typedefinition->options, NUMBER_OPTIONS_MAXIMUM);
        return rcp_typedefinition_parse_number_value(typedefinition, data, size, opt);

    case NUMBER_OPTIONS_MULTIPLEOF:
        opt = rcp_option_get_create(&typedefinition->options, NUMBER_OPTIONS_MULTIPLEOF);
        return rcp_typedefinition_parse_number_value(typedefinition, data, size, opt);

    case NUMBER_OPTIONS_SCALE:
    {
        opt = rcp_option_get_create(&typedefinition->options, NUMBER_OPTIONS_SCALE);
        rcp_option_free_data(opt);
        int8_t val;
        char* t = rcp_read_i8(data, size, &val);
        if (t == NULL) return NULL;

        rcp_option_set_i8(opt, val);
        return t;
    }
    case NUMBER_OPTIONS_UNIT:
        return rcp_read_tiny_string_option(&typedefinition->options, data, size, NUMBER_OPTIONS_UNIT);
    }

    return NULL;
}


char* parse_string_type_option(rcp_typedefinition* typedefinition, char* data, size_t* size, rcp_string_options option)
{
    if (typedefinition == NULL) return NULL;

    RCP_TYPEDEFINITION_DEBUG("parse_string_type_option: %d\n", option);

    rcp_option* opt;

    switch (option)
    {
    case STRING_OPTIONS_DEFAULT:
        opt = rcp_option_get_create(&typedefinition->options, STRING_OPTIONS_DEFAULT);
        return rcp_typedefinition_parse_string_value(typedefinition, data, size, opt);

    case STRING_OPTIONS_REGULAR_EXPRESSION:
        opt = rcp_option_get_create(&typedefinition->options, STRING_OPTIONS_REGULAR_EXPRESSION);
        return rcp_typedefinition_parse_string_value(typedefinition, data, size, opt);
    }

    return NULL;
}

char* parse_enum_type_option(rcp_typedefinition* typedefinition, char* data, size_t* size, rcp_enum_options option)
{
    if (typedefinition == NULL) return NULL;

    RCP_TYPEDEFINITION_DEBUG("parse_string_type_option: %d\n", option);

    switch (option)
    {
    case ENUM_OPTIONS_DEFAULT:
    {
        rcp_option* opt = rcp_option_get_create(&typedefinition->options, ENUM_OPTIONS_DEFAULT);
        return rcp_typedefinition_parse_string_value(typedefinition, data, size, opt);
    }

    case ENUM_OPTIONS_MULTISELECT:
    {
        if (*size >= 1)
        {
            rcp_option* opt = rcp_option_get_create(&typedefinition->options, ENUM_OPTIONS_MULTISELECT);

            if (opt == NULL)
            {
                RCP_ERROR("could not create or get option for ENUM_OPTIONS_MULTISELECT\n");
                return NULL;
            }

            int8_t val;
            data = rcp_read_i8(data, size, &val);
            if (data == NULL) return NULL;

            rcp_option_set_bool(opt, (val > 0));
            return data;
        }
    }

    case ENUM_OPTIONS_ENTRIES:
    {
        if (*data == RCP_TERMINATOR)
        {
            // skip if there are no entries
            *size -= 1;
            return data + 1;
        }
        rcp_option* opt = rcp_option_get_create(&typedefinition->options, ENUM_OPTIONS_ENTRIES);
        return rcp_typedefinition_parse_stringlist_value(typedefinition, data, size, opt);
    }
    }

    return NULL;
}

static char* parse_bool_type_option(rcp_typedefinition* typedefinition, char* data, size_t* size, uint8_t option)
{
    if (typedefinition == NULL) return NULL;

    RCP_TYPEDEFINITION_DEBUG("parse_bool_type_option: %d\n", option);

    rcp_option* opt;

    if (option == RCP_OPTIONS_DEFAULT)
    {
        opt = rcp_option_get_create(&typedefinition->options, RCP_OPTIONS_DEFAULT);
        return rcp_typedefinition_parse_number_value(typedefinition, data, size, opt);
    }

    return NULL;
}

static char* parse_ipv4_type_option(rcp_typedefinition* typedefinition, char* data, size_t* size, uint8_t option)
{
    if (typedefinition == NULL) return NULL;

    RCP_TYPEDEFINITION_DEBUG("parse_ipv4_type_option: %d\n", option);

    rcp_option* opt;

    if (option == IPV4_OPTIONS_DEFAULT)
    {
        opt = rcp_option_get_create(&typedefinition->options, IPV4_OPTIONS_DEFAULT);
        return rcp_typedefinition_parse_number_value(typedefinition, data, size, opt);
    }

    return NULL;
}

static char* parse_custom_type_option(rcp_typedefinition_custom* typedefinition, char* data, size_t* size, uint8_t option)
{
    if (typedefinition == NULL) return NULL;

    RCP_TYPEDEFINITION_DEBUG("parse_custom_type_option: %d\n", option);

    switch (option)
    {
    case CUSTOMTYPE_OPTIONS_DEFAULT:
    {
        if (*size >= typedefinition->size)
        {
            rcp_option* opt = rcp_option_get_create(&typedefinition->default_typedefinition.options, CUSTOMTYPE_OPTIONS_DEFAULT);

            if (opt == NULL)
            {
                RCP_ERROR("could not create or get option for CUSTOMTYPE_OPTIONS_DEFAULT\n");
                return NULL;
            }

            rcp_option_copy_data(opt, data, typedefinition->size, false);

            *size -= typedefinition->size;
            return data + typedefinition->size;
        }
    }

    case CUSTOMTYPE_OPTIONS_UUID:
    {
        if (*size >= RCP_CUSTOMTYPE_UUID_LENGTH)
        {
            rcp_option* opt = rcp_option_get_create(&typedefinition->default_typedefinition.options, CUSTOMTYPE_OPTIONS_UUID);

            if (opt == NULL)
            {
                RCP_ERROR("could not create or get option for CUSTOMWIDGET_OPTIONS_UUID\n");
                return NULL;
            }

            rcp_option_copy_data(opt, data, RCP_CUSTOMTYPE_UUID_LENGTH, false);

            *size -= RCP_CUSTOMTYPE_UUID_LENGTH;
            return data + RCP_CUSTOMTYPE_UUID_LENGTH;
        }
    }

    case CUSTOMTYPE_OPTIONS_CONFIG:
    {
        uint32_t data_size;
        data = rcp_read_i32(data, size, (int32_t*)&data_size);
        if (data == NULL) return NULL;

        if (*size < data_size)
        {
            RCP_ERROR("error - not enought data to read: %d bytes from %d\n", data_size, *size);
            return NULL;
        }

        rcp_option* opt = rcp_option_get_create(&typedefinition->default_typedefinition.options, CUSTOMTYPE_OPTIONS_CONFIG);

        if (opt == NULL)
        {
            RCP_ERROR("could not create or get option for CUSTOMTYPE_OPTIONS_CONFIG\n");
            return NULL;
        }

        rcp_option_copy_data(opt, data, data_size, true);

        *size -= data_size;
        return data + data_size;
    }
    }

    return NULL;
}

char* rcp_typedefinition_parse_type_options(rcp_typedefinition* typedefinition, char* data, size_t* size)
{
    if (typedefinition == NULL) return NULL;

    if (typedefinition->type_id == 0
            || typedefinition->type_id >= DATATYPE_MAX_)
    {
        // invalid!
        RCP_ERROR("parse_type_options - invalid\n");
        return NULL;
    }


    uint8_t option_prefix = 0;

    while (data != NULL &&
           *size > 0)
    {
        // reset
        option_prefix = 0;

        // read prefix
        data = rcp_read_u8(data, size, &option_prefix);
        if (data == NULL) return NULL;

        if (option_prefix == RCP_TERMINATOR)
        {
            // terminator - end of typedefinition
            RCP_TYPEDEFINITION_DEBUG("parse_type_options - terminator\n");
            return data;
        }

        if (*size == 0) return NULL;

        RCP_TYPEDEFINITION_DEBUG("parse_type_options - option_prefix: %d\n", option_prefix);

        // we need to handle option per typedefinition!
        switch (typedefinition->type_id)
        {
        case DATATYPE_BOOLEAN:
            // handle default option
            data = parse_bool_type_option(typedefinition, data, size, option_prefix);
            break;

        case DATATYPE_INT8:
        case DATATYPE_UINT8:
        case DATATYPE_INT16:
        case DATATYPE_UINT16:
        case DATATYPE_INT32:
        case DATATYPE_UINT32:
        case DATATYPE_INT64:
        case DATATYPE_UINT64:
		case DATATYPE_FLOAT32:
        case DATATYPE_FLOAT64:
        case DATATYPE_VECTOR2F32:
        case DATATYPE_VECTOR2I32:
        case DATATYPE_VECTOR3F32:
        case DATATYPE_VECTOR3I32:
        case DATATYPE_VECTOR4F32:
        case DATATYPE_VECTOR4I32:
            // handle number option
            data = parse_number_type_option(typedefinition, data, size, option_prefix);
            break;

        case DATATYPE_STRING:
            data = parse_string_type_option(typedefinition, data, size, option_prefix);
            break;

        case DATATYPE_ENUM:
            data = parse_enum_type_option(typedefinition, data, size, option_prefix);
            break;


        case DATATYPE_BANG:
        case DATATYPE_GROUP:
            // no options for this datatype
            break;

        case DATATYPE_IPV4:
            data = parse_ipv4_type_option(typedefinition, data, size, option_prefix);
            break;

        case DATATYPE_CUSTOMTYPE:
            data = parse_custom_type_option((rcp_typedefinition_custom*)typedefinition, data, size, option_prefix);
            break;
        }
    }

    // sane packets don't come here
    return NULL;
}

size_t rcp_typedefinition_get_size(rcp_typedefinition* typedefinition, bool all)
{
    if (typedefinition == NULL) return 0;

    // default size type-id(1) + terminator(1)
    size_t size = 2;

    // add typedefinition mandatory
    if (typedefinition->type_id == DATATYPE_CUSTOMTYPE)
    {
        // size prefix
        size += 4;
    }

    // add up options
    rcp_option* opt = typedefinition->options;
    while (opt)
    {
        size += rcp_option_get_size(opt, all);

        opt = rcp_option_get_next(opt);
    }

    return size;
}

size_t rcp_typedefinition_write(rcp_typedefinition* typedefinition, char* dst, size_t size, bool all)
{
    if (typedefinition == NULL)
    {
        return 0;
    }

    if (size < 1)
    {
        return 0;
    }

    size_t written = 0;

    // write mandatory
    size_t written_len = rcp_typedefinition_write_mandatory(typedefinition, dst, size);

    if (written_len == 0)
    {
        return 0;
    }

    written += written_len;

    if (written >= size) return 0;

    dst += written_len;


    // write type options
    rcp_option* opt = typedefinition->options;
    while (opt)
    {
        if (all || rcp_option_is_changed(opt))
        {
            written_len = rcp_option_write(opt, dst, size - written, all);
            if (written_len == 0)
            {
                return 0;
            }

            written += written_len;

            if (written >= size) return 0;

            dst += written_len;
        }

        opt = rcp_option_get_next(opt);
    }

    // write terminator
    memset(dst, 0, 1);
    written += 1;

    return written;
}


size_t rcp_typedefinition_write_mandatory(rcp_typedefinition* typedefinition, char* dst, size_t size)
{
    if (typedefinition == NULL)
    {
        return 0;
    }

    if (size < 1)
    {
        return 0;
    }

    size_t written = 0;

    // write type id
    dst[0] = typedefinition->type_id;
    written += 1;

    if (written >= size) return 0;

    dst += 1;

    // write type mandatory

    if (typedefinition->type_id == DATATYPE_CUSTOMTYPE)
    {
        if (size < 5)
        {
            return 0;
        }

        _rcp_store32(dst, ((rcp_typedefinition_custom*)typedefinition)->size);

        written += 4;

        if (written > size) return 0;
    }

    return written;
}


//void log_number_value(const char* prefix_str, option* opt, rcp_datatype type)
//{

//    char prefix = option_get_prefix(opt);

//    switch (type)
//    {

//    case DATATYPE_INT8:
//        INFO("\toption: 0x%02x - %s: %d\n", prefix, prefix_str, option_get_i8(opt));
//        break;

//    case DATATYPE_INT16:
//        INFO("\toption: 0x%02x - %s: %d\n", opt->prefix, prefix_str, opt->data.i16);
//        break;

//    case DATATYPE_INT32:
//        INFO("\toption: 0x%02x - %s: %d\n", opt->prefix, prefix_str, opt->data.i32);
//        break;

//    case DATATYPE_INT64:
//        INFO("\toption: 0x%02x - %s: %llu\n", opt->prefix, prefix_str, opt->data.i64);
//        break;

//    case DATATYPE_FLOAT32:
//        INFO("\toption: 0x%02x - %s: %f\n", opt->prefix, prefix_str, opt->data.f);
//        break;

//    case DATATYPE_FLOAT64:
//        INFO("\toption: 0x%02x - %s: %f\n", opt->prefix, prefix_str, opt->data.d);
//        break;
//    }
//}


void log_number_option(rcp_option* opt, rcp_datatype type)
{
#ifdef RCP_LOG_INFO
    bool is_unsigned = type == DATATYPE_UINT8 || type == DATATYPE_UINT16 || type == DATATYPE_UINT32;

    switch((rcp_number_options)rcp_option_get_prefix(opt))
    {
    case NUMBER_OPTIONS_DEFAULT:        
        rcp_option_log(opt, "DEFAULT", is_unsigned);
//        log_number_value("DEFAULT", opt, type);
        break;

    case NUMBER_OPTIONS_MINIMUM:
        rcp_option_log(opt, "MINIMUM", is_unsigned);
//        log_number_value("MINIMUM", opt, type);
        break;

    case NUMBER_OPTIONS_MAXIMUM:
        rcp_option_log(opt, "MAXIMUM", is_unsigned);
//        log_number_value("MAXIMUM", opt, type);
        break;

    case NUMBER_OPTIONS_MULTIPLEOF:
        rcp_option_log(opt, "MULT", is_unsigned);
//        log_number_value("MULT", opt, type);
        break;

    case NUMBER_OPTIONS_SCALE:
        RCP_INFO("\toption: 0x%02x - SCALE: %d\n", rcp_option_get_prefix(opt), rcp_option_get_i8(opt));
        break;

    case NUMBER_OPTIONS_UNIT:
        RCP_INFO("\toption: 0x%02x - UNIT: %s\n", rcp_option_get_prefix(opt), rcp_option_get_string(opt, TINY_STRING));
        break;
    }
#endif
}

#ifdef RCP_LOG_INFO
static const char* rcp_get_type_name(rcp_datatype type)
{
    switch(type)
    {
    case DATATYPE_INVALID:
        return "invalid";
    case DATATYPE_CUSTOMTYPE:
        return "custom";
    case DATATYPE_BOOLEAN:
        return "boolean";
    case DATATYPE_INT8:
        return "int8";
    case DATATYPE_UINT8:
        return "uint8";
    case DATATYPE_INT16:
        return "int16";
    case DATATYPE_UINT16:
        return "uint16";
    case DATATYPE_INT32:
        return "int32";
    case DATATYPE_UINT32:
        return "uint32";
    case DATATYPE_INT64:
        return "int64";
    case DATATYPE_UINT64:
        return "uint64";
    case DATATYPE_FLOAT32:
        return "float32";
    case DATATYPE_FLOAT64:
        return "float64";
    case DATATYPE_VECTOR2I32:
        return "vector2 int32";
    case DATATYPE_VECTOR2F32:
        return "vector2 float32";
    case DATATYPE_VECTOR3I32:
        return "vector3 int32";
    case DATATYPE_VECTOR3F32:
        return "vector3 float32";
    case DATATYPE_VECTOR4I32:
        return "vector4 int32";
    case DATATYPE_VECTOR4F32:
        return "vector4 float32";
    case DATATYPE_STRING:
        return "string";
    case DATATYPE_RGB:
        return "rgb";
    case DATATYPE_RGBA:
        return "rgba";
    case DATATYPE_ENUM:
        return "enum";
    case DATATYPE_ARRAY:
        return "array";
    case DATATYPE_LIST:
        return "list";
    case DATATYPE_BANG:
        return "bang";
    case DATATYPE_GROUP:
        return "group";
    case DATATYPE_URI:
        return "uri";
    case DATATYPE_IPV4:
        return "ipv4";
    case DATATYPE_IPV6:
        return "ipv6";
    case DATATYPE_RANGE:
        return "range";
    case DATATYPE_IMAGE:
        return "image";
    case DATATYPE_MAX_:
        break;
    }

    return "unknown type";
}
#endif

void rcp_typedefinition_log(rcp_typedefinition* typedefinition)
{
#ifdef RCP_LOG_INFO
    if (typedefinition == NULL) return;

    RCP_INFO("\ttype id: %d - %s\n", typedefinition->type_id, rcp_get_type_name(typedefinition->type_id));

    if (typedefinition->options)
    {
        RCP_INFO("  type options:\n");

        rcp_option* opt = typedefinition->options;

        while (opt)
        {
            switch (typedefinition->type_id)
            {
            case DATATYPE_BOOLEAN:
                RCP_INFO("\toption: 0x%02x - %d\n", rcp_option_get_prefix(opt), rcp_option_get_bool(opt));
                break;

            case DATATYPE_INT8:
            case DATATYPE_UINT8:
            case DATATYPE_INT16:
            case DATATYPE_UINT16:
            case DATATYPE_INT32:
            case DATATYPE_UINT32:
            case DATATYPE_INT64:
            case DATATYPE_UINT64:
            case DATATYPE_FLOAT32:
            case DATATYPE_FLOAT64:
            case DATATYPE_VECTOR2F32:
            case DATATYPE_VECTOR2I32:
            case DATATYPE_VECTOR3F32:
            case DATATYPE_VECTOR3I32:
            case DATATYPE_VECTOR4F32:
            case DATATYPE_VECTOR4I32:
                log_number_option(opt, typedefinition->type_id);
                break;

            case DATATYPE_STRING:
                RCP_INFO("\toption: 0x%02x - %s\n", rcp_option_get_prefix(opt), rcp_option_get_string(opt, LONG_STRING));
                break;

            case DATATYPE_ENUM:
                switch((rcp_enum_options)rcp_option_get_prefix(opt))
                {
                case ENUM_OPTIONS_DEFAULT:
                    rcp_option_log(opt, "DEFAULT", false);
                    break;
                case ENUM_OPTIONS_MULTISELECT:
                    rcp_option_log(opt, "MULTISELECT", false);
                    break;
                case ENUM_OPTIONS_ENTRIES:
                    rcp_option_log(opt, "ENTRIES", false);
                    break;
                }
                break;

            case DATATYPE_GROUP:
                RCP_INFO("\toption: 0x%02x\n", rcp_option_get_prefix(opt));
                break;

            case DATATYPE_IPV4:
            {
                uint32_t v = rcp_option_get_i32(opt);
                RCP_INFO("\toption: 0x%02x - %d.%d.%d.%d\n", rcp_option_get_prefix(opt), ((v >> 24) & 0xFF), ((v >> 16) & 0xFF), ((v >> 8) & 0xFF), ((v >> 0) & 0xFF));
            }
                break;

            case DATATYPE_CUSTOMTYPE:
            {
                switch((rcp_customtype_options)rcp_option_get_prefix(opt))
                {
                case CUSTOMTYPE_OPTIONS_DEFAULT:
                    rcp_option_log(opt, "DEFAULT", false);
                    break;
                case CUSTOMTYPE_OPTIONS_UUID:
                    rcp_option_log(opt, "UUID", false);
                    break;
                case CUSTOMTYPE_OPTIONS_CONFIG:
                    rcp_option_log(opt, "CONFIG", false);
                    break;
                }
                break;
            }

            default:
                RCP_INFO("\toption: 0x%02x\n", rcp_option_get_prefix(opt));
                break;
            }


            opt = rcp_option_get_next(opt);
        }
    }
#endif
}


void rcp_typedefinition_all_options_changed(rcp_typedefinition* typedefinition)
{
    if (typedefinition == NULL) return;

    rcp_option* opt = typedefinition->options;
    while (opt)
    {
        rcp_option_set_changed(opt, true);
        opt = rcp_option_get_next(opt);
    }
}

void rcp_typedefinition_all_options_unchanged(rcp_typedefinition* typedefinition)
{
    if (typedefinition == NULL) return;

    rcp_option* opt = typedefinition->options;
    while (opt)
    {
        rcp_option_set_changed(opt, false);
        opt = rcp_option_get_next(opt);
    }
}

bool rcp_typedefinition_changed(rcp_typedefinition* typedefinition)
{
	if (typedefinition == NULL) return false;

    rcp_option* opt = typedefinition->options;
    while (opt)
    {
		if (rcp_option_is_changed(opt)) return true;
        opt = rcp_option_get_next(opt);
    }
	
	return false;
}
