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

#ifndef RCP_STRING_H
#define RCP_STRING_H

#ifdef __cplusplus
extern "C"{
#endif

#include "rcp_option.h"

#define RCP_TINY_STRING_MAX_SIZE 255
#define RCP_SHORT_STRING_MAX_SIZE 65535
#define RCP_LONG_STRING_MAX_SIZE 4294967295

//#define RCP_STRING_DEBUG_LOG
//#define RCP_STRING_MALLOC_DEBUG_LOG


const char* rcp_read_tiny_string(const char* data, size_t* size, char** target, uint8_t* str_length);
const char* rcp_read_short_string(const char* data, size_t* size, char** target, uint16_t* str_length);
const char* rcp_read_long_string(const char* data, size_t* size, char** target, uint32_t* str_length);

const char* rcp_read_tiny_string_option(rcp_option** options, const char* data, size_t* size, char option_prefix);
const char* rcp_read_short_string_option(rcp_option** options, const char* data, size_t* size, char option_prefix);


size_t rcp_write_tiny_string(char* dst, size_t size, const char* str);
size_t rcp_write_short_string(char* dst, size_t size, const char* str);
size_t rcp_write_long_string(char* dst, size_t size, const char* str);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
