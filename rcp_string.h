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

#ifndef RCP_STRING_H
#define RCP_STRING_H

#ifdef __cplusplus
extern "C"{
#endif

#include "rcp_option.h"

#define RCP_TINY_STRING_MAX_SIZE 255
#define RCP_SHORT_STRING_MAX_SIZE 65535
#define RCP_LONG_STRING_MAX_SIZE 4294967295


char* rcp_read_tiny_string(char* data, size_t* size, char** target, uint8_t* str_length);
char* rcp_read_short_string(char* data, size_t* size, char** target, uint16_t* str_length);
char* rcp_read_long_string(char* data, size_t* size, char** target, uint32_t* str_length);

char* rcp_read_tiny_string_option(rcp_option** options, char* data, size_t* size, char option_prefix);
char* rcp_read_short_string_option(rcp_option** options, char* data, size_t* size, char option_prefix);


uint8_t rcp_write_tiny_string(char* dst, const char* str);
uint16_t rcp_write_short_string(char* dst, const char* str);
uint32_t rcp_write_long_string(char* dst, const char* str);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
