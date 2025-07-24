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

#include "rcp_server_log_transporter.h"

#include "rcp_memory.h"
#include "rcp_logging.h"

#if defined(RCP_SERVER_LOGTRANSPORTER_DEBUG_LOG) || defined(RCP_ALL_DEBUG)
#define RCP_SERVER_LOGTRANSPORTER_DEBUG(...) RCP_DEBUG(__VA_ARGS__)
#else
#define RCP_SERVER_LOGTRANSPORTER_DEBUG(...)
#endif

#if defined(RCP_SERVER_LOGTRANSPORTER_MALLOC_DEBUG_LOG) || defined(RCP_ALL_DEBUG)
#define RCP_SERVER_LOGTRANSPORTER_MALLOC_DEBUG(...) RCP_DEBUG(__VA_ARGS__)
#else
#define RCP_SERVER_LOGTRANSPORTER_MALLOC_DEBUG(...)
#endif

rcp_server_log_transporter* rcp_server_log_transporter_create()
{
    rcp_server_log_transporter* t = (rcp_server_log_transporter*)RCP_CALLOC(1, sizeof(rcp_server_log_transporter));

    if (t)
    {
        RCP_SERVER_LOGTRANSPORTER_MALLOC_DEBUG("*** log transporter: %p\n", t);

        rcp_server_transporter_setup(RCP_TRANSPORTER(t),
                                     rcp_server_log_transporter_send_to_one,
                                     rcp_server_log_transporter_send_to_all);
    }

    return t;
}

void rcp_server_log_transporter_free(rcp_server_log_transporter* transporter)
{
    if (transporter)
    {
        RCP_SERVER_LOGTRANSPORTER_MALLOC_DEBUG("+++ log transporter: %p\n", transporter);
        RCP_FREE(transporter);
    }
}


void rcp_server_log_transporter_push_data(rcp_server_log_transporter* transporter, char* data, size_t size)
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

void rcp_server_log_transporter_bind(rcp_server_transporter* transporter)
{
    rcp_server_log_transporter* t = (rcp_server_log_transporter*)transporter;
    RCP_INFO("open some connection!");
}

void rcp_server_log_transporter_unbind(rcp_server_transporter* transporter)
{
    rcp_server_log_transporter* t = (rcp_server_log_transporter*)transporter;
    RCP_INFO("close some connection!");
}

void rcp_server_log_transporter_send_to_one(rcp_server_transporter* transporter, char* data, size_t size, void* id)
{
//    server_log_transporter* t = (server_log_transporter*)transporter;
    RCP_INFO("LOG TRANSPORTER - send data to one: %p\n", id);

    for (size_t i=0; i<size; i++)
    {
        RCP_INFO_ONLY("0x%02x ", data[i]);
    }

    RCP_INFO_ONLY("\n");
}

void rcp_server_log_transporter_send_to_all(rcp_server_transporter* transporter, char* data, size_t size, void* excludeId)
{
    rcp_server_log_transporter* t = (rcp_server_log_transporter*)transporter;
    RCP_INFO("LOG TRANSPORTER - send data to all, except: %p\n", excludeId);
    for (size_t i=0; i<size; i++)
    {
        RCP_INFO_ONLY("0x%02x ", data[i]);
    }
    RCP_INFO_ONLY("\n");
}

int rcp_server_log_transporter_connection_count(rcp_server_transporter* transporter)
{
    return 1;
}
