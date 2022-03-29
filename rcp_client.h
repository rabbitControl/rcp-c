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

#ifndef RCP_CLIENT_H
#define RCP_CLIENT_H

#ifdef __cplusplus
extern "C"{
#endif

#include "rcp.h"
#include "rcp_client_type.h"
#include "rcp_parameter_type.h"
#include "rcp_client_transporter.h"
#include "rcp_manager_type.h"


// create / free
rcp_client* rcp_client_create(rcp_client_transporter* transporter);
void rcp_client_free(rcp_client* client);

// manager
rcp_manager* rcp_client_get_manager(rcp_client* client);

// application id
void rcp_client_set_id(rcp_client* client, const char* id); // copy id

// user - used for callbacks
void rcp_client_set_user(rcp_client* client, void* user);

void rcp_client_update(rcp_client* client);
void rcp_client_log(rcp_client* client);


// called from manager on parameter update
void rcp_client_manager_data_cb_all(void* clnt, char* data, size_t size);

// called from transporter
void rcp_client_receive_cb(rcp_client* client, char* data, size_t size);
void rcp_client_connected_cb(rcp_client* client);
void rcp_client_disconnected_cb(rcp_client* client);

// callbacks
void rcp_client_set_parameter_added_cb(rcp_client* client, void (*cb)(rcp_parameter* parameter, void* user));
void rcp_client_set_parameter_removed_cb(rcp_client* client, void (*cb)(rcp_parameter* parameter, void* user));

#ifdef __cplusplus
} // extern "C"
#endif

#endif
