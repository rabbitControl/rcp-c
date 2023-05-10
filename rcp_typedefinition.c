/*
********************************************************************
* rabbitcontrol - a protocol and data-format for remote control.
*
* https://rabbitcontrol.cc
* https://github.com/rabbitcontrol/rcp-c
*
* This file is part of rabbitcontrol for c.
*
* Written by Ingo Randolf, 2021 - 2022
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*********************************************************************
*/

#include "rcp_typedefinition.h"

#include <string.h>

#include "rcp_memory.h"
#include "rcp_parser.h"
#include "rcp_string.h"
#include "rcp_logging.h"
#include "rcp_option.h"

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



rcp_typedefinition* rcp_typedefinition_create(rcp_datatype type_id)
{
    rcp_typedefinition* td = (rcp_typedefinition*)RCP_CALLOC(1, sizeof(rcp_typedefinition));

    if (td != NULL)
    {
        RCP_DEBUG("*** type definition: %p\n", td);

        td->type_id = type_id;
    }

    return td;
}

void rcp_typedefinition_free(rcp_typedefinition* typedefinition)
{
    if (typedefinition)
    {
        rcp_option_free_chain(typedefinition->options);

        RCP_DEBUG("+++ typedefinition: %p\n", typedefinition);
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

bool rcp_typedefinition_set_option_string_tiny(rcp_typedefinition* typedefinition, char prefix, const char* value)
{
    if (typedefinition == NULL) return false;
    rcp_option* opt = rcp_option_get_create(&typedefinition->options, prefix);
	return rcp_option_copy_string(opt, value, TINY_STRING);
}

bool rcp_typedefinition_set_option_stringlist(rcp_typedefinition* typedefinition, char prefix, int count, va_list args)
{
    if (typedefinition == NULL) return false;

    rcp_option_put_stringlist(rcp_option_get_create(&typedefinition->options, prefix),
                              rcp_stringlist_create_args(count, args));

    return true;
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


//
char* rcp_typedefinition_parse_number_value(rcp_typedefinition* typedefinition, char* data, size_t* size, rcp_option* opt)
{
    if (typedefinition == NULL) return data;

    RCP_DEBUG("parse_number_value: %d\n", typedefinition->type_id);

    switch (typedefinition->type_id)
    {
    case DATATYPE_INVALID:
    case DATATYPE_MAX_:
        break;

    case DATATYPE_BOOLEAN:
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
    }

    RCP_DEBUG("wrong type or not implemented: did not read value!");
    return NULL;
}

char* rcp_typedefinition_parse_string_value(rcp_typedefinition* typedefinition, char* data, size_t* size, rcp_option* opt)
{
    if (typedefinition == NULL) return data;

    RCP_DEBUG("parse_string_value: %d\n", typedefinition->type_id);

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

    RCP_DEBUG("wrong type: did not read value!");
    return NULL;
}


// return offset
char* parse_number_type_option(rcp_typedefinition* typedefinition, char* data, size_t* size, rcp_number_options number_option)
{
    if (typedefinition == NULL) return NULL;

    RCP_DEBUG("parse_number_type_option: %d\n", number_option);

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

    return 0;
}


char* parse_string_type_option(rcp_typedefinition* typedefinition, char* data, size_t* size, rcp_string_options option)
{
    if (typedefinition == NULL) return NULL;

    RCP_DEBUG("parse_string_type_option: %d\n", option);

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

    return 0;
}

static char* parse_bool_type_option(rcp_typedefinition* typedefinition, char* data, size_t* size, uint8_t option)
{
    if (typedefinition == NULL) return NULL;

    RCP_DEBUG("parse_bool_type_option: %d\n", option);

    rcp_option* opt;

    if (option == RCP_OPTIONS_DEFAULT)
    {
        opt = rcp_option_get_create(&typedefinition->options, RCP_OPTIONS_DEFAULT);
        return rcp_typedefinition_parse_number_value(typedefinition, data, size, opt);
    }

    return 0;
}

char* rcp_typedefinition_parse_type_options(rcp_typedefinition* typedefinition, char* data, size_t* size)
{
    if (typedefinition == NULL) return NULL;
    if (data == NULL || *size == 0) return NULL;

    if (typedefinition->type_id == 0
            || typedefinition->type_id >= DATATYPE_MAX_)
    {
        // invalid!
        RCP_ERROR("parse_type_options - invalid\n");
        return NULL;
    }



    uint8_t option_prefix = 0;

    while (*size > 0)
    {
        // reset
        option_prefix = 0;

        // read prefix
        data = rcp_read_u8(data, size, &option_prefix);
        if (data == NULL) return NULL;

        if (option_prefix == RCP_TERMINATOR)
        {
            // terminator - end of typedefinition
            RCP_DEBUG("parse_type_options - terminator\n");
            return data;
        }

        if (*size == 0) return NULL;

        RCP_DEBUG("parse_type_options - option_prefix: %d\n", option_prefix);

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
            // handle number option
            data = parse_number_type_option(typedefinition, data, size, option_prefix);
            break;

        case DATATYPE_STRING:
            data = parse_string_type_option(typedefinition, data, size, option_prefix);
            break;

        case DATATYPE_BANG:
        case DATATYPE_GROUP:
            // no options for this datatype
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
    size_t written_len;

    // write type id
    dst[0] = typedefinition->type_id;
    written += 1;

    if (written >= size) return 0;

    dst += 1;

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

            if (written_len)
            {
                written += written_len;

                if (written >= size) return 0;

                dst += written_len;
            }
        }

        opt = rcp_option_get_next(opt);
    }

    // write terminator
    memset(dst, 0, 1);
    written += 1;

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
        RCP_INFO("--------- type options:\n");

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
                log_number_option(opt, typedefinition->type_id);
                break;

            case DATATYPE_STRING:
                RCP_INFO("\toption: 0x%02x - %s\n", rcp_option_get_prefix(opt), rcp_option_get_string(opt, LONG_STRING));
                break;

            case DATATYPE_GROUP:
                RCP_INFO("\toption: 0x%02x\n", rcp_option_get_prefix(opt));
                break;

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
