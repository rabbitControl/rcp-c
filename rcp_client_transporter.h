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

#ifndef RCP_CLIENT_TRANSPORTER_H
#define RCP_CLIENT_TRANSPORTER_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stddef.h>

#include "rcp_transporter.h"
#include "rcp_client_type.h"

typedef struct rcp_client_transporter rcp_client_transporter;

struct rcp_client_transporter
{
    void (*send)(rcp_client_transporter* transporter, char* data, size_t size);

    // used internally by client:
    // received callback
    void (*received)(rcp_client* client, char* data, size_t size);
    // connected and disconnected callbacks
    void (*connected)(rcp_client* client);
    void (*disconnected)(rcp_client* client);

    rcp_client* client;
    void* user;
};


rcp_client_transporter* rcp_client_transporter_setup(rcp_client_transporter* t,
                                                     void (*send)(rcp_client_transporter* transporter, char* data, size_t size));

// internally
// callbacks
void rcp_client_transporter_set_recv_cb(rcp_client_transporter* t,
                                        rcp_client* client,
                                        void (*received)(rcp_client* client, char* data, size_t size));

void rcp_client_transporter_set_connected_cb(rcp_client_transporter* t,
                                             rcp_client* client,
                                             void (*connected)(rcp_client* client));

void rcp_client_transporter_set_disconnected_cb(rcp_client_transporter* t,
                                                rcp_client* client,
                                                void (*disconnected)(rcp_client* client));

// call callbacks
void rcp_client_transporter_call_recv_cb(rcp_client_transporter* t, char* data, size_t size);
void rcp_client_transporter_call_connected_cb(rcp_client_transporter* t);
void rcp_client_transporter_call_disconnected_cb(rcp_client_transporter* t);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
