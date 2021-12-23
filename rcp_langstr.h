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

#ifndef RCP_LANGUAGE_STRING_H
#define RCP_LANGUAGE_STRING_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"{
#endif

#include "rcp.h"

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
