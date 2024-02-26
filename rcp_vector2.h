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

#ifndef RCP_VECTOR2_H
#define RCP_VECTOR2_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

#include "rcp_vector2_type.h"

//#define RCP_VECTOR_DEBUG_LOG
//#define RCP_VECTOR_MALLOC_DEBUG_LOG


rcp_vector2* rcp_vector2_create();
void rcp_vector2_free(rcp_vector2* vector);

size_t rcp_vector2_write(rcp_vector2* vector, char* data, size_t size);

void rcp_vector2_set_i(rcp_vector2* vector, int32_t x, int32_t y);
int32_t rcp_vector2_get_i_x(rcp_vector2* vector);
int32_t rcp_vector2_get_i_y(rcp_vector2* vector);

void rcp_vector2_set_f(rcp_vector2* vector, float x, float y);
float rcp_vector2_get_f_x(rcp_vector2* vector);
float rcp_vector2_get_f_y(rcp_vector2* vector);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // RCP_VECTOR2_H
