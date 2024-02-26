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
