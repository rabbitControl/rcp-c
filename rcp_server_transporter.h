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
