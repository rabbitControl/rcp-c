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
