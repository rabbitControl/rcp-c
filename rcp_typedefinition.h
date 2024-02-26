/*
********************************************************************
* rabbitcontrol - a protocol and data-format for remote control.
*
* https://rabbitcontrol.cc
* https://github.com/rabbitcontrol/rcp-c
*
* This file is part of rabbitcontrol for c.
*
* Written by Ingo Randolf, 2021 - 2024
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*********************************************************************
*/

#ifndef RCP_TYPEDEFINITION_H
#define RCP_TYPEDEFINITION_H


#ifdef __cplusplus
extern "C"{
#endif

#include <stdlib.h>
#include <stdarg.h>

#include "rcp.h"
#include "rcp_typedefinition_type.h"
#include "rcp_option_type.h"
#include "rcp_stringlist.h"

//#define RCP_TYPEDEFINITION_DEBUG_LOG
//#define RCP_TYPEDEFINITION_MALLOC_DEBUG_LOG


// create / free
rcp_typedefinition* rcp_typedefinition_create(rcp_datatype type_id);
void rcp_typedefinition_free(rcp_typedefinition* typedefinition);

// type id
rcp_datatype rcp_typedefinition_get_type_id(rcp_typedefinition* typedefinition);

// parse
char* rcp_typedefinition_parse_number_value(rcp_typedefinition* typedefinition, char* data, size_t* size, rcp_option* opt);
char* rcp_typedefinition_parse_string_value(rcp_typedefinition* typedefinition, char* data, size_t* size, rcp_option* opt);
char* rcp_typedefinition_parse_type_options(rcp_typedefinition* typedefinition, char* data, size_t* size);

// size and writing
size_t rcp_typedefinition_get_size(rcp_typedefinition* typedefinition, bool all);
size_t rcp_typedefinition_write(rcp_typedefinition* typedefinition, char* dst, size_t size, bool all);

// options
bool rcp_typedefinition_has_option(rcp_typedefinition* typedefinition, char prefix);
void rcp_typedefinition_all_options_changed(rcp_typedefinition* typedefinition);
void rcp_typedefinition_all_options_unchanged(rcp_typedefinition* typedefinition);
bool rcp_typedefinition_changed(rcp_typedefinition* typedefinition);

// setter / getter
bool rcp_typedefinition_set_option_bool(rcp_typedefinition* typedefinition, char prefix, bool value);
bool rcp_typedefinition_set_option_i8(rcp_typedefinition* typedefinition, char prefix, int8_t value);
bool rcp_typedefinition_set_option_i16(rcp_typedefinition* typedefinition, char prefix, int16_t value);
bool rcp_typedefinition_set_option_i32(rcp_typedefinition* typedefinition, char prefix, int32_t value);
bool rcp_typedefinition_set_option_f32(rcp_typedefinition* typedefinition, char prefix, float value);

bool rcp_typedefinition_set_option_v2f32(rcp_typedefinition* typedefinition, char prefix, float x, float y);

bool rcp_typedefinition_set_option_string_tiny(rcp_typedefinition* typedefinition, char prefix, const char* value); // copy
bool rcp_typedefinition_set_option_stringlist(rcp_typedefinition* typedefinition, char prefix, int count, va_list args); // copy



bool rcp_typedefinition_get_option_bool(rcp_typedefinition* typedefinition, char prefix, bool defaultValue);
int8_t rcp_typedefinition_get_option_i8(rcp_typedefinition* typedefinition, char prefix, int8_t defaultValue);
int16_t rcp_typedefinition_get_option_i16(rcp_typedefinition* typedefinition, char prefix, int16_t defaultValue);
int32_t rcp_typedefinition_get_option_i32(rcp_typedefinition* typedefinition, char prefix, int32_t defaultValue);
float rcp_typedefinition_get_option_f32(rcp_typedefinition* typedefinition, char prefix, float defaultValue);

float rcp_typedefinition_get_option_v2f32_x(rcp_typedefinition* typedefinition, char prefix, float defaultValue);
float rcp_typedefinition_get_option_v2f32_y(rcp_typedefinition* typedefinition, char prefix, float defaultValue);



const char* rcp_typedefinition_get_option_string_tiny(rcp_typedefinition* typedefinition, char prefix); // no transfer
rcp_stringlist* rcp_typedefinition_get_option_stringlist(rcp_typedefinition* typedefinition, char prefix); // no transfer

// logging
void rcp_typedefinition_log(rcp_typedefinition* typedefinition);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
