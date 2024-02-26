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

#ifndef RCP_MANAGER_H
#define RCP_MANAGER_H

#ifdef __cplusplus
extern "C"{
#endif

#include "rcp.h"

#include "rcp_manager_type.h"
#include "rcp_parameter_type.h"


// create / free
rcp_manager* rcp_manager_create(void* user);
void rcp_manager_free(rcp_manager* manager);

// available id
int16_t rcp_manager_get_available_id(rcp_manager* manager);

// clear
void rcp_manager_clear(rcp_manager* manager); // clear all parameters

// parameter
bool rcp_manager_add_parameter(rcp_manager* manager, rcp_parameter* parameter, bool is_server); // full transfer
bool rcp_manager_update_parameter(rcp_manager* manager, rcp_parameter* parameter, bool is_server);
rcp_parameter* rcp_manager_get_parameter(rcp_manager* manager, int16_t id);
rcp_parameter_list* rcp_manager_get_paramter_list(rcp_manager* manager);
void rcp_manager_set_dirty(rcp_manager* manager, rcp_parameter* parameter);
bool rcp_manager_remove_parameter_id(rcp_manager* manager, int16_t parameter_id, bool is_server);

// find
rcp_group_parameter* rcp_manager_find_group(rcp_manager* manager, const char* name, rcp_group_parameter* group);
rcp_parameter* rcp_manager_find_parameter(rcp_manager* manager, const char* name, rcp_group_parameter* group);

// callbacks
void rcp_manager_set_parameter_added_cb(rcp_manager* manager, void (*cb)(rcp_parameter* parameter, void* user));
void rcp_manager_set_parameter_removed_cb(rcp_manager* manager, void (*cb)(rcp_parameter* parameter, void* user));

// set data callbacks
void rcp_manager_set_data_cb_one(rcp_manager* manager, void (*cb)(void*, char*, size_t, void*));
void rcp_manager_set_data_cb_all(rcp_manager* manager, void (*cb)(void*, char*, size_t));

// update
void rcp_manager_update(rcp_manager* manager);


// logging
void rcp_manager_log(rcp_manager* manager);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
