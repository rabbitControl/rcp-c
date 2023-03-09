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

#ifndef RCP_SPPP_H
#define RCP_SPPP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

#ifndef MACRO_U32_FROM_4U8
    #define MACRO_U32_FROM_4U8(a, b, c, d) ((d << 24)&0xff000000 | (c << 16)&0x00ff0000 | (b << 8)&0x0000ff00 | (a)&0x000000ff)
#endif

// size prefixed packet parser
typedef struct rcp_sppp rcp_sppp;

// create / free
rcp_sppp* rcp_sppp_create(size_t max_size, void (*packet_cb)(char*, size_t, void*), void* user);
void rcp_sppp_free(rcp_sppp* pp);

// reset sppp
void rcp_sppp_reset(rcp_sppp* pp);

// callbacks
void rcp_sppp_set_packet_cb(rcp_sppp* pp, void (*packet_cb)(char*, size_t, void*), void* user);
void rcp_sppp_set_bypass_cb(rcp_sppp* pp, void (*data_cb)(char*, size_t, void*), void (*done_cb)(void*));

// new data in
void rcp_sppp_data(rcp_sppp* pp, char* data, size_t size);

// control data bypass
void rcp_sppp_set_bypass(rcp_sppp* pp, unsigned char bypass);
char rcp_sppp_get_bypass(rcp_sppp* pp);

// get packet size
size_t rcp_sppp_get_packet_size(rcp_sppp* pp);

#ifdef __cplusplus
}
#endif

#endif
