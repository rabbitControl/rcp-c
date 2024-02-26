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

#include "rcp_server_transporter.h"

#include <string.h>

#include "rcp_logging.h"


rcp_server_transporter* rcp_server_transporter_setup(rcp_server_transporter* t,
                                                     void (*sendToOne)(rcp_server_transporter* transporter, char* data, size_t size, void* id),
                                                     void (*sendTAll)(rcp_server_transporter* transporter, char* data, size_t size, void* excludeId))
{
    if (t)
    {
        memset(t, 0, sizeof(rcp_server_transporter));

        t->sendToOne = sendToOne;
        t->sendToAll = sendTAll;
    }

    return t;
}


void rcp_server_transporter_set_recv_cb(rcp_server_transporter* t,
                                        rcp_server* server,
                                        void (*received)(rcp_server* server, char* data, size_t size, void* user))
{
    if (t)
    {
        t->received = received;
        t->server = server;
    }
}

void rcp_server_transporter_call_recv_cb(rcp_server_transporter* t, char* data, size_t size, void* client)
{
    if (t && t->received)
    {
        t->received(t->server, data, size, client);
    }
}
