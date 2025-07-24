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

#include "rcp_client.h"

#include <string.h>

#include "rcp_memory.h"
#include "rcp_logging.h"
#include "rcp_packet.h"
#include "rcp_infodata.h"
#include "rcp_client_transporter.h"
#include "rcp_manager.h"
#include "rcp_parameter.h"
#include "rcp_semver.h"


#if defined(RCP_CLIENT_DEBUG_LOG) || defined(RCP_ALL_DEBUG)
#define RCP_CLIENT_DEBUG(...) RCP_DEBUG(__VA_ARGS__)
#else
#define RCP_CLIENT_DEBUG(...)
#endif

#if defined(RCP_CLIENT_MALLOC_DEBUG_LOG) || defined(RCP_ALL_DEBUG)
#define RCP_CLIENT_MALLOC_DEBUG(...) RCP_DEBUG(__VA_ARGS__)
#else
#define RCP_CLIENT_MALLOC_DEBUG(...)
#endif


struct rcp_client
{
    rcp_manager* manager;
    rcp_client_transporter* transporter;
    char* applicationId;
    bool acceptParameter;

    void (*parameterAddedCb)(rcp_parameter* parameter, void* user);
    void (*parameterRemovedCb)(rcp_parameter* parameter, void* user);
    void (*initializeDoneCb)(void* user);

    void* user;
};


static void manager_parameter_added_cb(rcp_parameter* parameter, void* client)
{
    if (client)
    {
        rcp_client* rcpc = (rcp_client*)client;
        if (rcpc->parameterAddedCb)
        {
            rcpc->parameterAddedCb(parameter, rcpc->user);
        }
    }
}

static void manager_parameter_removed_cb(rcp_parameter* parameter, void* client)
{
    if (client)
    {
        rcp_client* rcpc = (rcp_client*)client;
        if (rcpc->parameterRemovedCb)
        {
            rcpc->parameterRemovedCb(parameter, rcpc->user);
        }
    }
}


static void _create_manager(rcp_client* client)
{
    if (client->manager)
    {
        rcp_manager_free((client->manager));
        client->manager = NULL;
    }

    client->manager = rcp_manager_create(client);

    if (client->manager != NULL)
    {
        rcp_manager_set_parameter_added_cb(client->manager, manager_parameter_added_cb);
        rcp_manager_set_parameter_removed_cb(client->manager, manager_parameter_removed_cb);
        rcp_manager_set_data_cb_all(client->manager, rcp_client_manager_data_cb_all);
    }
    else
    {
        RCP_ERROR("could not alloc manager");
    }
}

rcp_client* rcp_client_create(rcp_client_transporter* transporter)
{
    rcp_client* client = RCP_CALLOC(1, sizeof(rcp_client));

    if (client != NULL)
    {
        RCP_CLIENT_MALLOC_DEBUG("*** rcp client : %p\n", client);

        _create_manager(client);

        if (transporter != NULL)
        {
            client->transporter = transporter;

            rcp_client_transporter_set_recv_cb(transporter, client, rcp_client_receive_cb);
            rcp_client_transporter_set_connected_cb(transporter, client, rcp_client_connected_cb);
            rcp_client_transporter_set_disconnected_cb(transporter, client, rcp_client_disconnected_cb);
        }
    }
    else
    {
        RCP_ERROR("could not alloc client");
    }

    return client;
}

void rcp_client_free(rcp_client* client)
{
    if (client)
    {
        rcp_manager_free(client->manager);

        if (client->applicationId)
        {
            RCP_CLIENT_MALLOC_DEBUG("+++ client application id: %p\n", client->applicationId);
            RCP_FREE(client->applicationId);
            client->applicationId = NULL;
        }

        RCP_CLIENT_MALLOC_DEBUG("+++ client: %p\n", client);
        RCP_FREE(client);
    }
}

rcp_manager* rcp_client_get_manager(rcp_client* client)
{
    if (client)
    {
        return client->manager;
    }
    return NULL;
}

void rcp_client_set_id(rcp_client* client, const char* id)
{
    if (client == NULL) return;

    if (client->applicationId != NULL)
    {
        RCP_CLIENT_MALLOC_DEBUG("+++ client application id: %p\n", client->applicationId);
        RCP_FREE(client->applicationId);
        client->applicationId = NULL;
    }

    if (id != NULL)
    {
        size_t str_len = strlen(id);
        if (str_len > 0)
        {
            client->applicationId = RCP_CALLOC(1, str_len + 1);
            if (client->applicationId)
            {
                RCP_CLIENT_MALLOC_DEBUG("*** client id: %p\n", client->applicationId);
                strncpy(client->applicationId, id, str_len);
            }
            else
            {
                RCP_CLIENT_DEBUG("client: could not calloc for applicationid\n", client->applicationId);
            }
        }
    }
}

void rcp_client_set_user(rcp_client* client, void* user)
{
    if (client)
    {
        client->user = user;
    }
}

void rcp_client_update(rcp_client* client)
{
    if (client && client->manager)
    {
        rcp_manager_update(client->manager);
    }
}

void rcp_client_log(rcp_client* client)
{
#ifdef RCP_LOG_INFO
    if (client && client->manager)
    {
        if (client->applicationId)
        {
            RCP_INFO("id: %s\n", client->applicationId);
        }

        rcp_manager_log(client->manager);
    }
#endif
}


// called from manager on parameter update
void rcp_client_manager_data_cb_all(void* c, char* data, size_t size)
{
    if (c == NULL) return;
    if (data == NULL) return;
    if (size == 0) return;

    // hope this is a rcp_client
    rcp_client* client = (rcp_client*)c;

    if (client->transporter)
    {
        client->transporter->send(client->transporter, data, size);
    }
}



static inline void _do_command_info(rcp_client* client, rcp_packet* packet)
{
    // NOTE: packet owns infodata
    // no need to free it later
    rcp_infodata* data = rcp_packet_get_infodata(packet);

    if (data)
    {
        const char* version = rcp_infodata_get_version(data);
        RCP_INFO("rcp server version: %s\n", version);
        RCP_INFO("rcp server id: %s\n", rcp_infodata_get_application_id(data));

        /* rcp versions
         *  0.0.0 - init
         *  0.0.1 - changed remove command (used to send whole parameter, now only parameter id)
         *  0.1.0 - mandatory support for updatevalue command
        */

        bool is_compatible = false;
        rcp_semver server_semver;

        if (rcp_semver_parse(version, &server_semver))
        {
            rcp_semver_log(&server_semver);

            if (server_semver.major == 0 &&
                    server_semver.minor == 0 &&
                    server_semver.patch < 1)
            {
                // invalid version - can not handle remove command
            }
            else if (server_semver.major <= RCP_VERSION_MAJOR ||
                     server_semver.minor <= RCP_VERSION_MINOR)
            {
                // own version is higher than server version - seems to be ok
                is_compatible = true;
            }
        }

        if (is_compatible)
        {
            // send init if server is compatible
            if (client->transporter)
            {
                // send initialize
                char data[2];
                data[0] = COMMAND_INITIALIZE;
                data[1] = RCP_TERMINATOR;

                client->transporter->send(client->transporter, data, 2);
            }

            // accept data
            client->acceptParameter = true;
        }

    }
    else if (client->transporter)
    {
        // no data, answer with own version

        rcp_packet* info_packet = rcp_packet_create(COMMAND_INFO);

        if (info_packet)
        {
            rcp_infodata* info_data = rcp_infodata_create(RCP_VERSION, client->applicationId);

            if (info_data)
            {
                // NOTE: ownership is transfered
                rcp_packet_put_infodata(info_packet, info_data);

                //--------------------------------
                char* data_out = NULL;
                size_t data_out_size = rcp_packet_write(info_packet, &data_out, false);

                // write data
                if (data_out_size > 0 &&
                        data_out != NULL)
                {
                    client->transporter->send(client->transporter, data_out, data_out_size);

                    RCP_CLIENT_MALLOC_DEBUG("+++ data out: %p\n", data_out);
                    RCP_FREE(data_out);
                    data_out = NULL;
                }
            }

            rcp_packet_free(info_packet);
        }
    }
    else
    {
        RCP_CLIENT_DEBUG("no data, no transporter! did not query version from server\n");
    }
}


// called from transporter
void rcp_client_receive_cb(rcp_client* client, char* data, size_t size)
{
    if (client == NULL) return;    

    // parse data
    rcp_packet* packet = NULL;

    while (data != NULL
           && size > 0)
    {
        data = rcp_packet_parse(data, size, &packet, &size);

        if (data && packet)
        {
            rcp_packet_command command = rcp_packet_get_command(packet);
            switch (command)
            {
            case COMMAND_INFO:
                _do_command_info(client, packet);
                break;

            case COMMAND_INITIALIZE:
                // init marks the end of init
                if (client->initializeDoneCb != NULL)
                {
                    client->initializeDoneCb(client->user);
                }
                break;

            case COMMAND_DISCOVER:
                // no discovery on client
                RCP_CLIENT_DEBUG("ignore command 'discover' on client!\n");
               break;

            case COMMAND_UPDATE:
            case COMMAND_UPDATEVALUE:
            {
                if (client->acceptParameter)
                {
                    // update parameter
                    // NOTE: take ownership of this parameter (it might gets added)
                    rcp_parameter* parameter = rcp_packet_take_parameter(packet);

                    if (parameter)
                    {
                        if (rcp_parameter_is_type(parameter, DATATYPE_BANG)
                                && (command == COMMAND_UPDATEVALUE
                                    || (command == COMMAND_UPDATE
                                        && !rcp_parameter_has_options(parameter)
                                        )
                                    ))
                        {
                            // in case this is a bang-parameter with no option changed
                            // or the packet arrived as UPDATEVALUE we want to call the bang callback
                            rcp_parameter* cached_parameter = rcp_manager_get_parameter(client->manager, rcp_parameter_get_id(parameter));

                            bool parameter_was_cached = cached_parameter != NULL;

                            if (cached_parameter == NULL)
                            {
                                // TODO: consider optimizing this: rcp_manager_update_parameter does a parameter-lookup again
                                // this does not happen often - so leave it for now

                                // add it to the cache
                                rcp_manager_update_parameter(client->manager, parameter, false);
                                // NOTE: cached_parameter = parameter; ?
                                cached_parameter = rcp_manager_get_parameter(client->manager, rcp_parameter_get_id(parameter));
                            }

                            if (cached_parameter)
                            {
                                if (rcp_parameter_is_type(cached_parameter, DATATYPE_BANG))
                                {
                                    rcp_bang_parameter_call_bang_cb(RCP_BANG_PARAMETER(cached_parameter));
                                }
                                else
                                {
                                    RCP_CLIENT_DEBUG("client: update: bang parameter type missmatch\n");
                                }
                            }
                            else
                            {
                                RCP_ERROR("client: update: error - no cached parameter\n");
                            }

                            // if (cached_parameter != parameter) ?
                            if (parameter_was_cached)
                            {
                                // this parameter was in cache - dispose of update parameter
                                rcp_parameter_free(parameter);
                            }
                        }
                        else
                        {
                            if (rcp_manager_update_parameter(client->manager, parameter, false))
                            {
                                // parameter was added
                                // as client: all options unchanged
                                rcp_parameter_all_options_unchanged(parameter);
                            }
                            else
                            {
                                // parameter was not added
                                // free it
                                rcp_parameter_free(parameter);
                            }
                        }
                    }
                }
#ifdef RCP_CLIENT_DEBUG_LOG
                else
                {
                    // client does not accept parameter yet
                    int16_t id = 0;

                    // NOTE: don't take ownership
                    rcp_parameter* parameter = rcp_packet_get_parameter(packet);
                    if (parameter)
                    {
                        id = rcp_parameter_get_id(parameter);
                    }

                    RCP_CLIENT_DEBUG("client does not accept parameters! - %d\n", id);
                }
#endif
                break;
            }

            case COMMAND_REMOVE:
            {
                if (client->acceptParameter)
                {
                    int16_t id = rcp_packet_get_iddata(packet);

                    if (id != 0)
                    {
                        // remove as client
                        rcp_manager_remove_parameter_id(client->manager, id, false);
                    }
                }
                break;
            }

            case COMMAND_INVALID:
            case COMMAND_MAX_:
                // nop
                break;
            }

            //
            rcp_packet_free(packet);
            packet = NULL;
        }
    }
}

void rcp_client_connected_cb(rcp_client* client)
{
    if (client
            && client->transporter
            && client->transporter->send)
    {
        // query version from server
        char data[] = {COMMAND_INFO, RCP_TERMINATOR};
        client->transporter->send(client->transporter, data, 2);
    }
}

void rcp_client_disconnected_cb(rcp_client* client)
{
    if (client)
    {
        client->acceptParameter = false;

        // free all paramters
        rcp_manager_clear(client->manager);
    }
}

void rcp_client_set_parameter_added_cb(rcp_client* client, void (*cb)(rcp_parameter* parameter, void* user))
{
    if (client)
    {
        client->parameterAddedCb = cb;
    }
}

void rcp_client_set_parameter_removed_cb(rcp_client* client, void (*cb)(rcp_parameter* parameter, void* user))
{
    if (client)
    {
        client->parameterRemovedCb = cb;
    }
}

void rcp_client_set_init_done_cb(rcp_client* client, void (*cb)(void* user))
{
    if (client)
    {
        client->initializeDoneCb = cb;
    }
}
