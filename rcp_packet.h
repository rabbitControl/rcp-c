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

#ifndef RCP_PACKET_H
#define RCP_PACKET_H

#ifdef __cplusplus
extern "C"{
#endif

#include "rcp.h"
#include "rcp_packet_type.h"
#include "rcp_option_type.h"
#include "rcp_parameter_type.h"
#include "rcp_infodata.h"

//#define RCP_PACKET_DEBUG_LOG
//#define RCP_PACKET_MALLOC_DEBUG_LOG

// create / free
rcp_packet* rcp_packet_create(rcp_packet_command command);
void rcp_packet_free(rcp_packet* packet);

// command
void rcp_packet_set_command(rcp_packet* packet, rcp_packet_command command);
rcp_packet_command rcp_packet_get_command(rcp_packet* packet);

// timestamp
void rcp_packet_set_timestamp(rcp_packet* packet, uint64_t timestamp);
uint64_t rcp_packet_get_timestamp(rcp_packet* packet);

// iddata
void rcp_packet_set_iddata(rcp_packet* packet, int16_t id);
int16_t rcp_packet_get_iddata(rcp_packet* packet);

// infodata
void rcp_packet_set_infodata(rcp_packet* packet, rcp_infodata* data); // no transfer
rcp_infodata* rcp_packet_get_infodata(rcp_packet* packet); // no transfer
void rcp_packet_put_infodata(rcp_packet* packet, rcp_infodata* data); // full transfer
rcp_infodata* rcp_packet_take_infodata(rcp_packet* packet); // full transfer

// parameter
void rcp_packet_set_parameter(rcp_packet* packet, rcp_parameter* parameter); // no transfer
rcp_parameter* rcp_packet_get_parameter(rcp_packet* packet); // no transfer
void rcp_packet_put_parameter(rcp_packet* packet, rcp_parameter* parameter); // full transfer
rcp_parameter* rcp_packet_take_parameter(rcp_packet* packet); // full transfer

// parse and write
char* rcp_packet_parse(char* data, size_t size, rcp_packet** out_packet, size_t* out_size);
size_t rcp_packet_write(rcp_packet* packet, char** dst, bool all);
size_t rcp_packet_write_buf(rcp_packet* packet, char* data, size_t size, bool all);

void rcp_packet_log(rcp_packet* packet);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
