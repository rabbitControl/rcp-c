/*
********************************************************************
* rabbitcontrol - a protocol and data-format for remote control.
*
* https://rabbitcontrol.cc
* https://github.com/rabbitcontrol/rcp-c
*
* This file is part of rabbitcontrol for c.
*
* Written by Ingo Randolf, 2021 - 2022
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*********************************************************************
*/

#include "rcp_log_transporter.h"

#include "rcp_memory.h"
#include "rcp_logging.h"

rcp_server_log_transporter* rcp_log_transporter_create()
{
    rcp_server_log_transporter* t = (rcp_server_log_transporter*)RCP_CALLOC(1, sizeof(rcp_server_log_transporter));

    if (t)
    {
        RCP_DEBUG("*** log transporter: %p\n", t);

        rcp_server_transporter_setup(RCP_TRANSPORTER(t),
                                 rcp_log_transporter_send_to_one,
                                 rcp_log_transporter_send_to_all);
    }

    return t;
}

void rcp_log_transporter_free(rcp_server_log_transporter* transporter)
{
    if (transporter)
    {
        RCP_DEBUG("+++ log transporter: %p\n", transporter);
        RCP_FREE(transporter);
    }
}


void rcp_log_transporter_push_data(rcp_server_log_transporter* transporter, char* data, size_t size)
{
    if (transporter &&
            data  &&
            size > 0)
    {
        if (transporter->transporter.received)
        {
            transporter->transporter.received(transporter->transporter.server,
                                              data,
                                              size,
                                              NULL);
        }
    }
}

void rcp_log_transporter_bind(rcp_server_transporter* transporter)
{
    rcp_server_log_transporter* t = (rcp_server_log_transporter*)transporter;
    RCP_INFO("open some connection!");
}

void rcp_log_transporter_unbind(rcp_server_transporter* transporter)
{
    rcp_server_log_transporter* t = (rcp_server_log_transporter*)transporter;
    RCP_INFO("close some connection!");
}

void rcp_log_transporter_send_to_one(rcp_server_transporter* transporter, char* data, size_t size, void* id)
{
//    server_log_transporter* t = (server_log_transporter*)transporter;
    RCP_INFO("LOG TRANSPORTER - send data to one: %p\n", id);

    for (size_t i=0; i<size; i++)
    {
        RCP_INFO_ONLY("0x%02x ", data[i]);
    }

    RCP_INFO_ONLY("\n");
}

void rcp_log_transporter_send_to_all(rcp_server_transporter* transporter, char* data, size_t size, void* excludeId)
{
    rcp_server_log_transporter* t = (rcp_server_log_transporter*)transporter;
    RCP_INFO("LOG TRANSPORTER - send data to all, except: %p\n", excludeId);
    for (size_t i=0; i<size; i++)
    {
        RCP_INFO_ONLY("0x%02x ", data[i]);
    }
    RCP_INFO_ONLY("\n");
}

int rcp_log_transporter_connection_count(rcp_server_transporter* transporter)
{
    return 1;
}
