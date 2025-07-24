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
