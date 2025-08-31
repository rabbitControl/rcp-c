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

#include "rcp_client_log_transporter.h"

#include "rcp_memory.h"
#include "rcp_logging.h"

#if defined(RCP_CLIENT_LOGTRANSPORTER_DEBUG_LOG) || defined(RCP_ALL_DEBUG)
#define RCP_CLIENT_LOGTRANSPORTER_DEBUG(...) RCP_DEBUG(__VA_ARGS__)
#else
#define RCP_CLIENT_LOGTRANSPORTER_DEBUG(...)
#endif

#if defined(RCP_CLIENT_LOGTRANSPORTER_MALLOC_DEBUG_LOG) || defined(RCP_ALL_DEBUG)
#define RCP_CLIENT_LOGTRANSPORTER_MALLOC_DEBUG(...) RCP_DEBUG(__VA_ARGS__)
#else
#define RCP_CLIENT_LOGTRANSPORTER_MALLOC_DEBUG(...)
#endif

rcp_client_log_transporter* rcp_client_log_transporter_create()
{
    rcp_client_log_transporter* t = (rcp_client_log_transporter*)RCP_CALLOC(1, sizeof(rcp_client_log_transporter));

    if (t)
    {
        RCP_CLIENT_LOGTRANSPORTER_MALLOC_DEBUG("*** client log transporter: %p\n", t);

        rcp_client_transporter_setup(RCP_TRANSPORTER(t),
                                     rcp_client_log_transporter_send);
    }

    return t;
}

void rcp_client_log_transporter_free(rcp_client_log_transporter* transporter)
{
    if (transporter)
    {
        RCP_CLIENT_LOGTRANSPORTER_MALLOC_DEBUG("+++ client log transporter: %p\n", transporter);
        RCP_FREE(transporter);
    }
}

void rcp_client_log_transporter_send(rcp_client_transporter* transporter, const char* data, size_t size)
{
    RCP_INFO("CLIENT LOG TRANSPORTER - send data:\n");

    for (size_t i=0; i<size; i++)
    {
        RCP_INFO_ONLY("0x%02x ", data[i]);
    }

    RCP_INFO_ONLY("\n");
}
