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

#ifndef RCP_SLIP_H
#define RCP_SLIP_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stddef.h>

/*
 * SLIP special character codes
 * see: https://tools.ietf.org/html/rfc1055
*/
#define END             0300    /* indicates end of packet (0xC0, 192) */
#define ESC             0333    /* indicates byte stuffing (0xDB, 219) */
#define ESC_END         0334    /* ESC ESC_END means END data byte (0xDC, 220) */
#define ESC_ESC         0335    /* ESC ESC_ESC means ESC data byte (0xDD, 221) */

typedef struct rcp_slip rcp_slip;

// create / free
rcp_slip* rcp_slip_create(size_t size);
void rcp_slip_free(rcp_slip* s);

void rcp_slip_create_buffer(rcp_slip* s, size_t size);
void rcp_slip_free_buffer(rcp_slip* s);

void rcp_slip_set_buffer(rcp_slip* s, char* buffer, size_t size);
void rcp_slip_set_user(rcp_slip* s, void* user);
void rcp_slip_set_packet_cb(rcp_slip* s, void (*packetCb)(char*, size_t, void*));

// append data for decoding
void rcp_slip_append(rcp_slip* s, unsigned char c);
void rcp_slip_append_data(rcp_slip* s, char* data, size_t size);

// encode data to slip
void rcp_slip_encode(char* data, size_t size, void (*dataCb)(char, void*), void* user);

#ifdef __cplusplus
}
#endif

#endif
