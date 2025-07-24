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

#define RCP_CUSTOMTYPE_UUID_LENGTH 16


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
size_t rcp_typedefinition_write_mandatory(rcp_typedefinition* typedefinition, char* dst, size_t size);

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

bool rcp_typedefinition_set_option_data(rcp_typedefinition* typedefinition, char prefix, const char* data, size_t size, bool sizeprefixed); // no copy
void rcp_typedefinition_get_option_data(rcp_typedefinition* typedefinition, char prefix, const char** out_data, size_t* out_size); // no copy


bool rcp_typedefinition_get_option_bool(rcp_typedefinition* typedefinition, char prefix, bool defaultValue);
int8_t rcp_typedefinition_get_option_i8(rcp_typedefinition* typedefinition, char prefix, int8_t defaultValue);
int16_t rcp_typedefinition_get_option_i16(rcp_typedefinition* typedefinition, char prefix, int16_t defaultValue);
int32_t rcp_typedefinition_get_option_i32(rcp_typedefinition* typedefinition, char prefix, int32_t defaultValue);
float rcp_typedefinition_get_option_f32(rcp_typedefinition* typedefinition, char prefix, float defaultValue);

float rcp_typedefinition_get_option_v2f32_x(rcp_typedefinition* typedefinition, char prefix, float defaultValue);
float rcp_typedefinition_get_option_v2f32_y(rcp_typedefinition* typedefinition, char prefix, float defaultValue);

// custom
void rcp_typedefinition_custom_set_size(rcp_typedefinition_custom* typedefinition, uint32_t size);
uint32_t rcp_typedefinition_custom_get_size(rcp_typedefinition_custom* typedefinition);


const char* rcp_typedefinition_get_option_string_tiny(rcp_typedefinition* typedefinition, char prefix); // no transfer
rcp_stringlist* rcp_typedefinition_get_option_stringlist(rcp_typedefinition* typedefinition, char prefix); // no transfer

// logging
void rcp_typedefinition_log(rcp_typedefinition* typedefinition);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
