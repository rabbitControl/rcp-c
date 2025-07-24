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

#include "rcp_string.h"

#include <string.h>

#include "rcp_types.h"
#include "rcp_memory.h"
#include "rcp_parser.h"
#include "rcp_logging.h"
#include "rcp_endian.h"

#if defined(RCP_STRING_DEBUG_LOG) || defined(RCP_ALL_DEBUG)
#define RCP_STRING_DEBUG(...) RCP_DEBUG(__VA_ARGS__)
#else
#define RCP_STRING_DEBUG(...)
#endif

#if defined(RCP_STRING_MALLOC_DEBUG_LOG) || defined(RCP_ALL_DEBUG)
#define RCP_STRING_MALLOC_DEBUG(...) RCP_DEBUG(__VA_ARGS__)
#else
#define RCP_STRING_MALLOC_DEBUG(...)
#endif

// read tiny string from data and store it into option
char* rcp_read_tiny_string_option(rcp_option** options, char* data, size_t* size, char option_prefix)
{
    if (options == NULL) return NULL;

    uint8_t str_len = 0;
    char* string_data = NULL;

    // copy string from data into string_data
    data = rcp_read_tiny_string(data, size, &string_data, &str_len);

    if (data &&
            str_len > 0 &&
            string_data != NULL)
    {
        RCP_STRING_DEBUG("tiny string: %s\n", string_data);

        rcp_option* opt = rcp_option_get_create(options, option_prefix);
        rcp_option_free_data(opt);

        // full transfer
        rcp_option_move_string(opt, string_data, TINY_STRING);
    }
    else
    {
        RCP_STRING_DEBUG("error reading tiny string: %s\n", string_data);
    }

    return data;
}

// read short string from data and store it into option
char* rcp_read_short_string_option(rcp_option** options, char* data, size_t* size, char option_prefix)
{
    if (options == NULL) return NULL;

    uint16_t str_len;
    char* string_data = NULL;

    data = rcp_read_short_string(data, size, &string_data, &str_len);

    if (data &&
            str_len > 0 &&
            string_data != NULL)
    {
        RCP_STRING_DEBUG("short string: %s\n", string_data);

        rcp_option* opt = rcp_option_get_create(options, option_prefix);
        rcp_option_free_data(opt);

        // full transfer
        rcp_option_move_string(opt, string_data, SHORT_STRING);
    }
    else
    {
        RCP_STRING_DEBUG("error reading short string: %s\n", string_data);
    }

    return data;
}

// copy tiny-string from data into target
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
            RCP_STRING_MALLOC_DEBUG("*** tiny string: %p\n", (void*)*target);

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
            RCP_STRING_MALLOC_DEBUG("*** short string: %p\n", (void*)*target);

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
            RCP_STRING_MALLOC_DEBUG("*** long string [%d]: %p\n", *str_length, (void*)*target);

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


size_t rcp_write_tiny_string(char* dst, size_t size, const char* str)
{
    if (dst == NULL) return 0;
    if (size == 0) return 0;

    RCP_STRING_DEBUG("rcp_write_tiny_string: %s\n", (str != NULL ? str : "null"));

    size_t str_len = str != NULL ? strlen(str) : 0;

    if (str_len > RCP_TINY_STRING_MAX_SIZE)
    {
        str_len = RCP_TINY_STRING_MAX_SIZE;
    }

    if (size < (str_len+TINY_STRING))
    {
        RCP_ERROR("write tiny string: insufficient memory\n")
        return 0;
    }

    // write size
    dst[0] = (char)str_len;

    if (str_len > 0)
    {
        memcpy(dst+TINY_STRING, str, str_len);
    }

    // return offset
    return str_len + TINY_STRING;
}

size_t rcp_write_short_string(char* dst, size_t size, const char* str)
{
    if (dst == NULL) return 0;
    if (size == 0) return 0;

    RCP_STRING_DEBUG("rcp_write_short_string: %s\n", (str != NULL ? str : "null"));

    size_t str_len = str != NULL ? strlen(str) : 0;

    if (str_len > RCP_SHORT_STRING_MAX_SIZE)
    {
        str_len = RCP_SHORT_STRING_MAX_SIZE;
    }

    if (size < (str_len+SHORT_STRING))
    {
        RCP_ERROR("write short string: insufficient memory\n")
        return 0;
    }

    // write size
    _rcp_store16(dst, (uint16_t)str_len);

    if (str_len > 0)
    {
        memcpy(dst+SHORT_STRING, str, str_len);
    }

    // return offset
    return str_len + SHORT_STRING;
}

size_t rcp_write_long_string(char* dst, size_t size, const char* str)
{
    if (dst == NULL) return 0;
    if (size == 0) return 0;

    RCP_STRING_DEBUG("rcp_write_long_string: %s\n", (str != NULL ? str : "null"));

    size_t str_len = str != NULL ? strlen(str) : 0;

    if (str_len > RCP_LONG_STRING_MAX_SIZE)
    {
        str_len = RCP_LONG_STRING_MAX_SIZE;
    }

    if (size < (str_len+LONG_STRING))
    {
        RCP_ERROR("write long string: insufficient memory\n")
        return 0;
    }

    // write size
    _rcp_store32(dst, (uint32_t)str_len);

    if (str_len > 0)
    {
        memcpy(dst+LONG_STRING, str, str_len);
    }

    // return offset
    return str_len + LONG_STRING;
}
