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

#ifndef RCP_CLIENT_LOG_TRANSPORTER_H
#define RCP_CLIENT_LOG_TRANSPORTER_H

#ifdef __cplusplus
extern "C"{
#endif

#include "rcp_client_transporter.h"

//#define RCP_CLIENT_LOGTRANSPORTER_DEBUG_LOG
//#define RCP_CLIENT_LOGTRANSPORTER_MALLOC_DEBUG_LOG

typedef struct rcp_client_log_transporter
{
    rcp_client_transporter transporter;
} rcp_client_log_transporter;


rcp_client_log_transporter* rcp_client_log_transporter_create();
void rcp_client_log_transporter_free(rcp_client_log_transporter* transporter);

// push data in
void rcp_client_log_transporter_push_data(rcp_client_log_transporter* transporter, char* data, size_t size);

// rcp_client_transporter interface
void rcp_client_log_transporter_send(rcp_client_transporter* transporter, char* data, size_t size);


#ifdef __cplusplus
} // extern "C"
#endif

#endif // RCP_CLIENT_LOG_TRANSPORTER_H
