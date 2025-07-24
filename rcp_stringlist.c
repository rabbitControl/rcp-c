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

#include "rcp_stringlist.h"

#include "rcp_memory.h"
#include "rcp_logging.h"
#include "rcp_string.h"

#if defined(RCP_STRINGLIST_DEBUG_LOG) || defined(RCP_ALL_DEBUG)
#define RCP_STRINGLIST_DEBUG(...) RCP_DEBUG(__VA_ARGS__)
#else
#define RCP_STRINGLIST_DEBUG(...)
#endif

struct rcp_stringlist
{
    char** strings;
    int count;
};

static void _rcp_stringlist_copy_string(char** dst, const char* src)
{
    size_t len = strlen(src);
    if (len > RCP_TINY_STRING_MAX_SIZE)
    {
        len = RCP_TINY_STRING_MAX_SIZE;
    }

    *dst = (char*)RCP_CALLOC(1, len + 1);

    if (*dst)
    {
        RCP_STRINGLIST_DEBUG("*** stringlist string: %p\n", *dst);

        // copy string
        strncpy(*dst, src, len);
    }
    else
    {
        RCP_ERROR("could not calloc for string: %lu\n", len);
    }
}

rcp_stringlist* rcp_stringlist_create(int count, ...)
{
    rcp_stringlist* list = NULL;

    va_list valist;
    va_start(valist, count);

    list = rcp_stringlist_create_args(count, valist);

    va_end(valist);

    return list;
}

rcp_stringlist* rcp_stringlist_create_args(int count, va_list args)
{
    uint16_t i;

    rcp_stringlist* list = (rcp_stringlist*)RCP_CALLOC(1, sizeof(rcp_stringlist));

    if (list)
    {
        RCP_STRINGLIST_DEBUG("*** stringlist: %p\n", list);

        list->count = count;

        if (count > 0)
        {
            list->strings = (char**)RCP_CALLOC(count, sizeof(char*));

            if (list->strings)
            {
                RCP_STRINGLIST_DEBUG("*** stringlist strings: %p\n", list->strings);

                for (i = 0; i < count; i++)
                {
                    const char* s = va_arg(args, char*);
                    _rcp_stringlist_copy_string(&list->strings[i], s);
                }
            }
        }

    }

    return list;
}

// copy
void rcp_stringlist_append(rcp_stringlist* list, const char* string)
{
    if (list == NULL) return;

    list->strings = (char**)RCP_REALLOC(list->strings, (list->count + 1) * sizeof(char*));

    if (list->strings)
    {
        RCP_STRINGLIST_DEBUG("*** stringlist strings: %p\n", list->strings);

        _rcp_stringlist_copy_string(&list->strings[list->count], string);

        list->count++;
    }
}

// full transfer
void rcp_stringlist_append_put(rcp_stringlist* list, char* string)
{
    if (list == NULL) return;

    list->strings = (char**)RCP_REALLOC(list->strings, (list->count + 1) * sizeof(char*));

    if (list->strings)
    {
        RCP_STRINGLIST_DEBUG("*** stringlist strings: %p\n", list->strings);

        list->strings[list->count] = string;

        list->count++;
    }
}

void rcp_stringlist_free(rcp_stringlist* list)
{
    int i;
    if (list == NULL) return;

    if (list->strings)
    {
        // TODO: free strings
        for (i = 0; i < list->count; i++)
        {
            if (list->strings[i])
            {
                RCP_STRINGLIST_DEBUG("+++ stringlist string: %p\n", list->strings[i]);
                RCP_FREE(list->strings[i]);
            }
        }

        RCP_STRINGLIST_DEBUG("+++ liststring strings: %p\n", list->strings);
        RCP_FREE(list->strings);
    }

    RCP_STRINGLIST_DEBUG("+++ liststring: %p\n", list);
    RCP_FREE(list);
}

// get serialized size
size_t rcp_stringlist_get_size(rcp_stringlist* list)
{
    size_t size = 0;
    int i;
    if (list == NULL) return 0;

    for (i = 0; i < list->count; i++)
    {
        size_t len = strlen(list->strings[i]);
        if (len > RCP_TINY_STRING_MAX_SIZE)
        {
            len = RCP_TINY_STRING_MAX_SIZE;
        }

        size += 1; // size prefix
        size += len; // string length
    }

    size += 1; // terminator

    return size;
}

size_t rcp_stringlist_write(rcp_stringlist* list, char* data, size_t size)
{
    int i;
    size_t written = 0;

    if (list == NULL) return 0;

    for (i = 0; i < list->count; i++)
    {
        size_t len = strlen(list->strings[i]);
        if (len > RCP_TINY_STRING_MAX_SIZE)
        {
            len = RCP_TINY_STRING_MAX_SIZE;
        }

        if (size <= (len + 1))
        {
            RCP_STRINGLIST_DEBUG("not enough bytes to write stringlist string!\n");
            return 0;
        }

        *data = len;

        memcpy(data + 1, list->strings[i], len);
        written += len + 1;
        data += len + 1;
        size -= len + 1;
    }

    if (size == 0)
    {
        RCP_STRINGLIST_DEBUG("not enough bytes to write stringlist terminator!\n");
        return 0;
    }

    *data = RCP_TERMINATOR;

    return written + 1;
}

void rcp_stringlist_log(rcp_stringlist* list)
{
    int i;
    if (list == NULL) return;

    RCP_INFO("string list: %d : ", list->count);

    for (i = 0; i < list->count; i++)
    {
        RCP_INFO_ONLY("%s ", list->strings[i]);
    }
    RCP_INFO_ONLY("\n");
}
