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

#ifndef RCP_DUMMY_SERVER_TRANSPORTER_H
#define RCP_DUMMY_SERVER_TRANSPORTER_H

#ifdef __cplusplus
extern "C"{
#endif


#include "rcp_server_transporter.h"

typedef struct rcp_server_log_transporter
{
    rcp_server_transporter transporter;
} rcp_server_log_transporter;


// create / free
rcp_server_log_transporter* rcp_log_transporter_create();
void rcp_log_transporter_free(rcp_server_log_transporter* transporter);

// push data in
void rcp_log_transporter_push_data(rcp_server_log_transporter* transporter, char* data, size_t size);

// server transporter interface
void rcp_log_transporter_bind(rcp_server_transporter* transporter);
void rcp_log_transporter_unbind(rcp_server_transporter* transporter);

void rcp_log_transporter_send_to_one(rcp_server_transporter* transporter, char* data, size_t size, void* id);
void rcp_log_transporter_send_to_all(rcp_server_transporter* transporter, char* data, size_t size, void* excludeId);
int rcp_log_transporter_connection_count(rcp_server_transporter* transporter);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
