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

#ifndef RCP_SPPP_H
#define RCP_SPPP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

//#define RCP_SPPP_DEBUG_LOG
//#define RCP_SPPP_MALLOC_DEBUG_LOG

#ifndef MACRO_U32_FROM_4U8
    #define MACRO_U32_FROM_4U8(a, b, c, d) ((d << 24)&0xff000000 | (c << 16)&0x00ff0000 | (b << 8)&0x0000ff00 | (a)&0x000000ff)
#endif

// size prefixed packet parser
typedef struct rcp_sppp rcp_sppp;

// create / free
rcp_sppp* rcp_sppp_create(size_t max_size, void (*packet_cb)(const char*, size_t, void*), void* user);
void rcp_sppp_free(rcp_sppp* pp);

// reset sppp
void rcp_sppp_reset(rcp_sppp* pp);

// callbacks
void rcp_sppp_set_packet_cb(rcp_sppp* pp, void (*packet_cb)(const char*, size_t, void*), void* user);
void rcp_sppp_set_bypass_cb(rcp_sppp* pp, void (*data_cb)(const char*, size_t, void*), void (*done_cb)(void*));

// new data in
void rcp_sppp_data(rcp_sppp* pp, const char* data, size_t size);

// control data bypass
void rcp_sppp_set_bypass(rcp_sppp* pp, unsigned char bypass);
char rcp_sppp_get_bypass(rcp_sppp* pp);

// report zero size packets
void rcp_sppp_set_report_zerosize(rcp_sppp* pp);
void rcp_sppp_clear_report_zerosize(rcp_sppp* pp);

// get packet size
size_t rcp_sppp_get_packet_size(rcp_sppp* pp);

#ifdef __cplusplus
}
#endif

#endif
