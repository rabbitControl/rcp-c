/*
********************************************************************
* RabbitControl - a protocol for remote control.
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

//#define RCP_CLIENT_DEBUG_LOG
//#define RCP_CLIENT_MALLOC_DEBUG_LOG


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
void rcp_client_set_init_done_cb(rcp_client* client, void (*cb)(void* user));

#ifdef __cplusplus
} // extern "C"
#endif

#endif
