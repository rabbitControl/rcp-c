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

#include "rcp_client_transporter.h"

#include <string.h>

#include "rcp_logging.h"
#include "rcp_client.h"

rcp_client_transporter* rcp_client_transporter_setup(rcp_client_transporter* t,
                                                     void (*send)(rcp_client_transporter* transporter, char* data, size_t size))
{
    if (t)
    {
        memset(t, 0, sizeof(rcp_client_transporter));

        t->send = send;
    }

    return t;
}


void rcp_client_transporter_set_recv_cb(rcp_client_transporter* t,
                                        rcp_client* client,
                                        void (*received)(rcp_client* client, char* data, size_t size))
{
    if (t)
    {
        t->received = received;

        if (t->client == NULL)
        {
            t->client = client;
        }
        else if (t->client != client)
        {
            RCP_ERROR("inconsistent client! - received callback may fail\n");
        }
    }
}

void rcp_client_transporter_set_connected_cb(rcp_client_transporter* t,
                                             rcp_client* client,
                                             void (*connected)(rcp_client* client))
{
    if (t)
    {
        t->connected = connected;

        if (t->client == NULL)
        {
            t->client = client;
        }
        else if (t->client != client)
        {
            RCP_ERROR("inconsistent client! - connect callback may fail\n");
        }
    }
}

void rcp_client_transporter_set_disconnected_cb(rcp_client_transporter* t,
                                                rcp_client* client,
                                                void (*disconnected)(rcp_client* client))
{
    if (t)
    {
        t->disconnected = disconnected;

        if (t->client == NULL)
        {
            t->client = client;
        }
        else if (t->client != client)
        {
            RCP_ERROR("inconsistent client! - disconnect callback may fail\n");
        }
    }
}

// call callbacks
void rcp_client_transporter_call_recv_cb(rcp_client_transporter* t, char* data, size_t size)
{
    if (t && t->received)
    {
        t->received(t->client, data, size);
    }
}

void rcp_client_transporter_call_connected_cb(rcp_client_transporter* t)
{
    if (t && t->connected)
    {
        t->connected(t->client);
    }
}

void rcp_client_transporter_call_disconnected_cb(rcp_client_transporter* t)
{
    if (t && t->disconnected)
    {
        t->disconnected(t->client);
    }
}
