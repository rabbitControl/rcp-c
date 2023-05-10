/*
********************************************************************
* rabbitcontrol - a protocol and data-format for remote control.
*
* https://rabbitcontrol.cc
* https://github.com/rabbitcontrol/rcp-c
*
* This file is part of rabbitcontrol for c.
*
* Written by Ingo Randolf, 2021 - 2023
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*********************************************************************
*/

#ifndef RCP_STRING_LIST_H
#define RCP_STRING_LIST_H


#ifdef __cplusplus
extern "C"{
#endif

#include <string.h>
#include <stdint.h>
#include <stdarg.h>


//#define RCP_STRINGLIST_DEBUG_LOG

typedef struct rcp_stringlist rcp_stringlist;

rcp_stringlist* rcp_stringlist_create(int count, ...);
rcp_stringlist* rcp_stringlist_create_args(int count, va_list args);
void rcp_stringlist_free(rcp_stringlist* list);

void rcp_stringlist_append(rcp_stringlist* list, const char* string); // copy
void rcp_stringlist_append_put(rcp_stringlist* list, char* string); // full transfer

size_t rcp_stringlist_get_size(rcp_stringlist* list);
size_t rcp_stringlist_write(rcp_stringlist* list, char* dst, size_t size);

void rcp_stringlist_log(rcp_stringlist* list);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
