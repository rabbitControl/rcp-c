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

#ifndef RCP_INFODATA_H
#define RCP_INFODATA_H

#ifdef __cplusplus
extern "C"{
#endif

#include "rcp.h"
#include "rcp_option_type.h"

#define RCP_INFODATA(x) ((rcp_infodata*)x)

typedef struct rcp_infodata rcp_infodata;

// create / free
rcp_infodata* rcp_infodata_create(const char* version, const char* applicationId);
void rcp_infodata_free(rcp_infodata* data);

// parse
rcp_infodata* rcp_infodata_parse(char** data, size_t* size);

// size and writing
size_t rcp_infodata_get_size(rcp_infodata* data);
size_t rcp_infodata_write(rcp_infodata* data, char* dst, size_t size);

// getter
const char* rcp_infodata_get_version(rcp_infodata* data);
const char* rcp_infodata_get_application_id(rcp_infodata* data);


// logging
void rcp_infodata_log(rcp_infodata* data);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
