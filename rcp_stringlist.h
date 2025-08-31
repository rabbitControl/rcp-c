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
void rcp_stringlist_append_put(rcp_stringlist* list, const char* string); // full transfer

size_t rcp_stringlist_get_size(rcp_stringlist* list);
size_t rcp_stringlist_write(rcp_stringlist* list, char* dst, size_t size);

void rcp_stringlist_log(rcp_stringlist* list);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
