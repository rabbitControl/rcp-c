/*
********************************************************************
* RabbitControl - a protocol for remote control.
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

#include "rcp_langstr.h"

#include <string.h>

#include "rcp_memory.h"
#include "rcp_string.h"
#include "rcp_logging.h"

#if defined(RCP_LANGUAGE_STRING_DEBUG_LOG) || defined(RCP_ALL_DEBUG)
#define RCP_LANGUAGE_STRING_DEBUG(...) RCP_DEBUG(__VA_ARGS__)
#else
#define RCP_LANGUAGE_STRING_DEBUG(...)
#endif

#if defined(RCP_LANGUAGE_STRING_MALLOC_DEBUG_LOG) || defined(RCP_ALL_DEBUG)
#define RCP_LANGUAGE_STRING_MALLOC_DEBUG(...) RCP_DEBUG(__VA_ARGS__)
#else
#define RCP_LANGUAGE_STRING_MALLOC_DEBUG(...)
#endif

struct rcp_language_str
{
    rcp_language_str* next;

    // zero-terminated c-string
    char* str;

    // size when serialized! (not length of string!)
    size_t length;

    // string type: string-tiny, string-short, string-long
    rcp_string_types type;

    // the 3-character language code (not 0-terminated)
    char code[RCP_LANGUAGE_CODE_SIZE];
};



rcp_language_str* rcp_langstr_create(const char* code)
{
    if (code == NULL) return NULL;

    rcp_language_str* lng_str = RCP_CALLOC(1, sizeof(rcp_language_str));

    if (lng_str)
    {
        RCP_LANGUAGE_STRING_MALLOC_DEBUG("*** langstr: %p\n", lng_str);

        strncpy(lng_str->code, code, RCP_LANGUAGE_CODE_SIZE);
    }

    return lng_str;
}


rcp_language_str* rcp_langstr_copy(rcp_language_str* ls)
{
    // recreate language string chain
    rcp_language_str* dst_lng_str = NULL;
    rcp_language_str* src_lng_str = ls;

    while (src_lng_str)
    {
        // create a new lng-string
        rcp_language_str* new_lng_str = rcp_langstr_create(rcp_langstr_get_code(src_lng_str));
        if (new_lng_str == NULL)
        {
            RCP_LANGUAGE_STRING_DEBUG("could not create language string\n");
            rcp_langstr_free_chain(dst_lng_str);
            return NULL;
        }

        rcp_langstr_copy_string(new_lng_str, src_lng_str->str, src_lng_str->type);
        new_lng_str->next = dst_lng_str;
        dst_lng_str = new_lng_str;

        // next
        src_lng_str = src_lng_str->next;
    }

    return dst_lng_str;
}



void rcp_langstr_free_chain(rcp_language_str* ls)
{
    RCP_LANGUAGE_STRING_DEBUG("||| free_langstr_chain: %p\n", ls)
    rcp_language_str* next;

    while (ls)
    {
        next = ls->next;

        if (ls->str != NULL)
        {
            RCP_LANGUAGE_STRING_MALLOC_DEBUG("+++ langstr str: %p\n", ls->str);
            RCP_FREE(ls->str);
        }

        RCP_LANGUAGE_STRING_MALLOC_DEBUG("+++ langstr: %p\n", ls);
        RCP_FREE(ls);

        ls = next;
    }
}


void rcp_langstr_log_chain(rcp_language_str* ls)
{
    while (ls)
    {
        RCP_INFO_ONLY("str [%s]: %s\n", ls->code, ls->str);
        ls = ls->next;
    }
}


void rcp_langstr_set_next(rcp_language_str* ls, rcp_language_str* next)
{
    if (ls == NULL) return;
    ls->next = next;
}

rcp_language_str* rcp_langstr_get_next(rcp_language_str* ls)
{
    if (ls == NULL) return NULL;
    return ls->next;
}


size_t rcp_langstr_get_size(rcp_language_str* ls)
{
    if (ls == NULL) return 0;

    return ls->length;
}

size_t rcp_langstr_get_chain_size(rcp_language_str* ls)
{
    size_t size = 1; // terminator at end
    while (ls)
    {
        size += ls->length;
        ls = ls->next;
    }

    return size;
}


bool rcp_langstr_is_code(rcp_language_str* ls, const char* code)
{
    if (ls == NULL) return false;

    return strcmp(ls->code, code) == 0;
}

const char* rcp_langstr_get_code(rcp_language_str* ls)
{
    if (ls == NULL) return NULL;

    return ls->code;
}



static void _langstr_free_str(rcp_language_str* ls)
{
    if (ls != NULL &&
            ls->str != NULL)
    {
        RCP_LANGUAGE_STRING_MALLOC_DEBUG("+++ string: %p\n", ls->str);
        RCP_FREE((void*)ls->str);

        ls->str = NULL;
        ls->length = 0;
    }
}


/* rcp_langstr_set_string
 *  move str into langstr:
 *  langstr "owns" the data and attempts to free it
 */
// full transfer
void rcp_langstr_set_string(rcp_language_str* ls, const char* str, size_t str_len, rcp_string_types type)
{
    if (ls == NULL) return;

    _langstr_free_str(ls);

    ls->str = (char*)str;
    ls->length = str_len + type + RCP_LANGUAGE_CODE_SIZE;
    ls->type = type;
}

/* rcp_langstr_copy_string
 *  copy str into langstr:
 *  langstr "owns" the data and attempts to free it
 */
void rcp_langstr_copy_string(rcp_language_str* ls, const char* str, rcp_string_types type)
{
    if (ls == NULL) return;

    _langstr_free_str(ls);

    // malloc
    size_t str_len = strlen(str);
    if (str_len > 0)
    {
        ls->str = (char*)RCP_CALLOC(1, str_len + 1);

        if (ls->str != NULL)
        {
            RCP_LANGUAGE_STRING_MALLOC_DEBUG("*** string data: %p\n", ls->str);

            strncpy(ls->str, str, str_len);

            ls->length = str_len + type + RCP_LANGUAGE_CODE_SIZE;
            ls->type = type;

//            RCP_LANGUAGE_STRING_DEBUG("string data: %s\n", ls->str);
//            RCP_LANGUAGE_STRING_DEBUG("lng string length: %d\n", ls->length);
        }
        else
        {
            RCP_ERROR("could not allocate for string\n");
        }
    }
}

const char* rcp_langstr_get_string(rcp_language_str* ls)
{
    if (ls == NULL) return NULL;
    return ls->str;
}

// write whole language-string chain
size_t rcp_langstr_write(rcp_language_str* lng_str, char* data, size_t size)
{
    size_t written = 0;
    size_t written_len = 0;

    while (lng_str)
    {
        // TODO: check size first

        // write language code
        memcpy(data, rcp_langstr_get_code(lng_str), RCP_LANGUAGE_CODE_SIZE);
        written += RCP_LANGUAGE_CODE_SIZE;

        if (written >= size)
        {
            RCP_LANGUAGE_STRING_DEBUG("offset >= data_size! 1\n");
            return 0;
        }

        data += RCP_LANGUAGE_CODE_SIZE;

        switch (lng_str->type)
        {
        case TINY_STRING:
            written_len = rcp_write_tiny_string(data, (size - written), lng_str->str);
            break;
        case SHORT_STRING:
            written_len = rcp_write_short_string(data, (size - written), lng_str->str);
            break;
        case LONG_STRING:
            written_len = rcp_write_long_string(data, (size - written), lng_str->str);
            break;
        }

        written += written_len;

        if (written >= size)
        {
            RCP_LANGUAGE_STRING_DEBUG("offset >= data_size! 2\n");
            return 0;
        }

        data += written_len;

        lng_str = rcp_langstr_get_next(lng_str);
    }

    // write terminator
    memset(data, 0, 1);
    written += 1;

    return written;
}
