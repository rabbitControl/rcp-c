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
