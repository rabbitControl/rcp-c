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

#include "rcp_manager.h"

#include <string.h>

#include "rcp_memory.h"
#include "rcp_logging.h"
#include "rcp_packet.h"
#include "rcp_parameter.h"


#if defined(RCP_MANAGER_DEBUG_LOG) || defined(RCP_ALL_DEBUG)
#define RCP_MANAGER_DEBUG(...) RCP_DEBUG(__VA_ARGS__)
#else
#define RCP_MANAGER_DEBUG(...)
#endif

#if defined(RCP_MANAGER_MALLOC_DEBUG_LOG) || defined(RCP_ALL_DEBUG)
#define RCP_MANAGER_MALLOC_DEBUG(...) RCP_DEBUG(__VA_ARGS__)
#else
#define RCP_MANAGER_MALLOC_DEBUG(...)
#endif

struct rcp_manager
{
    rcp_parameter_list* parameters;
    rcp_parameter_list* dirty_parameters;
    rcp_parameter_list* removed_parameters; // only used on servers

    uint16_t parameter_count;

    void (*sendDataCbOne)(void* user, const char* data, size_t size, void* client);
    void (*sendDataCbAll)(void* user, const char* data, size_t size);

    void (*parameterAddedCb)(rcp_parameter* parameter, void* user);
    void (*parameterRemovedCb)(rcp_parameter* parameter, void* user);

    void* user;
};

rcp_manager* rcp_manager_create(void* user)
{
    rcp_manager* manager = RCP_CALLOC(1, sizeof(rcp_manager));

    if (manager != NULL)
    {
        RCP_MANAGER_MALLOC_DEBUG("*** manager : %p\n", manager);

        manager->user = user;

        if (manager->user == NULL)
        {
            RCP_MANAGER_DEBUG("!! manager without user");
        }

    }
    else
    {
        RCP_ERROR("could not malloc manager\n");
    }

    return manager;
}

void rcp_manager_free(rcp_manager* manager)
{
    if (manager)
    {
        rcp_manager_clear(manager);

        RCP_MANAGER_MALLOC_DEBUG("+++ manager: %p\n", manager);
        RCP_FREE(manager);
    }
}

void rcp_manager_clear(rcp_manager* manager)
{
    if (manager)
    {
        //------------------
        // remove dirty list
        rcp_parameter_list* pe = manager->dirty_parameters;
        rcp_parameter_list* next;
        while (pe)
        {
            next = pe->next;

            RCP_MANAGER_MALLOC_DEBUG("+++ dirty parameter list entry: %p\n", pe);
            RCP_FREE(pe);

            pe = next;
        }
        manager->dirty_parameters = NULL;


        //--------------------
        // remove removed list
        pe = manager->removed_parameters;
        while (pe)
        {
            next = pe->next;

            rcp_parameter_free(pe->parameter);
            RCP_MANAGER_MALLOC_DEBUG("+++ removed parameter list entry: %p\n", pe);
            RCP_FREE(pe);

            pe = next;
        }
        manager->removed_parameters = NULL;


        //--------------------
        // cleanup parameter
        pe = manager->parameters;
        while (pe)
        {
            next = pe->next;

            rcp_parameter_free(pe->parameter);
            RCP_MANAGER_MALLOC_DEBUG("+++ parameter list entry: %p\n", pe);
            RCP_FREE(pe);

            pe = next;
        }
        manager->parameters = NULL;
    }
}

void rcp_manager_set_data_cb_one(rcp_manager* manager, void (*cb)(void*, const char*, size_t, void*))
{
    if (manager == NULL) return;

    manager->sendDataCbOne = cb;
}

void rcp_manager_set_data_cb_all(rcp_manager* manager, void (*cb)(void*, const char*, size_t))
{
    if (manager == NULL) return;

    manager->sendDataCbAll = cb;
}

void rcp_manager_log(rcp_manager* manager)
{
#ifdef RCP_LOG_INFO
    if (manager == NULL) return;

    RCP_INFO("---- parameters: (%d) ----\n", manager->parameter_count);
    rcp_parameter_list* pe = manager->parameters;
    while (pe)
    {
        rcp_parameter_log(pe->parameter);
        pe = pe->next;
    }

    RCP_INFO("---- dirty parameters ----\n");
    pe = manager->dirty_parameters;
    while (pe)
    {
        RCP_INFO("-- parameter id: %d\n", rcp_parameter_get_id(pe->parameter));
        pe = pe->next;
    }
    RCP_INFO("\n");

    RCP_INFO("---- removed parameters ----\n");
    pe = manager->removed_parameters;
    while (pe)
    {
        RCP_INFO("-- parameter id: %d\n", rcp_parameter_get_id(pe->parameter));
        pe = pe->next;
    }
    RCP_INFO("\n");

#endif
}


rcp_parameter* rcp_manager_get_parameter(rcp_manager* manager, int16_t id)
{
    if (manager == NULL) return NULL;

    rcp_parameter_list* pe = manager->parameters;
    while (pe)
    {
        if (rcp_parameter_get_id(pe->parameter) == id)
        {
            return pe->parameter;
        }

        pe = pe->next;
    }

    return NULL;
}

rcp_parameter_list* rcp_manager_get_paramter_list(rcp_manager* manager)
{
    if (manager != NULL)
    {
        return manager->parameters;
    }
    return NULL;
}

static rcp_parameter* _get_removed_parameter(rcp_manager* manager, int16_t id)
{
    if (manager == NULL) return NULL;

    rcp_parameter_list* pe = manager->removed_parameters;
    while (pe)
    {
        if (rcp_parameter_get_id(pe->parameter) == id)
        {
            return pe->parameter;
        }

        pe = pe->next;
    }

    return NULL;
}


int16_t rcp_manager_get_available_id(rcp_manager* manager)
{
    if (manager == NULL) return 0;

    uint16_t i = 1;
    while (i != 0)
    {
        // look through list
        if (rcp_manager_get_parameter(manager, (int16_t)i) == NULL &&
                _get_removed_parameter(manager, (int16_t)i) == NULL)
        {
            // id is available
            return (int16_t)i;
        }

        i++;
    }

    return 0;
}

static bool _do_add_parameter(rcp_manager* manager, rcp_parameter* parameter, bool is_server)
{
    rcp_parameter_list* new_list_item = RCP_CALLOC(1, sizeof(rcp_parameter_list));

    if (new_list_item == NULL)
    {
        RCP_ERROR("could not alloc list entry for new child!\n");
        return false;
    }

    // all ok
    RCP_MANAGER_MALLOC_DEBUG("*** param list entry: %p\n", new_list_item);

    // set manager
    rcp_parameter_set_manager(parameter, manager);

    if (is_server)
    {
        // on servers added parameters are dirty immediately
        rcp_manager_set_dirty(manager, parameter);
    }

    // resolve parent
    rcp_parameter_resolve_parent(parameter);

    // setup list item
    new_list_item->parameter = parameter;

    // add new parameter to beginning
    new_list_item->next = manager->parameters;
    manager->parameters = new_list_item;

    // count
    manager->parameter_count++;

    if (manager->parameterAddedCb)
    {
        manager->parameterAddedCb(parameter, manager->user);
    }

    return true;
}

// NOTE: only applicable for servers
bool rcp_manager_add_parameter(rcp_manager* manager, rcp_parameter* parameter, bool is_server)
{
    if (!is_server) return false;
    if (manager == NULL) return false;
    if (parameter == NULL) return false;

    // check if parameter exists
    if (rcp_manager_get_parameter(manager, rcp_parameter_get_id(parameter)) == NULL)
    {
        return _do_add_parameter(manager, parameter, is_server);
    }

    RCP_ERROR("parameter with id %d already in cache\n", rcp_parameter_get_id(parameter));
    return false;
}

// return true if it was added
bool rcp_manager_update_parameter(rcp_manager* manager, rcp_parameter* parameter, bool is_server)
{
    if (manager == NULL) return false;
    if (parameter == NULL) return false;

    // look for parameter in cache
    rcp_parameter* cached_parameter = rcp_manager_get_parameter(manager, rcp_parameter_get_id(parameter));

    if (cached_parameter != NULL)
    {
        RCP_MANAGER_DEBUG("update cached parameter - server: %d\n", is_server);

        // update option chain
        rcp_parameter_copy_from(cached_parameter, parameter);
    }
    else if (!is_server)
    {
        // only on clients:
        // add it to the cache     
        if (_do_add_parameter(manager, parameter, is_server))
        {
            return true;
        }

        RCP_MANAGER_DEBUG("could not add parameter to cache %d\n", rcp_parameter_get_id(parameter));
    }
    else
    {
        RCP_MANAGER_DEBUG("parameter not in chache - server can not add parameter!\n");
    }

    return false;
}

bool rcp_manager_remove_parameter_id(rcp_manager* manager, int16_t parameter_id, bool is_server)
{
    if (manager == NULL) return false;

    rcp_parameter_list* one_before = NULL;
    rcp_parameter_list* entry = NULL;
    rcp_parameter_list* next = NULL;

    // first remove id from dirty list
    entry = manager->dirty_parameters;
    while (entry)
    {
        next = entry->next;

        if (rcp_parameter_get_id(entry->parameter) == parameter_id)
        {
            RCP_MANAGER_DEBUG("remove dirty parameter: %d\n", parameter_id);

            // remove from list
            if (one_before == NULL)
            {
                manager->dirty_parameters = entry->next;
            }
            else
            {
                one_before->next = entry->next;
            }

            // free this item
            RCP_MANAGER_MALLOC_DEBUG("+++ dirty parameter list entry: %p\n", entry);
            RCP_FREE(entry);

            // we only hold one parameter with that id
            // ok to break
            break;
        }
        else
        {
            one_before = entry;
        }

        entry = next;
    }


    // second remove parameter
    one_before = NULL;
    entry = manager->parameters;
    while (entry)
    {
        next = entry->next;

        if (rcp_parameter_get_id(entry->parameter) == parameter_id)
        {
            RCP_MANAGER_DEBUG("remove parameter: %d\n", parameter_id);

            // remove from list
            if (one_before == NULL)
            {
                manager->parameters = entry->next;
            }
            else
            {
                one_before->next = entry->next;
            }

            manager->parameter_count--;

            //
            if (manager->parameterRemovedCb)
            {
                manager->parameterRemovedCb(entry->parameter, manager->user);
            }


            if (rcp_parameter_is_group(entry->parameter))
            {
                // also remove children
                rcp_parameter_list* children = rcp_group_get_children(RCP_GROUP_PARAMETER(entry->parameter));
                while (children != NULL)
                {
                    // INFO: pass is_server = false to free parameter
                    // on server:
                    //      we only need to send remove for the outer most group parameter
                    //      all other parameters can get destroyed immediately
                    // on client:
                    //      we also destroy the outer most group parameter
                    rcp_manager_remove_parameter_id(manager, rcp_parameter_get_id(children->parameter), false);
                    children = children->next;
                }
            }


            if (is_server)
            {
                // add to removed parameter list
                // server sends remove command on next update
                // INFO: parameter gets freed after sending that remove command

                // recycle that list-item
                entry->next = manager->removed_parameters;
                manager->removed_parameters = entry;
            }
            else
            {
                // free parameter
                rcp_parameter_free(entry->parameter);

                // free list item
                RCP_MANAGER_MALLOC_DEBUG("+++ parameter list entry: %p\n", entry);
                RCP_FREE(entry);
            }

            return true;
        }

        one_before = entry;
        entry = next;
    }

    return false;
}


void rcp_manager_set_dirty(rcp_manager* manager, rcp_parameter* parameter)
{
    if (manager == NULL) return;
    if (parameter == NULL) return;

    rcp_parameter_list* pl = manager->dirty_parameters;
    while (pl)
	{
        if (pl->parameter == parameter ||
			rcp_parameter_get_id(pl->parameter) == rcp_parameter_get_id(parameter))
		{
            // already in list
            return;
        }

        pl = pl->next;
    }

    // add to dirty-list
    rcp_parameter_list* new_entry = RCP_CALLOC(1, sizeof(rcp_parameter_list));
    if (new_entry)
	{
        RCP_MANAGER_MALLOC_DEBUG("*** dirty parameter list entry: %p\n", new_entry);

		new_entry->parameter = parameter;
        new_entry->next = manager->dirty_parameters;
        manager->dirty_parameters = new_entry;
    }
    else
    {
        RCP_ERROR("could not alloc list element for dirty parameter\n");
    }
}

void rcp_manager_update(rcp_manager* manager)
{
    if (manager == NULL) return;

    rcp_packet* packet;
    size_t data_out_size = 0;
    char* data_out = NULL;

    rcp_parameter_list* pl;
    rcp_parameter_list* next;

    //-----------------------------
    // send remove parameters first
    if (manager->removed_parameters != NULL)
    {
        pl = manager->removed_parameters;
        packet = rcp_packet_create(COMMAND_REMOVE);
        while (pl)
        {
            next = pl->next;

            // only serialize if we have a callback
            if (packet &&
                    manager->sendDataCbAll != NULL)
            {
                rcp_packet_set_iddata(packet, rcp_parameter_get_id(pl->parameter));

                data_out_size = rcp_packet_write(packet, &data_out, false);

                if (data_out_size > 0 &&
                        data_out != NULL)
                {
                    // send it out...
                    manager->sendDataCbAll(manager->user, data_out, data_out_size);

                    RCP_MANAGER_MALLOC_DEBUG("+++ data out: %p\n", data_out);
                    RCP_FREE(data_out);
                    data_out = NULL;
                }
            }

            // free parameter and list entry
            rcp_parameter_free(pl->parameter);
            RCP_MANAGER_MALLOC_DEBUG("+++ parameter list entry: %p\n", pl);
            RCP_FREE(pl);

            pl = next;
        } // while

        manager->removed_parameters = NULL;

        if (packet)
        {
            rcp_packet_free(packet);
            packet = NULL;
        }
    }


    //-----------------------------
    // send < parameters
    if (manager->dirty_parameters != NULL)
    {
        pl = manager->dirty_parameters;
        packet = rcp_packet_create(COMMAND_UPDATE);
        while (pl != NULL)
        {
            next = pl->next;

    //        RCP_MANAGER_DEBUG("sending dirty parameter(%d) - %p\n", parameter_get_id(pl->parameter), pl->parameter);

            if (packet &&
                    manager->sendDataCbAll != NULL)
            {
                // set command
                if (rcp_parameter_only_value_changed(pl->parameter))
                {
                    rcp_packet_set_command(packet, COMMAND_UPDATEVALUE);
                }
                else
                {
                    rcp_packet_set_command(packet, COMMAND_UPDATE);
                }

                // set parameter (no transfer)
                rcp_packet_set_parameter(packet, pl->parameter);

                data_out_size = rcp_packet_write(packet, &data_out, false);

                if (data_out_size > 0 &&
                        data_out != NULL)
                {
                    // send it out...
                    manager->sendDataCbAll(manager->user, data_out, data_out_size);

                    RCP_MANAGER_MALLOC_DEBUG("+++ data out: %p\n", data_out);
                    RCP_FREE(data_out);
                    data_out = NULL;
                }
            }

            // remove list element
            RCP_MANAGER_MALLOC_DEBUG("+++ dirty parameter list entry: %p\n", pl);
            RCP_FREE(pl);

            pl = next;
        } // while

        manager->dirty_parameters = NULL;

        //
        if (packet)
        {
            rcp_packet_free(packet);
        }
    }
}

void rcp_manager_set_parameter_added_cb(rcp_manager* manager, void (*cb)(rcp_parameter* parameter, void* user))
{
    if (manager)
    {
        manager->parameterAddedCb = cb;
    }
}

void rcp_manager_set_parameter_removed_cb(rcp_manager* manager, void (*cb)(rcp_parameter* parameter, void* user))
{
    if (manager)
    {
        manager->parameterRemovedCb = cb;
    }
}

rcp_group_parameter* rcp_manager_find_group(rcp_manager* manager, const char* name, rcp_group_parameter* group)
{
	if (manager == NULL) return NULL;
    if (name == NULL) return NULL;
	
	rcp_parameter_list* list_item = manager->parameters;
	
	if (group != NULL)
	{
		list_item = rcp_group_get_children(group);
	}
	
	// look through list
    const char* label = NULL;
	while (list_item != NULL)
	{
        label = rcp_parameter_get_label(list_item->parameter);
		if (label != NULL &&
                rcp_parameter_is_group(list_item->parameter) &&
                strcmp(label, name) == 0)
		{
			return RCP_GROUP_PARAMETER(list_item->parameter);
		}
		list_item = list_item->next;
	}
	
	// not found
	return NULL;
}

rcp_parameter* rcp_manager_find_parameter(rcp_manager* manager, const char* name, rcp_group_parameter* group)
{
    if (manager == NULL) return NULL;
    if (name == NULL) return NULL;

	rcp_parameter_list* list_item = manager->parameters;

	if (group != NULL)
	{
		list_item = rcp_group_get_children(group);
	}

	// look through list
    const char* label = NULL;
	while (list_item != NULL)
	{
        if (rcp_parameter_get_parent(list_item->parameter) == group)
        {
            label = rcp_parameter_get_label(list_item->parameter);
            if (label != NULL &&
                    strcmp(label, name) == 0)
            {
                return list_item->parameter;
            }
        }

		list_item = list_item->next;
	}

	// not found
	return NULL;
}
