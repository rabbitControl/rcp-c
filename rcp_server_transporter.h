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

#ifndef RCP_SERVER_TRANSPORTER_H
#define RCP_SERVER_TRANSPORTER_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stddef.h>

#include "rcp_transporter.h"
#include "rcp_server_type.h"

typedef struct rcp_server_transporter rcp_server_transporter;

struct rcp_server_transporter
{
    void (*sendToOne)(rcp_server_transporter* transporter, char* data, size_t size, void* id);
    void (*sendToAll)(rcp_server_transporter* transporter, char* data, size_t size, void* excludeId);

    // received callback
    void (*received)(rcp_server* server, char* data, size_t size, void* client);

    // server reference
    rcp_server* server;
    void* user;
};

#define RCP_SERVER_TRANSPORTER(x) ((rcp_server_transporter*)x)


rcp_server_transporter* rcp_server_transporter_setup(rcp_server_transporter* t,
                                                     void (*sendToOne)(rcp_server_transporter* transporter, char* data, size_t size, void* id),
                                                     void (*sendTAll)(rcp_server_transporter* transporter, char* data, size_t size, void* excludeId));

// callback (set by rcp_server)
void rcp_server_transporter_set_recv_cb(rcp_server_transporter* t,
                                        rcp_server* server,
                                        void (*received)(rcp_server* server, char* data, size_t size, void* client));

// call callback (call when new data arrived)
void rcp_server_transporter_call_recv_cb(rcp_server_transporter* t, char* data, size_t size, void* client);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
