/*
********************************************************************
* rabbitcontrol - a protocol and data-format for remote control.
*
* https://rabbitcontrol.cc
* https://github.com/rabbitcontrol/rcp-c
*
* This file is part of rabbitcontrol for c.
*
* Written by Ingo Randolf, 2021
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*********************************************************************
*/

#ifndef RCP_SERVER_H
#define RCP_SERVER_H

#ifdef __cplusplus
extern "C"{
#endif

#include "rcp.h"
#include "rcp_server_type.h"
#include "rcp_manager_type.h"
#include "rcp_parameter_type.h"
#include "rcp_server_transporter.h"
#include "rcp_manager_type.h"


// create / free
rcp_server* rcp_server_create(rcp_server_transporter* transporter);
void rcp_server_free(rcp_server* server);

// manager
rcp_manager* rcp_server_get_manager(rcp_server* server);

// application id
void rcp_server_set_id(rcp_server* server, const char* id); // copy id

// transporter
void rcp_server_add_transporter(rcp_server* server, rcp_server_transporter* transporter);
void rcp_server_remove_transporter(rcp_server* server, rcp_server_transporter* transporter);

void rcp_server_update(rcp_server* server);

// expose parameter
rcp_value_parameter* rcp_server_expose_bool(rcp_server* server, const char* label, rcp_group_parameter* group);
rcp_value_parameter* rcp_server_expose_i8(rcp_server* server, const char* label, rcp_group_parameter* group);
rcp_value_parameter* rcp_server_expose_i32(rcp_server* server, const char* label, rcp_group_parameter* group);
rcp_value_parameter* rcp_server_expose_f32(rcp_server* server, const char* label, rcp_group_parameter* group);
rcp_value_parameter* rcp_server_expose_string(rcp_server* server, const char* label, rcp_group_parameter* group);
rcp_bang_parameter*  rcp_server_expose_bang(rcp_server* server, const char* label, rcp_group_parameter* group);
rcp_group_parameter* rcp_server_create_group(rcp_server* server, const char* label, rcp_group_parameter* group);

// remove
bool rcp_server_remove_parameter(rcp_server* server, rcp_parameter* parameter);
bool rcp_server_remove_parameter_id(rcp_server* server, int16_t parameter_id);

// find
rcp_group_parameter* rcp_server_find_group(rcp_server* server, const char* name, rcp_group_parameter* group);


// called from manager on init, ...
void rcp_server_manager_data_cb_one(void* server, char* data, size_t size, void* client);
// called from manager on parameter update
void rcp_server_manager_data_cb_all(void* server, char* data, size_t size);

// called from transporter
void rcp_server_receive_cb(rcp_server* server, char* data, size_t size, void* client);


// logging
void rcp_server_log(rcp_server* server);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
