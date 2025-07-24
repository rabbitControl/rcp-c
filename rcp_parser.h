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

#ifndef RCP_PARSER_H
#define RCP_PARSER_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stdlib.h>

#include "rcp.h"
#include "rcp_parameter_type.h"

char* rcp_read_i8(char* data, size_t* size, int8_t* target);
char* rcp_read_u8(char* data, size_t* size, uint8_t* target);
char* rcp_read_i16(char* data, size_t* size, int16_t* target);
char* rcp_read_i32(char* data, size_t* size, int32_t* target);
char* rcp_read_i64(char* data, size_t* size, int64_t* target);
char* rcp_read_f32(char* data, size_t* size, float* target);
char* rcp_read_f64(char* data, size_t* size, double* target);

rcp_parameter* rcp_parse_parameter(char** data, size_t* size);
rcp_parameter* rcp_parse_value_update(char** data, size_t* size);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
