/*
********************************************************************
* rabbitcontrol - a protocol and data-format for remote control.
*
* https://rabbitcontrol.cc
* https://github.com/rabbitcontrol/rcp-c
*
* This file is part of rabbitcontrol for c.
*
* Written by Ingo Randolf, 2021
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*********************************************************************
*/

#include "rcp_string.h"

#include <string.h>

#include "types.h"
#include "rcp_memory.h"
#include "rcp_parser.h"
#include "rcp_logging.h"
#include "rcp_endian.h"

char* rcp_read_tiny_string_option(rcp_option** options, char* data, size_t* size, char option_prefix)
{
    if (options == NULL) return NULL;

    uint8_t str_len;
    char* string_data = NULL;

    char* r_data = rcp_read_tiny_string(data, size, &string_data, &str_len);
    if (r_data == NULL) return NULL;

    data = r_data;

    RCP_DEBUG("tiny string: %s\n", string_data);

    if (str_len > 0
            && string_data != NULL)
    {
        rcp_option* opt = rcp_option_get_create(options, option_prefix);
        rcp_option_free_data(opt);       

        // full transfer
        rcp_option_move_string(opt, string_data, TINY_STRING);
    }

    return data;
}

char* rcp_read_short_string_option(rcp_option** options, char* data, size_t* size, char option_prefix)
{
    uint16_t str_len;
    char* string_data = NULL;

    if (options == NULL) return NULL;

    char* r_data = rcp_read_short_string(data, size, &string_data, &str_len);
    if (r_data == NULL) return NULL;

    data = r_data;

    RCP_DEBUG("short string: %s\n", string_data);

    if (str_len > 0
            && string_data != NULL)
    {
        rcp_option* opt = rcp_option_get_create(options, option_prefix);
        rcp_option_free_data(opt);

        // full transfer
        rcp_option_move_string(opt, string_data, SHORT_STRING);
    }

    return data;
}

char* rcp_read_tiny_string(char* data, size_t* size, char** target, uint8_t* str_length)
{
    if (data == NULL) return NULL;
    if (target == NULL) return NULL;
    if (*size < 1) return NULL;

    // read string length
    data = rcp_read_u8(data, size, str_length);
    if (data == NULL) return NULL;

    // check if we have more data
    if (*size == 0) return NULL;

    if (*str_length > 0)
    {
        *target = (char*)RCP_CALLOC(1, *str_length + 1);
        if (*target != NULL)
        {
            RCP_DEBUG("*** tiny string: %p\n", (void*)*target);

            // copy data
            strncpy(*target, data, *str_length);

            // decrease size
            *size -= *str_length;
            // return new location in data
            return data + *str_length;
        }
        else
        {
            RCP_ERROR("could not malloc for tiny string\n");
        }
    }
    else
    {
        *target = NULL;
    }

    // 0 length string - just return data
    return data;
}

char* rcp_read_short_string(char* data, size_t* size, char** target, uint16_t* str_length)
{
    if (data == NULL) return NULL;
    if (target == NULL) return NULL;
    if (*size < 2) return NULL;

    // read string length
    data = rcp_read_i16(data, size, (int16_t*)str_length);
    if (data == NULL) return NULL;

    // check if we have more data
    if (*size == 0) return NULL;

    if (*str_length > 0)
    {
        *target = RCP_CALLOC(1, *str_length + 1);
        if (*target != NULL)
        {
            RCP_DEBUG("*** short string: %p\n", (void*)*target);

            // copy data
            strncpy(*target, data, *str_length);

            // decrease size
            *size -= *str_length;
            // return new location in data
            return data + *str_length;
        }
        else
        {
            RCP_ERROR("could not malloc for short string\n");
        }
    }
    else
    {
        *target = NULL;
    }

    // 0 length string - just return data
    return data;
}

char* rcp_read_long_string(char* data, size_t* size, char** target, uint32_t* str_length)
{
    if (data == NULL) return NULL;
    if (target == NULL) return NULL;
    if (*size < 4) return NULL;

    // read string length
    data = rcp_read_i32(data, size, (int32_t*)str_length);
    if (data == NULL) return NULL;

    // check if we have more data
    if (*size == 0) return NULL;

    if (*str_length > 0)
    {
        // zero terminated string
        *target = (char*)RCP_CALLOC(1, *str_length + 1);
        if (*target != NULL)
        {
            RCP_DEBUG("*** long string [%d]: %p\n", *str_length, (void*)*target);

            // copy data
            strncpy(*target, data, *str_length);

            // decrease size
            *size -= *str_length;
            // return new location in data
            return data + *str_length;
        }
        else
        {
            RCP_ERROR("could not malloc for long string\n");
        }
    }
    else
    {
        *target = NULL;
    }

    // 0 length string - just return data
    return data;
}


uint8_t rcp_write_tiny_string(char* dst, const char* str)
{
    if (dst == NULL) return 0;

    size_t str_len = str != NULL ? strlen(str) : 0;

    if (str_len > RCP_TINY_STRING_MAX_SIZE)
    {
        str_len = RCP_TINY_STRING_MAX_SIZE;
    }

    // write size
    dst[0] = (char)str_len;

    if (str_len > 0)
    {
        memcpy(dst+TINY_STRING, str, str_len);
    }

    // return offset
    return (uint8_t)str_len + TINY_STRING;
}

uint16_t rcp_write_short_string(char* dst, const char* str)
{
    if (dst == NULL) return 0;

    size_t str_len = str != NULL ? strlen(str) : 0;

    if (str_len > RCP_SHORT_STRING_MAX_SIZE)
    {
        str_len = RCP_SHORT_STRING_MAX_SIZE;
    }

    _rcp_store16(dst, (uint16_t)str_len);

    if (str_len > 0)
    {
        memcpy(dst+SHORT_STRING, str, str_len);
    }

    // return offset
    return (uint16_t)str_len + SHORT_STRING;
}

uint32_t rcp_write_long_string(char* dst, const char* str)
{
    if (dst == NULL) return 0;

    RCP_DEBUG("write_long_string: %s\n", (str != NULL ? str : "null"));

    size_t str_len = str != NULL ? strlen(str) : 0;

    if (str_len > RCP_LONG_STRING_MAX_SIZE)
    {
        str_len = RCP_LONG_STRING_MAX_SIZE;
    }

    _rcp_store32(dst, (uint32_t)str_len);

    if (str_len > 0)
    {
        memcpy(dst+LONG_STRING, str, str_len);
    }

    // return offset
    return (uint16_t)str_len + LONG_STRING;
}
