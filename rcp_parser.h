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

#ifndef RCP_PARSER_H
#define RCP_PARSER_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C"{
#endif

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
