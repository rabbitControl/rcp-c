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

#ifndef RCP_LANGUAGE_STRING_H
#define RCP_LANGUAGE_STRING_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stddef.h>
#include <stdbool.h>

#include "rcp.h"

//#define RCP_LANGUAGE_STRING_DEBUG_LOG
//#define RCP_LANGUAGE_STRING_MALLOC_DEBUG_LOG

#define RCP_LANGUAGE_CODE_SIZE 3
#define RCP_LANGUAGE_CODE_ANY "any"

typedef struct rcp_language_str rcp_language_str;

// create / free
rcp_language_str* rcp_langstr_create(const char* code);
rcp_language_str* rcp_langstr_copy(rcp_language_str* ls);
void rcp_langstr_free_chain(rcp_language_str* ls);

// chain
void rcp_langstr_set_next(rcp_language_str* ls, rcp_language_str* next);
rcp_language_str* rcp_langstr_get_next(rcp_language_str* ls);

// code
bool rcp_langstr_is_code(rcp_language_str* ls, const char* code);
const char* rcp_langstr_get_code(rcp_language_str* ls);

// setter / getter
void rcp_langstr_set_string(rcp_language_str* ls, const char* str, size_t str_len, rcp_string_types type); // full transfer
void rcp_langstr_copy_string(rcp_language_str* ls, const char* str, rcp_string_types type);

const char* rcp_langstr_get_string(rcp_language_str* ls);

// size and write
size_t rcp_langstr_get_size(rcp_language_str* ls);
size_t rcp_langstr_get_chain_size(rcp_language_str* ls);
size_t rcp_langstr_write(rcp_language_str* ls, char* dst, size_t size);


// logging
void rcp_langstr_log_chain(rcp_language_str* ls);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
