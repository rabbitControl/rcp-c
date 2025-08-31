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

#include "rcp_server.h"

#include <string.h>

#include "rcp_memory.h"
#include "rcp_logging.h"
#include "rcp_packet.h"
#include "rcp_infodata.h"
#include "rcp_manager.h"
#include "rcp_parameter.h"

#define RCP_SERVER_SETUP_PARAMETER(p, m) \
    rcp_parameter_set_label(RCP_PARAMETER(p), label);\
    rcp_parameter_set_parent(RCP_PARAMETER(p), group);\
    rcp_manager_add_parameter(m, RCP_PARAMETER(p), true);


#if defined(RCP_SERVER_DEBUG_LOG) || defined(RCP_ALL_DEBUG)
#define RCP_SERVER_DEBUG(...) RCP_DEBUG(__VA_ARGS__)
#else
#define RCP_SERVER_DEBUG(...)
#endif

#if defined(RCP_SERVER_MALLOC_DEBUG_LOG) || defined(RCP_ALL_DEBUG)
#define RCP_SERVER_MALLOC_DEBUG(...) RCP_DEBUG(__VA_ARGS__)
#else
#define RCP_SERVER_MALLOC_DEBUG(...)
#endif

typedef struct transporter_list_item transporter_list_item;

struct rcp_server
{
    rcp_manager* manager;
    transporter_list_item* transporters;
    char* applicationId;
};

struct transporter_list_item
{
    transporter_list_item* next;
    rcp_server_transporter* transporter;
};



static inline void _rcp_server_send_to_one(rcp_server* server, const char* data, size_t size, void* client)
{
    transporter_list_item* le = server->transporters;
    while (le)
    {
        le->transporter->sendToOne(le->transporter,
                                   data,
                                   size,
                                   client);

        le = le->next;
    }
}

static inline void _rcp_server_send_to_all(rcp_server* server, const char* data, size_t size, void* client)
{
    transporter_list_item* le = server->transporters;
    while (le)
    {
        le->transporter->sendToAll(le->transporter,
                                   data,
                                   size,
                                   client);

        le = le->next;
    }
}


rcp_server* rcp_server_create(rcp_server_transporter* transporter)
{
    rcp_server* server = RCP_CALLOC(1, sizeof(rcp_server));

    if (server != NULL)
    {
        RCP_SERVER_MALLOC_DEBUG("*** server: %p\n", server);

        server->manager = rcp_manager_create(server);

        if (server->manager != NULL)
        {
            rcp_manager_set_data_cb_one(server->manager, rcp_server_manager_data_cb_one);
            rcp_manager_set_data_cb_all(server->manager, rcp_server_manager_data_cb_all);
        }
        else
        {
            RCP_ERROR("could not alloc manager");
        }

        rcp_server_add_transporter(server, transporter);
    }
    else
    {
        RCP_ERROR("could not alloc server");
    }

    return server;
}

// NOTE: transporters are not freed
void rcp_server_free(rcp_server* server)
{
    if (server)
    {
        // remove transporter list items
        transporter_list_item* le = server->transporters;
        transporter_list_item* next;
        while (le)
        {
            next = le->next;

            RCP_SERVER_MALLOC_DEBUG("+++ transporter list item: %p\n", le);
            RCP_FREE(le);

            le = next;
        }
        server->transporters = NULL;

        rcp_manager_free(server->manager);

        if (server->applicationId)
        {
            RCP_SERVER_MALLOC_DEBUG("+++ server id: %p\n", server->applicationId);
            RCP_FREE(server->applicationId);
            server->applicationId = NULL;
        }

        RCP_SERVER_MALLOC_DEBUG("+++ server: %p\n", server);
        RCP_FREE(server);
    }
}

rcp_manager* rcp_server_get_manager(rcp_server* server)
{
    if (server)
    {
        return server->manager;
    }
    return NULL;
}

void rcp_server_set_id(rcp_server* server, const char* id)
{
    if (server == NULL) return;

    if (server->applicationId != NULL)
    {
        RCP_SERVER_MALLOC_DEBUG("+++ server id: %p\n", server->applicationId);
        RCP_FREE(server->applicationId);
        server->applicationId = NULL;
    }

    if (id != NULL)
    {
        size_t str_len = strlen(id);
        if (str_len > 0)
        {
            server->applicationId = RCP_CALLOC(1, str_len + 1);
            if (server->applicationId)
            {
                RCP_SERVER_MALLOC_DEBUG("*** server id: %p\n", server->applicationId);
                strncpy(server->applicationId, id, str_len);
            }
        }
    }
}

void rcp_server_add_transporter(rcp_server* server, rcp_server_transporter* transporter)
{
    if (server != NULL && transporter != NULL)
    {
        // check if transporter already exists
        transporter_list_item* item = server->transporters;
        while (item != NULL)
        {
            if (item->transporter == transporter)
            {
                return;
            }
            item = item->next;
        }

        //--------------------------------
        // transporter does not yet exist

        rcp_server_transporter_set_recv_cb(transporter, server, rcp_server_receive_cb);

        // add transporter to transporterlist
        transporter_list_item* transporter_item = RCP_CALLOC(1, sizeof (transporter_list_item));
        if (transporter_item)
        {
            RCP_SERVER_MALLOC_DEBUG("*** transporter list item: %p\n", transporter_item);

            transporter_item->next = server->transporters;
            transporter_item->transporter = transporter;

            server->transporters = transporter_item;
        }
    }
}

void rcp_server_remove_transporter(rcp_server* server, rcp_server_transporter* transporter)
{
    if (server != NULL && transporter != NULL)
    {
        RCP_SERVER_DEBUG("remove transporter\n");

        rcp_server_transporter_set_recv_cb(transporter, NULL, NULL);

        // remove transporter from serverlist
        transporter_list_item* item = server->transporters;
        transporter_list_item* last_item = NULL;
        while (item != NULL)
        {
            if (item->transporter == transporter)
            {
                if (last_item == NULL)
                {
                    server->transporters = item->next;
                }
                else
                {
                    last_item->next = item->next;
                }

                // destroy list item
                RCP_SERVER_MALLOC_DEBUG("+++ transporter list item: %p\n", item);
                RCP_FREE(item);

                return;
            }

            last_item = item;
            item = item->next;
        }
    }
}


void rcp_server_update(rcp_server* server)
{
    if (server && server->manager)
    {
        rcp_manager_update(server->manager);
    }
}

void rcp_server_log(rcp_server* server)
{
#ifdef RCP_LOG_INFO
    if (server && server->manager)
    {
        if (server->applicationId)
        {
            RCP_INFO("id: %s\n", server->applicationId);
        }

        rcp_manager_log(server->manager);
    }
#endif
}

// called from manager on init, ...
// we need to send this data
void rcp_server_manager_data_cb_one(void* srv, const char* data, size_t size, void* client)
{
    if (srv == NULL) return;
    if (data == NULL) return;
    if (size == 0) return;

    // hope this is a server!
    _rcp_server_send_to_one((rcp_server*)srv, data, size, client);
}

// called from manager on parameter update
// we need to send this data
void rcp_server_manager_data_cb_all(void* srv, const char* data, size_t size)
{
    if (srv == NULL) return;
    if (data == NULL) return;
    if (size == 0) return;

    // hope this is a server!
    _rcp_server_send_to_all((rcp_server*)srv, data, size, NULL);
}


static inline void _do_command_info(rcp_server* server, rcp_packet* packet, void* client)
{
    // NOTE: don't take ownership
    rcp_infodata* data = rcp_packet_get_infodata(packet);

    if (data)
    {
        RCP_INFO("rcp client version: %s\n", rcp_infodata_get_version(data));
        RCP_INFO("rcp client id: %s\n", rcp_infodata_get_application_id(data));

        // TODO: check client version?
    }
    else
    {
        // no data, answer with own version

        rcp_packet* info_packet = rcp_packet_create(COMMAND_INFO);

        if (info_packet)
        {
            rcp_infodata* info_data = rcp_infodata_create(RCP_VERSION, server->applicationId);

            if (info_data)
            {
                // NOTE: ownership is transfered
                rcp_packet_put_infodata(info_packet, info_data);

                //--------------------------------
                char* data_out = NULL;
                size_t data_out_size = rcp_packet_write(info_packet, &data_out, false);

                if (data_out_size > 0 &&
                        data_out != NULL)
                {
                    // send it out...
                    _rcp_server_send_to_one(server, data_out, data_out_size, client);

                    RCP_SERVER_MALLOC_DEBUG("+++ data out: %p\n", data_out);
                    RCP_FREE(data_out);
                }
            }

            rcp_packet_free(info_packet);
        }


        //--------------------------------
        // request infodata from client
        info_packet = rcp_packet_create(COMMAND_INFO);

        if (info_packet)
        {
            char* data_out = NULL;
            size_t data_out_size = rcp_packet_write(info_packet, &data_out, false);

            if (data_out_size > 0 &&
                    data_out != NULL)
            {
                // send it out...
                _rcp_server_send_to_one(server, data_out, data_out_size, client);

                RCP_SERVER_MALLOC_DEBUG("+++ data out: %p\n", data_out);
                RCP_FREE(data_out);
            }

            rcp_packet_free(info_packet);
        }
    }
}

// send initial state of all parameters
static void send_initial_parameters(rcp_server* server, void* client)
{
    char* data_out = NULL;
    size_t data_out_size = 0;

    rcp_packet* packet = rcp_packet_create(COMMAND_UPDATE);
    if (packet == NULL)
    {
        RCP_SERVER_DEBUG("could not create packet\n");
        return;
    }

    rcp_parameter_list* pe = rcp_manager_get_paramter_list(server->manager);
    while (pe)
    {
        rcp_packet_set_parameter(packet, pe->parameter);

        data_out_size = rcp_packet_write(packet, &data_out, true);

        if (data_out_size > 0 &&
                data_out != NULL)
        {
            // send it out...
            _rcp_server_send_to_one(server, data_out, data_out_size, client);

            RCP_SERVER_MALLOC_DEBUG("+++ data out: %p\n", data_out);
            RCP_FREE(data_out);
            data_out = NULL;
        }

        pe = pe->next;
    }

    // info: free packet without freeing parameter
    rcp_packet_free(packet);

    // send INITIALIZE after sending all parameter
    char data[2] = {COMMAND_INITIALIZE, RCP_TERMINATOR};
    _rcp_server_send_to_one(server, data, 2, client);
}

// receive from transporter
void rcp_server_receive_cb(rcp_server* server, const char* data, size_t size, void* client)
{
    if (server == NULL) return;

    // parse data
    rcp_packet* packet = NULL;

    // NOTE: don't use data and size directly
    // we need it to forward the data in case of COMMAND_UPDATE and COMMAND_UPDATEVALUE
    const char* parse_data = data;
    size_t parse_data_size = size;

    while (parse_data != NULL
           && parse_data_size > 0)
    {
        parse_data = rcp_packet_parse(parse_data, parse_data_size, &packet, &parse_data_size);

        if (parse_data && packet)
        {
            rcp_packet_command command = rcp_packet_get_command(packet);
            switch (command)
            {
            case COMMAND_INFO:
                _do_command_info(server, packet, client);
                break;

            case COMMAND_INITIALIZE:
            {
                RCP_SERVER_DEBUG("INITIALIZE:\n");
                int16_t id_data = rcp_packet_get_iddata(packet);

                // call pre_init cb?

                if (id_data != 0)
                {
                    //
                    RCP_SERVER_DEBUG("not implemented: init with id: %d\n", id_data);
                }
                else
                {
                    // send all data
                    send_initial_parameters(server, client);
                }
                break;
            }

            case COMMAND_DISCOVER:
            {
                // TODO: implement
                RCP_SERVER_DEBUG("not implemented - DISCOVER:\n");
                int16_t id_data = rcp_packet_get_iddata(packet);

                if (id_data != 0)
                {
                    RCP_SERVER_DEBUG("discover with id: %d\n", id_data);
                }
                else
                {
                    // discover root group
                }
                break;
            }

            case COMMAND_UPDATE:
            case COMMAND_UPDATEVALUE:
            {
                // update parameter
                // NOTE: don't take ownership
                rcp_parameter* parameter = rcp_packet_get_parameter(packet);
                if (parameter)
                {
                    if (rcp_parameter_is_type(parameter, DATATYPE_BANG)
                            && (command == COMMAND_UPDATEVALUE
                                || (command == COMMAND_UPDATE
                                    && !rcp_parameter_has_options(parameter)
                                    )
                                ))
                    {
                        rcp_parameter* cached_parameter = rcp_manager_get_parameter(server->manager, rcp_parameter_get_id(parameter));

                        if (cached_parameter)
                        {
                            if (rcp_parameter_is_type(cached_parameter, DATATYPE_BANG))
                            {
                                rcp_bang_parameter_call_bang_cb(RCP_BANG_PARAMETER(cached_parameter));
                            }
                            else
                            {
                                RCP_SERVER_DEBUG("server - bang parameter - type missmatch!\n");
                            }
                        }
                        else
                        {
                            RCP_ERROR("server - bang parameter - no cached parameter\n")
                        }
                    }
                    else
                    {
                        // NOTE: parameter is never added
                        rcp_manager_update_parameter(server->manager, parameter, true);

                        // relay this data to all other clients
                        _rcp_server_send_to_all(server, data, parse_data - data, client);
                    }
                }
                break;
            }

            case COMMAND_REMOVE:
                // no parameter removal on server

            case COMMAND_INVALID:
            case COMMAND_MAX_:
                // nop
                break;
            }

            //
            rcp_packet_free(packet);
            packet = NULL;

            data = parse_data;
        }
    }
}

rcp_value_parameter* rcp_server_expose_bool(rcp_server* server, const char* label, rcp_group_parameter* group)
{
    if (server && server->manager)
    {
        int16_t id = rcp_manager_get_available_id(server->manager);

        if (id > 0)
        {
            // create new paramter
            RCP_SERVER_DEBUG("create bool parameter with id: %d\n", id);
            rcp_value_parameter* p = rcp_bool_parameter_create(id);
            RCP_SERVER_SETUP_PARAMETER(p, server->manager)
            return p;
        }
        else
        {
            RCP_ERROR("could not expose parameter - manager is out of space\n");
        }
    }

    return NULL;
}


rcp_value_parameter* rcp_server_expose_i8(rcp_server* server, const char* label, rcp_group_parameter* group)
{
    if (server && server->manager)
    {
        int16_t id = rcp_manager_get_available_id(server->manager);

        if (id > 0)
        {
            // create new paramter
            RCP_SERVER_DEBUG("create i8 parameter with id: %d\n", id);
            rcp_value_parameter* p = rcp_i8_parameter_create(id);
            RCP_SERVER_SETUP_PARAMETER(p, server->manager)
            return p;
        }
        else
        {
            RCP_ERROR("could not expose parameter - manager is out of space\n");
        }
    }

    return NULL;
}

rcp_value_parameter* rcp_server_expose_i32(rcp_server* server, const char* label, rcp_group_parameter* group)
{
    if (server && server->manager)
    {
        int16_t id = rcp_manager_get_available_id(server->manager);

        if (id > 0)
        {
            // create new paramter
            RCP_SERVER_DEBUG("create i32 parameter with id: %d\n", id);
            rcp_value_parameter* p = rcp_i32_parameter_create(id);
            RCP_SERVER_SETUP_PARAMETER(p, server->manager)
            return p;
        }
        else
        {
            RCP_ERROR("could not expose parameter - manager is out of space\n");
        }
    }

    return NULL;
}

rcp_value_parameter* rcp_server_expose_f32(rcp_server* server, const char* label, rcp_group_parameter* group)
{
	if (server && server->manager)
	{
        int16_t id = rcp_manager_get_available_id(server->manager);

        if (id > 0)
        {
            // create new paramter
            RCP_SERVER_DEBUG("create float parameter with id: %d\n", id);
            rcp_value_parameter* p = rcp_f32_parameter_create(id);
            RCP_SERVER_SETUP_PARAMETER(p, server->manager)
            return p;
        }
        else
        {
            RCP_ERROR("could not expose parameter - manager is out of space\n");
        }
    }

    return NULL;
}

rcp_value_parameter* rcp_server_expose_string(rcp_server* server, const char* label, rcp_group_parameter* group)
{
    if (server && server->manager)
    {
        int16_t id = rcp_manager_get_available_id(server->manager);

        if (id > 0)
        {
            // create new paramter
            RCP_SERVER_DEBUG("create string parameter with id: %d\n", id);
            rcp_value_parameter* p = rcp_string_parameter_create(id);
            RCP_SERVER_SETUP_PARAMETER(p, server->manager)
            return p;
        }
        else
        {
            RCP_ERROR("could not expose parameter - manager is out of space\n");
        }
    }

    return NULL;
}

rcp_value_parameter* rcp_server_expose_custom(rcp_server* server, const char* label, uint32_t size, rcp_group_parameter* group)
{
    if (server && server->manager)
    {
        int16_t id = rcp_manager_get_available_id(server->manager);

        if (id > 0)
        {
            // create new paramter
            RCP_SERVER_DEBUG("create custom parameter with id: %d\n", id);
            rcp_value_parameter* p = rcp_custom_parameter_create(id, size);
            RCP_SERVER_SETUP_PARAMETER(p, server->manager)
            return p;
        }
        else
        {
            RCP_ERROR("could not expose parameter - manager is out of space\n");
        }
    }

    return NULL;
}

rcp_value_parameter* rcp_server_expose_enum(rcp_server* server, const char* label, rcp_group_parameter* group)
{
    if (server && server->manager)
    {
        int16_t id = rcp_manager_get_available_id(server->manager);

        if (id > 0)
        {
            // create new paramter
            RCP_SERVER_DEBUG("create enum parameter with id: %d\n", id);
            rcp_value_parameter* p = rcp_enum_parameter_create(id);
            RCP_SERVER_SETUP_PARAMETER(p, server->manager)
            return p;
        }
        else
        {
            RCP_ERROR("could not expose parameter - manager is out of space\n");
        }
    }

    return NULL;
}

rcp_bang_parameter* rcp_server_expose_bang(rcp_server* server, const char* label, rcp_group_parameter* group)
{
    if (server && server->manager)
    {
        int16_t id = rcp_manager_get_available_id(server->manager);

        if (id > 0)
        {
            // create new paramter
            RCP_SERVER_DEBUG("create bang parameter with id: %d\n", id);
            rcp_bang_parameter* p = rcp_bang_parameter_create(id);
            RCP_SERVER_SETUP_PARAMETER(p, server->manager)
            return p;
        }
        else
        {
            RCP_ERROR("could not expose parameter - manager is out of space\n");
        }
    }

    return NULL;
}

rcp_group_parameter* rcp_server_create_group(rcp_server* server, const char* label, rcp_group_parameter* group)
{
    if (server && server->manager)
    {
        int16_t id = rcp_manager_get_available_id(server->manager);

        if (id > 0)
        {
            // create new paramter
            RCP_SERVER_DEBUG("create group parameter with id: %d\n", id);
            rcp_group_parameter* p = rcp_group_parameter_create(id);
            RCP_SERVER_SETUP_PARAMETER(p, server->manager)
            return p;
        }
        else
        {
            RCP_ERROR("could not expose parameter - manager is out of space\n");
        }
    }

    return NULL;
}


bool rcp_server_remove_parameter(rcp_server* server, rcp_parameter* parameter)
{
    if (server && parameter && server->manager)
    {
        // remove as server
        return rcp_manager_remove_parameter_id(server->manager, rcp_parameter_get_id(parameter), true);
    }

    return false;
}

bool rcp_server_remove_parameter_id(rcp_server* server, int16_t parameter_id)
{
    if (server && server->manager)
    {
        return rcp_manager_remove_parameter_id(server->manager, parameter_id, true);
    }

    return false;
}


rcp_group_parameter* rcp_server_find_group(rcp_server* server, const char* name, rcp_group_parameter* group)
{
	if (server == NULL) return NULL;
	
	return rcp_manager_find_group(server->manager, name, group);
}
