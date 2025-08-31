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

#include "rcp_client_transporter.h"

#include <string.h>

#include "rcp_logging.h"

rcp_client_transporter* rcp_client_transporter_setup(rcp_client_transporter* t,
                                                     void (*send)(rcp_client_transporter* transporter, const char* data, size_t size))
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
                                        void (*received)(rcp_client* client, const char* data, size_t size))
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
void rcp_client_transporter_call_recv_cb(rcp_client_transporter* t, const char* data, size_t size)
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
