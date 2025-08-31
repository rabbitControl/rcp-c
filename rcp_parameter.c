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

#include "rcp_parameter.h"

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <stdarg.h>

#include "rcp.h"
#include "rcp_memory.h"
#include "rcp_langstr.h"
#include "rcp_parser.h"
#include "rcp_string.h"
#include "rcp_logging.h"
#include "rcp_endian.h"
#include "rcp_manager.h"
#include "rcp_typedefinition.h"


#if defined(RCP_PARAMETER_DEBUG_LOG) || defined(RCP_ALL_DEBUG)
#define RCP_PARAMETER_DEBUG(...) RCP_DEBUG(__VA_ARGS__)
#else
#define RCP_PARAMETER_DEBUG(...)
#endif

#if defined(RCP_PARAMETER_MALLOC_DEBUG_LOG) || defined(RCP_ALL_DEBUG)
#define RCP_PARAMETER_MALLOC_DEBUG(...) RCP_DEBUG(__VA_ARGS__)
#else
#define RCP_PARAMETER_MALLOC_DEBUG(...)
#endif


struct rcp_parameter
{
    // mandatory
    int16_t id;
    rcp_typedefinition* typedefinition;

    // options
    rcp_option* options;
//    option* removed_options;

    void (*optionUpdatedCb)(rcp_parameter*, void* user);
    rcp_manager* manager;
    rcp_group_parameter* parent;
	
	void* user;
};

struct rcp_bang_parameter
{
    rcp_parameter parameter_base;

    void (*bangCb)(rcp_bang_parameter* parameter, void* user);
};

struct rcp_group_parameter
{
    rcp_parameter parameter_base;

    rcp_parameter_list* children;
};

struct rcp_value_parameter
{
    rcp_parameter parameter_base;

    rcp_option* value_option;
    void (*valueUpdatedCb)(rcp_value_parameter*, void* user);
};



static rcp_value_parameter* _create_value_parameter(int16_t id, rcp_datatype typeid)
{
    if (id == 0)
    {
        RCP_PARAMETER_DEBUG("reject creating parameter with id 0\n");
        return NULL;
    }

    rcp_value_parameter* parameter = (rcp_value_parameter*)RCP_CALLOC(1, sizeof(rcp_value_parameter));

    if (parameter)
    {
        RCP_PARAMETER_MALLOC_DEBUG("*** param: %p\n", parameter);

        RCP_PARAMETER(parameter)->typedefinition = rcp_typedefinition_create(typeid);
        RCP_PARAMETER(parameter)->id = id;
    }

    return parameter;
}

static inline bool is_value_type(rcp_datatype type)
{
    return type != DATATYPE_INVALID &&
            type != DATATYPE_BANG &&
            type != DATATYPE_GROUP &&
            type != DATATYPE_MAX_;
}

rcp_value_parameter* rcp_bool_parameter_create(int16_t id)
{
    return _create_value_parameter(id, DATATYPE_BOOLEAN);
}

rcp_value_parameter* rcp_i8_parameter_create(int16_t id)
{
    return _create_value_parameter(id, DATATYPE_INT8);
}

rcp_value_parameter* rcp_u8_parameter_create(int16_t id)
{
    return _create_value_parameter(id, DATATYPE_UINT8);
}

rcp_value_parameter* rcp_i16_parameter_create(int16_t id)
{
    return _create_value_parameter(id, DATATYPE_INT16);
}

rcp_value_parameter* rcp_u16_parameter_create(int16_t id)
{
    return _create_value_parameter(id, DATATYPE_UINT16);
}

rcp_value_parameter* rcp_i32_parameter_create(int16_t id)
{
    return _create_value_parameter(id, DATATYPE_INT32);
}

rcp_value_parameter* rcp_u32_parameter_create(int16_t id)
{
    return _create_value_parameter(id, DATATYPE_UINT32);
}

rcp_value_parameter* rcp_f32_parameter_create(int16_t id)
{
    return _create_value_parameter(id, DATATYPE_FLOAT32);
}


// vector
rcp_value_parameter* rcp_vector2f32_parameter_create(int16_t id)
{
    return _create_value_parameter(id, DATATYPE_VECTOR2F32);
}



rcp_value_parameter* rcp_string_parameter_create(int16_t id)
{
    return _create_value_parameter(id, DATATYPE_STRING);
}

rcp_value_parameter* rcp_enum_parameter_create(int16_t id)
{
    return _create_value_parameter(id, DATATYPE_ENUM);
}

rcp_value_parameter* rcp_ipv4_parameter_create(int16_t id)
{
    return _create_value_parameter(id, DATATYPE_IPV4);
}

rcp_bang_parameter* rcp_bang_parameter_create(int16_t id)
{
    if (id == 0) return NULL;

    rcp_bang_parameter* parameter = (rcp_bang_parameter*)RCP_CALLOC(1, sizeof(rcp_bang_parameter));

    if (parameter)
    {
        RCP_PARAMETER_MALLOC_DEBUG("*** param: %p\n", parameter);

        // create typedef
        parameter->parameter_base.typedefinition = rcp_typedefinition_create(DATATYPE_BANG);
        parameter->parameter_base.id = id;
    }

    return parameter;
}

rcp_value_parameter* rcp_custom_parameter_create(int16_t id, uint32_t size)
{
    rcp_value_parameter* param = _create_value_parameter(id, DATATYPE_CUSTOMTYPE);
    if (param)
    {
        rcp_typedefinition_custom* p = (rcp_typedefinition_custom*)param->parameter_base.typedefinition;
        rcp_typedefinition_custom_set_size(p, size);
    }

    return param;
}


rcp_group_parameter* rcp_group_parameter_create(int16_t id)
{
    if (id == 0) return NULL;

    rcp_group_parameter* parameter = (rcp_group_parameter*)RCP_CALLOC(1, sizeof(rcp_group_parameter));

    if (parameter)
    {
        RCP_PARAMETER_MALLOC_DEBUG("*** param: %p\n", parameter);

        // create typedef
        RCP_PARAMETER(parameter)->typedefinition = rcp_typedefinition_create(DATATYPE_GROUP);
        RCP_PARAMETER(parameter)->id = id;
    }

    return parameter;
}




static void free_group_parameter(rcp_group_parameter* group)
{
    if (group == NULL) return;

    // free group parameter specific data
    rcp_parameter_list* list = group->children;
    rcp_parameter_list* next;
    while (list != NULL)
    {
        next = list->next;

        // INFO: don't free children here! (just cleanup the list)
        // parameters are either manager in the manager or have to be freed manually

        RCP_PARAMETER_MALLOC_DEBUG("+++ children list item: %p\n", list);
        RCP_FREE(list);

        list = next;
    }
}

void rcp_parameter_free(rcp_parameter* parameter)
{
    if (parameter == NULL) return;

    // before freeing base parameter, free type specific data
    if (rcp_parameter_is_group(parameter))
    {
        free_group_parameter(RCP_GROUP_PARAMETER(parameter));
    }

    //---
    // free all option

    RCP_PARAMETER_DEBUG("free all option of parameter: %d\n", parameter->id);

    rcp_option_free_chain(parameter->options);
//    free_option_chain(param->removed_options);

    // free type options
    rcp_typedefinition_free(parameter->typedefinition);
    parameter->typedefinition = NULL;

    RCP_PARAMETER_MALLOC_DEBUG("+++ parameter: %p\n", parameter);
    RCP_FREE(parameter);
}

bool rcp_parameter_is_type(rcp_parameter* parameter, rcp_datatype type)
{
    if (parameter)
    {
        return RCP_IS_TYPE(parameter, type);
    }

    return false;
}

bool rcp_parameter_has_options(rcp_parameter* parameter)
{
    if (parameter != NULL)
    {
        return parameter->options != NULL;
    }

    return false;
}

void rcp_parameter_set_user(rcp_parameter* parameter, void* user)
{
	if (parameter != NULL)
    {
        parameter->user = user;
    }
}

void rcp_parameter_set_updated_cb(rcp_parameter* parameter, void (*cb)(rcp_parameter*, void*))
{
    if (parameter != NULL)
    {
        parameter->optionUpdatedCb = cb;
    }
}

void rcp_parameter_set_value_updated_cb(rcp_value_parameter* parameter, void (*cb)(rcp_value_parameter*, void*))
{
    if (parameter != NULL)
    {
        parameter->valueUpdatedCb = cb;
    }
}

void rcp_parameter_copy_from(rcp_parameter* dst, rcp_parameter* src)
{
    if (dst == NULL) return;
    if (src == NULL) return;
    if (dst == src) return;

    // check id
    if (dst->id != src->id)
    {
        RCP_ERROR("can not update parameters with different id: %d != %d\n", dst->id, src->id);
        return;
    }

    if (RCP_TYPE_ID(dst) != RCP_TYPE_ID(src))
    {
        RCP_ERROR("can not update parameters with different type: %d != %d\n", RCP_TYPE_ID(dst), RCP_TYPE_ID(src));
        return;
    }


    bool call_update_cb = false;
    rcp_value_parameter* value_parameter = NULL;

    rcp_option* src_opt = src->options;
	while (src_opt != NULL)
	{
        RCP_PARAMETER_DEBUG("from opt: %p (%d)\n", src_opt, rcp_option_get_prefix(src_opt));
		
        // add option or update existing option
        rcp_option* dst_opt = rcp_option_add_or_update(&dst->options, src_opt);
		
        if (rcp_option_get_prefix(src_opt) == PARAMETER_OPTIONS_VALUE)
        {
            rcp_value_parameter* dst_val = RCP_VALUE_PARAMETER(dst);

            // in case option was added we need to set value_option
            if (dst_val->value_option == NULL)
            {
                dst_val->value_option = dst_opt;
            }

            if (rcp_option_is_changed(dst_val->value_option))
            {
                value_parameter = dst_val;
            }
        }
        else
        {
            call_update_cb = true;

            if (rcp_option_get_prefix(src_opt) == PARAMETER_OPTIONS_PARENTID)
            {
                rcp_parameter_resolve_parent(dst);
            }
        }

        src_opt = rcp_option_get_next(src_opt);
    } // while


    // call update callbacks

    if (value_parameter)
    {
        if (value_parameter->valueUpdatedCb != NULL)
        {
            value_parameter->valueUpdatedCb(value_parameter, RCP_PARAMETER(value_parameter)->user);
        }

//        OPTION_UNSET_CHANGED(value_parameter->value_option);
    }

    if (call_update_cb)
    {
        if (dst->optionUpdatedCb != NULL)
        {
            dst->optionUpdatedCb(dst, dst->user);
        }
    }
}


// TODO?
//static bool remove_parameter_option(rcp_parameter* param, char prefix)
//{

//    if (param == NULL) return false;

//    option* opt = param->options;
//    option* last_opt = 0;

//    while (opt != 0)
//    {

//        if (option_get_prefix(opt) == prefix)
//        {
//            if (last_opt != 0)
//            {
//                // remove opt from chain
//                option_set_next(last_opt, option_get_next(opt));
//            }

//            // add option to beginning of removed_options
//            // we need this information to send default-value for this option on next update
//            // cleanup is done when default value was sent or the parameter is cleaned up
//            option_set_next(opt, param->removed_options);
//            param->removed_options = opt;
//            return true;
//        }

//        last_opt = opt;
//        opt = option_get_next(opt);
//    }

//    // did not remove option
//    return false;
//}


int16_t rcp_parameter_get_id(rcp_parameter* parameter)
{
    if (parameter == NULL) return 0;
    return parameter->id;
}

rcp_typedefinition* rcp_parameter_get_typedefinition(rcp_parameter* parameter)
{
    if (parameter == NULL) return NULL;
    return parameter->typedefinition;
}



//

bool rcp_parameter_has_option(rcp_parameter* parameter, rcp_parameter_options option)
{
    if (parameter == NULL) return false;

    return rcp_option_get(parameter->options, (char)option) != NULL;
}


void rcp_bang_parameter_set_bang_cb(rcp_bang_parameter* parameter, void (*fn)(rcp_bang_parameter* parameter, void* user))
{
    if (parameter == NULL) return;

    parameter->bangCb = fn;
}

void rcp_bang_parameter_call_bang_cb(rcp_bang_parameter* parameter)
{
    if (parameter == NULL) return;
    if (parameter->bangCb == NULL) return;

    parameter->bangCb(parameter, parameter->parameter_base.user);
}

void rcp_bang_parameter_set_dirty(rcp_bang_parameter* parameter)
{
    if (parameter == NULL) return;
    rcp_manager_set_dirty(RCP_PARAMETER(parameter)->manager, RCP_PARAMETER(parameter));
}

/*
 * LABEL
 *
 *
 *
 */
void rcp_parameter_set_label(rcp_parameter* parameter, const char* label)
{
    if (parameter == NULL) return;
    if (label == NULL) return;

    // label prefix: 0x21

    // check if we have a label option already
    rcp_option* opt = rcp_option_get_create(&parameter->options, PARAMETER_OPTIONS_LABEL);

    if (rcp_option_copy_any_language(opt, label, TINY_STRING))
    {
        rcp_manager_set_dirty(parameter->manager, parameter);
    }
}

const char* rcp_parameter_get_label(rcp_parameter* parameter)
{
    if (parameter == NULL) return NULL;

    // check if we have that option
    rcp_option* opt = rcp_option_get(parameter->options, PARAMETER_OPTIONS_LABEL);

    return rcp_option_get_any_language(opt);
}

//bool remove_label(rcp_parameter* parameter)
//{
//    if (parameter == NULL) return false;

//    if (remove_parameter_option(parameter, PARAMETER_OPTIONS_LABEL))
//    {
//        set_dirty(parameter->manager, parameter);
//        return true;
//    }

//    return false;
//}

/*
 * DESCRIPTION
 *
 *
 *
 */
void rcp_parameter_set_description(rcp_parameter* parameter, const char* str)
{
    if (parameter == NULL) return;

    // label prefix: 0x22

    // check if we got option already
    rcp_option* opt = rcp_option_get_create(&parameter->options, PARAMETER_OPTIONS_DESCRIPTION);

    if (rcp_option_copy_any_language(opt, str, SHORT_STRING))
    {
        rcp_manager_set_dirty(parameter->manager, parameter);
    }
}

const char* rcp_parameter_get_description(rcp_parameter* parameter)
{
    if (parameter == NULL) return NULL;

    // get option
    rcp_option* opt = rcp_option_get(parameter->options, PARAMETER_OPTIONS_DESCRIPTION);

    return rcp_option_get_any_language(opt);
}


static void _add_child(rcp_group_parameter* group, rcp_parameter* parameter)
{
    if (group == NULL) return;
    if (parameter == NULL) return;

    rcp_parameter_list* child = group->children;
    while (child)
    {
        if (child->parameter->id == parameter->id)
        {
            // already a child
            return;
        }

        child = child->next;
    }

    rcp_parameter_list* new_child = RCP_MALLOC(sizeof(rcp_parameter_list));
    if (new_child)
    {
        RCP_PARAMETER_MALLOC_DEBUG("*** param list entry: %p\n", new_child);

        new_child->next = group->children;
        new_child->parameter = parameter;
        group->children = new_child;
    }
    else
    {
        RCP_ERROR("could not alloc list entry for new child!\n");
    }
}


static void _remove_from_parent(rcp_parameter* parameter)
{
    if (parameter == NULL) return;
    if (parameter->parent == NULL) return;

    rcp_parameter_list* child_item = parameter->parent->children;
    rcp_parameter_list* prev_child_item = NULL;

    while (child_item)
    {
        if (child_item->parameter->id == parameter->id)
        {
            // is a child, remove
            if (prev_child_item != NULL)
            {
                prev_child_item->next = child_item->next;
            }
            else
            {
                // first entry
                parameter->parent->children = child_item->next;
            }

            child_item->next = NULL;
            RCP_PARAMETER_MALLOC_DEBUG("+++ param list entry: %p\n", child_item);
            RCP_FREE(child_item);

            return;
        }

        prev_child_item = child_item;
        child_item = child_item->next;
    }
}

//-------------------
// group parameter

rcp_parameter_list* rcp_group_get_children(rcp_group_parameter* group)
{
	if (group == NULL) return NULL;
	return group->children;
}

/*
 * parent
 *
 *
 *
 */
void rcp_parameter_set_parent(rcp_parameter* parameter, rcp_group_parameter* group)
{
    RCP_PARAMETER_DEBUG("parameter set parent: %d\n", rcp_parameter_get_id(group));

    if (parameter == NULL) return;
    if (parameter->parent == NULL && group == NULL) return;

    if (parameter->parent != NULL)
    {
        // remove from parent
        _remove_from_parent(parameter);
    }

    parameter->parent = group;
    _add_child(group, parameter);

    // set option
    rcp_option* opt = rcp_option_get_create(&parameter->options, PARAMETER_OPTIONS_PARENTID);
    rcp_option_free_data(opt);
    if (rcp_option_set_i16(opt, rcp_parameter_get_id(RCP_PARAMETER(group))))
    {
        rcp_manager_set_dirty(parameter->manager, parameter);
    }
}

rcp_group_parameter* rcp_parameter_get_parent(rcp_parameter* parameter)
{
    if (parameter) return parameter->parent;
    return NULL;
}

void rcp_parameter_resolve_parent(rcp_parameter* parameter)
{
    if (parameter == NULL) return;

    rcp_option* parent_option = rcp_option_get(parameter->options, PARAMETER_OPTIONS_PARENTID);
    if (parent_option)
    {
        int16_t parent_id = rcp_option_get_i16(parent_option);

        if (parent_id == 0)
        {
            // just remove from parent
            _remove_from_parent(parameter);
            parameter->parent = NULL;
            return;
        }

        if (parameter->parent != NULL)
        {
            if (rcp_parameter_get_id(RCP_PARAMETER(parameter->parent)) == parent_id)
            {
                // same id - nothing changed - done
                return;
            }
        }

        // look for group parameter
        rcp_parameter* group = rcp_manager_get_parameter(parameter->manager, parent_id);
        if (group)
        {
            if (rcp_parameter_is_group(group))
            {
                // set as parent
                rcp_parameter_set_parent(parameter, RCP_GROUP_PARAMETER(group));
            }
            else
            {
                RCP_ERROR("parent is not a group-parameter! id: %d\n", parent_id);
            }
        }
        else
        {
            RCP_PARAMETER_DEBUG("could not find group with id: %d\n", parent_id);
        }
    }
    else
    {
        // remove from parent
        _remove_from_parent(parameter);
    }

}


/*
 * user data
 *
 *
 *
 */
// set external data
void rcp_parameter_set_userdata(rcp_parameter* parameter, void* data, size_t size)
{
    if (parameter == NULL) return;

    // check if we have that option
    rcp_option* opt = rcp_option_get_create(&parameter->options, PARAMETER_OPTIONS_USERDATA);

    // set data
    if (opt != NULL &&
            rcp_option_set_data(opt, data, size, true))
    {
        rcp_manager_set_dirty(parameter->manager, parameter);
    }
}

void rcp_parameter_copy_userdata(rcp_parameter* parameter, void* data, size_t size)
{
    if (parameter == NULL) return;

    // check if we have that option
    rcp_option* opt = rcp_option_get_create(&parameter->options, PARAMETER_OPTIONS_USERDATA);

    // copy data
    if (opt != NULL &&
            rcp_option_copy_data(opt, data, size, true))
    {
        rcp_manager_set_dirty(parameter->manager, parameter);
    }
}

void rcp_parameter_get_userdata(rcp_parameter* parameter, void** out_data, size_t* out_size)
{
    if (parameter == NULL) return;

    // check if we have that option
    rcp_option* opt = rcp_option_get(parameter->options, PARAMETER_OPTIONS_USERDATA);
    if (opt) rcp_option_get_data(opt, out_data, out_size);
}


static inline void _parameter_set_string_option(rcp_parameter* parameter,
                                                const char* string,
                                                rcp_parameter_options option)
{
    if (parameter == NULL) return;

    RCP_PARAMETER_DEBUG("parameter_set string option type: %d\n", option);

    // check if we have that option
    rcp_option* opt = rcp_option_get_create(&parameter->options, option);

    RCP_PARAMETER_DEBUG("parameter_set string option: %p\n", opt);

    rcp_option_free_data(opt);

    // set data
    if (rcp_option_copy_string(opt, string, TINY_STRING))
    {
        rcp_manager_set_dirty(parameter->manager, parameter);
    }
}

static inline const char* _parameter_get_string_option(rcp_parameter* parameter,
                                                       rcp_parameter_options option)
{
    if (parameter == NULL) return NULL;

    // check if we have that option
    rcp_option* opt = rcp_option_get(parameter->options, option);

    if (opt)
    {
        return rcp_option_get_string(opt, TINY_STRING);
    }

    // default
    return NULL;
}


/*
 * USERID
 *
 *
 *
 */
void rcp_parameter_set_userid(rcp_parameter* parameter, const char* userid)
{
    _parameter_set_string_option(parameter, userid, PARAMETER_OPTIONS_USERID);
}

const char* rcp_parameter_get_userid(rcp_parameter* parameter)
{
    return _parameter_get_string_option(parameter, PARAMETER_OPTIONS_USERID);
}


/*
 * TAGS
 *
 *
 *
 */
void rcp_parameter_set_tags(rcp_parameter* parameter, const char* tags)
{
    _parameter_set_string_option(parameter, tags, PARAMETER_OPTIONS_TAGS);
}

const char* rcp_parameter_get_tags(rcp_parameter* parameter)
{
    return _parameter_get_string_option(parameter, PARAMETER_OPTIONS_TAGS);
}

/*
 * ORDER
 *
 *
 *
 */
void rcp_parameter_set_order(rcp_parameter* parameter, int32_t order)
{
    if (parameter == NULL) return;

    // check if we have that option
    rcp_option* opt = rcp_option_get_create(&parameter->options, PARAMETER_OPTIONS_ORDER);

    if (rcp_option_get_i32(opt) == order)
    {
        // same value... leave it
        rcp_option_set_changed(opt, false);
        return;
    }

    rcp_option_free_data(opt);

    // set data
    if (rcp_option_set_i32(opt, order))
    {
        rcp_manager_set_dirty(parameter->manager, parameter);
    }
}

int32_t rcp_parameter_get_order(rcp_parameter* parameter)
{
    if (parameter == NULL) return 0;

    // check if we have that option
    rcp_option* opt = rcp_option_get(parameter->options, PARAMETER_OPTIONS_ORDER);

    if (opt)
    {
        return rcp_option_get_i32(opt);
    }

    // default
    return 0;
}

//bool remove_order(rcp_parameter* parameter)
//{
//    if (parameter == NULL) return false;

//    if (remove_parameter_option(parameter, PARAMETER_OPTIONS_ORDER))
//    {
//        set_dirty(parameter->manager, parameter);
//        return true;
//    }

//    return false;
//}


// readonly
void rcp_parameter_set_readonly(rcp_parameter* parameter, bool ro)
{
    if (parameter == NULL) return;

    // check if we have that option
    rcp_option* opt = rcp_option_get_create(&parameter->options, PARAMETER_OPTIONS_READONLY);

    if (rcp_option_get_bool(opt) == ro)
    {
        // same value... leave it
        rcp_option_set_changed(opt, false);
        return;
    }

    rcp_option_free_data(opt);

    // set data
    if (rcp_option_set_bool(opt, ro))
    {
        rcp_manager_set_dirty(parameter->manager, parameter);
    }
}

bool rcp_parameter_get_readonly(rcp_parameter* parameter)
{
    if (parameter == NULL) return 0;

    // check if we have that option
    rcp_option* opt = rcp_option_get(parameter->options, PARAMETER_OPTIONS_READONLY);

    if (opt)
    {
        return rcp_option_get_bool(opt);
    }

    // default
    return false;
}



/*
 * VALUE
 *
 *
 *
 */

static bool validate_value_parameter(rcp_value_parameter* parameter, rcp_datatype type, rcp_datatype type2)
{
    if (parameter == NULL) return false;

    // check if parameter is of correct type
    if (RCP_TYPE_ID(RCP_PARAMETER(parameter)) != type
            && RCP_TYPE_ID(RCP_PARAMETER(parameter)) != type2)
    {
        // error
        RCP_ERROR("value parameter of wrong type! %d != %d\n", RCP_TYPE_ID(RCP_PARAMETER(parameter)), type);
        return false;
    }
	
    if (parameter->value_option == NULL)
    {
		parameter->value_option = rcp_option_get_create(&parameter->parameter_base.options, PARAMETER_OPTIONS_VALUE);
	}

	if (parameter->value_option == NULL)
    {
		// !! error!
		return false;
	}

    return true;
}

//------------------------
// bool parameter
void rcp_parameter_set_value_bool(rcp_value_parameter* parameter, bool value)
{
    if (parameter == NULL) return;

    // check if parameter is of corrent type
    if (validate_value_parameter(parameter, DATATYPE_BOOLEAN, DATATYPE_INVALID))
    {
        if (rcp_option_set_bool(parameter->value_option, value))
        {
            rcp_manager_set_dirty(RCP_PARAMETER(parameter)->manager, RCP_PARAMETER(parameter));
        }
    }
    else
    {
        // error!
    }
}

bool rcp_parameter_get_value_bool(rcp_value_parameter* parameter)
{
    if (parameter == NULL) return 0;

    // check if parameter is of correct type
    if (rcp_typedefinition_get_type_id(parameter->parameter_base.typedefinition) != DATATYPE_BOOLEAN)
    {
        // error
        RCP_ERROR("value parameter of wrong type!\n");
        return false;
    }

    return rcp_option_get_bool(parameter->value_option);
}

//------------------------
// int8 parameter
void rcp_parameter_set_value_int8(rcp_value_parameter* parameter, int8_t value)
{
    if (parameter == NULL) return;

    // check if parameter is of corrent type
    if (validate_value_parameter(parameter, DATATYPE_INT8, DATATYPE_UINT8))
    {
        if (rcp_option_set_i8(parameter->value_option, value))
        {
            rcp_manager_set_dirty(RCP_PARAMETER(parameter)->manager, RCP_PARAMETER(parameter));
        }
    }
}

int8_t rcp_parameter_get_value_int8(rcp_value_parameter* parameter)
{
    if (parameter == NULL) return 0;

    // check if parameter is of correct type
    if (rcp_typedefinition_get_type_id(parameter->parameter_base.typedefinition) != DATATYPE_INT8
            && rcp_typedefinition_get_type_id(parameter->parameter_base.typedefinition) != DATATYPE_UINT8)
    {
        // error
        RCP_ERROR("value parameter of wrong type!\n");
        return 0;
    }

    return rcp_option_get_i8(parameter->value_option);
}

//------------------------
// int16 parameter
void rcp_parameter_set_value_int16(rcp_value_parameter* parameter, int16_t value)
{
    if (parameter == NULL) return;

    // check if parameter is of corrent type
    if (validate_value_parameter(parameter, DATATYPE_INT16, DATATYPE_UINT16))
    {
        if (rcp_option_set_i16(parameter->value_option, value))
        {
            rcp_manager_set_dirty(RCP_PARAMETER(parameter)->manager, RCP_PARAMETER(parameter));
        }
    }
}

int16_t rcp_parameter_get_value_int16(rcp_value_parameter* parameter)
{
    if (parameter == NULL) return 0;

    // check if parameter is of correct type
    if (rcp_typedefinition_get_type_id(parameter->parameter_base.typedefinition) != DATATYPE_INT16
            && rcp_typedefinition_get_type_id(parameter->parameter_base.typedefinition) != DATATYPE_UINT16)
    {
        // error
        RCP_ERROR("value parameter of wrong type!\n");
        return 0;
    }

    return rcp_option_get_i16(parameter->value_option);
}

//------------------------
// int32 parameter
void rcp_parameter_set_value_int32(rcp_value_parameter* parameter, int32_t value)
{
    if (parameter == NULL) return;

    // check if parameter is of corrent type
    if (validate_value_parameter(parameter, DATATYPE_INT32, DATATYPE_UINT32))
    {
        if (rcp_option_set_i32(parameter->value_option, value))
        {
            rcp_manager_set_dirty(RCP_PARAMETER(parameter)->manager, RCP_PARAMETER(parameter));
        }
    }
}

int32_t rcp_parameter_get_value_int32(rcp_value_parameter* parameter)
{
    if (parameter == NULL) return 0;

    // check if parameter is of correct type
    if (rcp_typedefinition_get_type_id(parameter->parameter_base.typedefinition) != DATATYPE_INT32
            && rcp_typedefinition_get_type_id(parameter->parameter_base.typedefinition) != DATATYPE_UINT32)
    {
        // error
        RCP_ERROR("value parameter of wrong type!\n");
        return 0;
    }

    return rcp_option_get_i32(parameter->value_option);
}

//------------------------
// float parameter
void rcp_parameter_set_value_float(rcp_value_parameter* parameter, float value)
{
	if (parameter == NULL) return;

    // check if parameter is of corrent type
    if (validate_value_parameter(parameter, DATATYPE_FLOAT32, DATATYPE_INVALID))
    {
        if (rcp_option_set_f32(parameter->value_option, value))
        {
            rcp_manager_set_dirty(RCP_PARAMETER(parameter)->manager, RCP_PARAMETER(parameter));
        }
    }
}

float rcp_parameter_get_value_float(rcp_value_parameter* parameter)
{
	if (parameter == NULL) return 0;

    // check if parameter is of correct type
    if (rcp_typedefinition_get_type_id(parameter->parameter_base.typedefinition) != DATATYPE_FLOAT32)
    {
        // error
        RCP_ERROR("value parameter of wrong type!\n");
        return 0;
    }

    return rcp_option_get_float(parameter->value_option);
}

//------------------------
// number parameters
static void _set_typdefinition_option_int8(rcp_value_parameter* parameter, char prefix, int8_t value)
{
	if (parameter == NULL) return;

    if (rcp_typedefinition_set_option_i8(RCP_PARAMETER(parameter)->typedefinition, prefix, value))
    {
        rcp_manager_set_dirty(RCP_PARAMETER(parameter)->manager, RCP_PARAMETER(parameter));
    }
}

void rcp_parameter_set_number_scale(rcp_value_parameter* parameter, rcp_number_scale scale)
{
    if (rcp_parameter_is_number(RCP_PARAMETER(parameter)))
	{
        _set_typdefinition_option_int8(parameter, NUMBER_OPTIONS_SCALE, scale);
    }
    else
    {
        // error!
        RCP_PARAMETER_DEBUG("parameter not a number-type");
    }
}

void rcp_parameter_set_number_unit(rcp_value_parameter* parameter, const char* unit)
{
    if (parameter == NULL) return;

    // check if parameter is of correct type
    if (rcp_parameter_is_number(RCP_PARAMETER(parameter)))
	{
        if (rcp_typedefinition_set_option_string_tiny(RCP_PARAMETER(parameter)->typedefinition, NUMBER_OPTIONS_UNIT, unit))
        {
            rcp_manager_set_dirty(RCP_PARAMETER(parameter)->manager, RCP_PARAMETER(parameter));
        }
    }
    else
    {
        // error!
        RCP_PARAMETER_DEBUG("parameter not a number-type");
    }
}

void rcp_parameter_set_default_int8(rcp_value_parameter* parameter, int8_t value)
{
    _set_typdefinition_option_int8(parameter, NUMBER_OPTIONS_DEFAULT, value);
}
void rcp_parameter_set_min_int8(rcp_value_parameter* parameter, int8_t value)
{
    _set_typdefinition_option_int8(parameter, NUMBER_OPTIONS_MINIMUM, value);
}
void rcp_parameter_set_max_int8(rcp_value_parameter* parameter, int8_t value)
{
    _set_typdefinition_option_int8(parameter, NUMBER_OPTIONS_MAXIMUM, value);
}
void rcp_parameter_set_multipleof_int8(rcp_value_parameter* parameter, int8_t value)
{
    _set_typdefinition_option_int8(parameter, NUMBER_OPTIONS_MULTIPLEOF, value);
}


static void _set_typdefinition_option_int16(rcp_value_parameter* parameter, char prefix, int16_t value)
{
	if (parameter == NULL) return;

    if (rcp_typedefinition_set_option_i16(RCP_PARAMETER(parameter)->typedefinition, prefix, value))
    {
        rcp_manager_set_dirty(RCP_PARAMETER(parameter)->manager, RCP_PARAMETER(parameter));
    }
}

void rcp_parameter_set_default_int16(rcp_value_parameter* parameter, int16_t value)
{
    _set_typdefinition_option_int16(parameter, NUMBER_OPTIONS_DEFAULT, value);
}
void rcp_parameter_set_min_int16(rcp_value_parameter* parameter, int16_t value)
{
    _set_typdefinition_option_int16(parameter, NUMBER_OPTIONS_MINIMUM, value);
}
void rcp_parameter_set_max_int16(rcp_value_parameter* parameter, int16_t value)
{
    _set_typdefinition_option_int16(parameter, NUMBER_OPTIONS_MAXIMUM, value);
}
void rcp_parameter_set_multipleof_int16(rcp_value_parameter* parameter, int16_t value)
{
    _set_typdefinition_option_int16(parameter, NUMBER_OPTIONS_MULTIPLEOF, value);
}

static void _set_typdefinition_option_int32(rcp_value_parameter* parameter, char prefix, int32_t value)
{
	if (parameter == NULL) return;

    if (rcp_typedefinition_set_option_i32(RCP_PARAMETER(parameter)->typedefinition, prefix, value))
    {
        rcp_manager_set_dirty(RCP_PARAMETER(parameter)->manager, RCP_PARAMETER(parameter));
    }
}

static int32_t _get_typdefinition_option_int32(rcp_value_parameter* parameter, char prefix, int32_t defaultValue)
{
	if (parameter == NULL) return defaultValue;

    return rcp_typedefinition_get_option_i32(RCP_PARAMETER(parameter)->typedefinition, prefix, defaultValue);
}

void rcp_parameter_set_default_int32(rcp_value_parameter* parameter, int32_t value)
{
	_set_typdefinition_option_int32(parameter, NUMBER_OPTIONS_DEFAULT, value);
}
void rcp_parameter_set_min_int32(rcp_value_parameter* parameter, int32_t value)
{
    _set_typdefinition_option_int32(parameter, NUMBER_OPTIONS_MINIMUM, value);
}
void rcp_parameter_set_max_int32(rcp_value_parameter* parameter, int32_t value)
{
    _set_typdefinition_option_int32(parameter, NUMBER_OPTIONS_MAXIMUM, value);
}
void rcp_parameter_set_multipleof_int32(rcp_value_parameter* parameter, int32_t value)
{
	_set_typdefinition_option_int32(parameter, NUMBER_OPTIONS_MULTIPLEOF, value);
}

int32_t rcp_parameter_get_default_int32(rcp_value_parameter* parameter)
{
    return _get_typdefinition_option_int32(parameter, NUMBER_OPTIONS_DEFAULT, 0);
}
int32_t rcp_parameter_get_min_int32(rcp_value_parameter* parameter)
{
    return _get_typdefinition_option_int32(parameter, NUMBER_OPTIONS_MINIMUM, INT_MIN);
}
int32_t rcp_parameter_get_max_int32(rcp_value_parameter* parameter)
{
    return _get_typdefinition_option_int32(parameter, NUMBER_OPTIONS_MAXIMUM, INT_MAX);
}
int32_t rcp_parameter_get_multipleof_int32(rcp_value_parameter* parameter)
{
    return _get_typdefinition_option_int32(parameter, NUMBER_OPTIONS_MULTIPLEOF, 0);
}

// float
static void _set_typedefinition_option_f32(rcp_value_parameter* parameter, char prefix, float value)
{
	if (parameter == NULL) return;

    // check if parameter is of corrent type
    if (RCP_IS_TYPE(parameter, DATATYPE_FLOAT32))
	{
        if (rcp_typedefinition_set_option_f32(RCP_PARAMETER(parameter)->typedefinition, prefix, value))
		{
            rcp_manager_set_dirty(RCP_PARAMETER(parameter)->manager, RCP_PARAMETER(parameter));
        }
    }
    else
    {
        // error!
    }
}

static float _get_typdefinition_option_f32(rcp_value_parameter* parameter, char prefix, float defaultValue)
{
	if (parameter == NULL) return defaultValue;

    return rcp_typedefinition_get_option_f32(RCP_PARAMETER(parameter)->typedefinition, prefix, defaultValue);
}

void rcp_parameter_set_default_float(rcp_value_parameter* parameter, float value)
{
	_set_typedefinition_option_f32(parameter, NUMBER_OPTIONS_DEFAULT, value);
}
void rcp_parameter_set_min_float(rcp_value_parameter* parameter, float value)
{
	_set_typedefinition_option_f32(parameter, NUMBER_OPTIONS_MINIMUM, value);
}
void rcp_parameter_set_max_float(rcp_value_parameter* parameter, float value)
{
	_set_typedefinition_option_f32(parameter, NUMBER_OPTIONS_MAXIMUM, value);
}
void rcp_parameter_set_multipleof_float(rcp_value_parameter* parameter, float value)
{
	_set_typedefinition_option_f32(parameter, NUMBER_OPTIONS_MULTIPLEOF, value);
}

float rcp_parameter_get_default_float(rcp_value_parameter* parameter)
{
    return _get_typdefinition_option_f32(parameter, NUMBER_OPTIONS_DEFAULT, 0);
}
float rcp_parameter_get_min_float(rcp_value_parameter* parameter)
{
    return _get_typdefinition_option_f32(parameter, NUMBER_OPTIONS_MINIMUM, FLT_MIN);
}
float rcp_parameter_get_max_float(rcp_value_parameter* parameter)
{
    return _get_typdefinition_option_f32(parameter, NUMBER_OPTIONS_MAXIMUM, FLT_MAX);
}
float rcp_parameter_get_multipleof_float(rcp_value_parameter* parameter)
{
    return _get_typdefinition_option_f32(parameter, NUMBER_OPTIONS_MULTIPLEOF, 0);
}


// IPV4

void rcp_parameter_set_value_ipv4(rcp_value_parameter* parameter, uint32_t value)
{
    if (parameter == NULL) return;

    // check if parameter is of corrent type
    if (validate_value_parameter(parameter, DATATYPE_IPV4, DATATYPE_INVALID))
    {
        if (rcp_option_set_i32(parameter->value_option, value))
        {
            rcp_manager_set_dirty(RCP_PARAMETER(parameter)->manager, RCP_PARAMETER(parameter));
        }
    }
}

uint32_t rcp_parameter_get_value_ipv4(rcp_value_parameter* parameter)
{
    if (parameter == NULL) return 0;

    // check if parameter is of correct type
    if (rcp_typedefinition_get_type_id(parameter->parameter_base.typedefinition) != DATATYPE_IPV4)
    {
        // error
        RCP_ERROR("value parameter of wrong type!\n");
        return 0;
    }

    return rcp_option_get_i32(parameter->value_option);
}

void rcp_parameter_set_default_ipv4(rcp_value_parameter* parameter, uint32_t value)
{
    _set_typdefinition_option_int32(parameter, IPV4_OPTIONS_DEFAULT, value);
}

uint32_t rcp_parameter_get_default_ipv4(rcp_value_parameter* parameter)
{
    return _get_typdefinition_option_int32(parameter, IPV4_OPTIONS_DEFAULT, 0);
}


// custom parameter
void rcp_parameter_copy_value_data(rcp_value_parameter* parameter, const char* data, size_t size)
{
    if (parameter == NULL) return;

    if (validate_value_parameter(parameter, DATATYPE_CUSTOMTYPE, DATATYPE_INVALID))
    {
        // check size
        if (size == rcp_typedefinition_custom_get_size((rcp_typedefinition_custom*)parameter->parameter_base.typedefinition))
        {
            if (rcp_option_copy_data(parameter->value_option, (void*)data, size, false))
            {
                rcp_manager_set_dirty(RCP_PARAMETER(parameter)->manager, RCP_PARAMETER(parameter));
            }
        }
        else
        {
            RCP_DEBUG("custom parameter set value size mismatch\n");
        }
    }
}

void rcp_parameter_set_default_data(rcp_value_parameter* parameter, const char* data, size_t size)
{
    if (parameter == NULL) return;

    if (RCP_IS_TYPE(parameter, DATATYPE_CUSTOMTYPE))
    {
        // check size
        if (size == rcp_typedefinition_custom_get_size((rcp_typedefinition_custom*)parameter->parameter_base.typedefinition))
        {
            if (rcp_typedefinition_set_option_data(RCP_PARAMETER(parameter)->typedefinition, CUSTOMTYPE_OPTIONS_DEFAULT, data, size, false))
            {
                rcp_manager_set_dirty(RCP_PARAMETER(parameter)->manager, RCP_PARAMETER(parameter));
            }
        }
        else
        {
            RCP_DEBUG("custom parameter set value size mismatch\n");
        }
    }
}

void rcp_parameter_set_uuid(rcp_value_parameter* parameter, const char* uuid, size_t size)
{
    if (parameter == NULL) return;
    if (size != RCP_CUSTOMTYPE_UUID_LENGTH) return;

    if (RCP_IS_TYPE(parameter, DATATYPE_CUSTOMTYPE))
    {
        if (rcp_typedefinition_set_option_data(parameter->parameter_base.typedefinition, CUSTOMTYPE_OPTIONS_UUID, uuid, size, false))
        {
            rcp_manager_set_dirty(RCP_PARAMETER(parameter)->manager, RCP_PARAMETER(parameter));
        }
    }
}

void rcp_parameter_set_config(rcp_value_parameter* parameter, const char* data, size_t size)
{
    if (parameter == NULL) return;

    if (RCP_IS_TYPE(parameter, DATATYPE_CUSTOMTYPE))
    {
        if (rcp_typedefinition_set_option_data(parameter->parameter_base.typedefinition, CUSTOMTYPE_OPTIONS_CONFIG, data, size, true))
        {
            rcp_manager_set_dirty(RCP_PARAMETER(parameter)->manager, RCP_PARAMETER(parameter));
        }
    }
}


void rcp_parameter_get_value_data(rcp_value_parameter* parameter, const char** out_data, size_t* out_size)
{
    *out_data = NULL;
    *out_size = 0;
    if (parameter == NULL) return;

    return rcp_option_get_data(parameter->value_option, (void**)out_data, out_size);
}

void rcp_parameter_get_default_data(rcp_value_parameter* parameter, const char** out_data, size_t* out_size)
{    
    if (parameter == NULL) return;

    rcp_typedefinition_get_option_data(RCP_PARAMETER(parameter)->typedefinition, CUSTOMTYPE_OPTIONS_DEFAULT, out_data, out_size);
}

void rcp_parameter_get_uuid(rcp_value_parameter* parameter, const char** out_uuid, size_t* out_size)
{
    if (parameter == NULL) return;

    rcp_typedefinition_get_option_data(RCP_PARAMETER(parameter)->typedefinition, CUSTOMTYPE_OPTIONS_UUID, out_uuid, out_size);
}

void rcp_parameter_get_config(rcp_value_parameter* parameter, const char** out_data, size_t* out_size)
{
    if (parameter == NULL) return;

    rcp_typedefinition_get_option_data(RCP_PARAMETER(parameter)->typedefinition, CUSTOMTYPE_OPTIONS_CONFIG, out_data, out_size);
}


// vector

// float
static void _set_typedefinition_option_v2f32(rcp_value_parameter* parameter, char prefix, float x, float y)
{
    if (parameter == NULL) return;
    if (RCP_PARAMETER(parameter)->typedefinition == NULL) return;

    // check if parameter is of corrent type
    if (RCP_IS_TYPE(parameter, DATATYPE_VECTOR2F32))
    {
        if (rcp_typedefinition_set_option_v2f32(RCP_PARAMETER(parameter)->typedefinition, prefix, x, y))
        {
            rcp_manager_set_dirty(RCP_PARAMETER(parameter)->manager, RCP_PARAMETER(parameter));
        }
    }
    else
    {
        // error!
    }
}

static float _get_typdefinition_option_v2f32_x(rcp_value_parameter* parameter, char prefix, float defaultValue)
{
    if (parameter == NULL) return defaultValue;

    return rcp_typedefinition_get_option_v2f32_x(RCP_PARAMETER(parameter)->typedefinition, prefix, defaultValue);
}

static float _get_typdefinition_option_v2f32_y(rcp_value_parameter* parameter, char prefix, float defaultValue)
{
    if (parameter == NULL) return defaultValue;

    return rcp_typedefinition_get_option_v2f32_y(RCP_PARAMETER(parameter)->typedefinition, prefix, defaultValue);
}

// vector2 float
void rcp_parameter_set_value_vector2f32(rcp_value_parameter* parameter, float x, float y)
{
    if (parameter == NULL) return;

    // check if parameter is of corrent type
    if (validate_value_parameter(parameter, DATATYPE_VECTOR2F32, DATATYPE_INVALID))
    {
        if (rcp_option_set_vector2f(parameter->value_option, x, y))
        {
            rcp_manager_set_dirty(RCP_PARAMETER(parameter)->manager, RCP_PARAMETER(parameter));
        }
    }
}

float rcp_parameter_get_value_vector2f32_x(rcp_value_parameter* parameter)
{
    if (parameter == NULL) return 0;

    // check if parameter is of correct type
    if (rcp_typedefinition_get_type_id(parameter->parameter_base.typedefinition) != DATATYPE_VECTOR2F32)
    {
        // error
        RCP_ERROR("value parameter of wrong type!\n");
        return 0;
    }

    return rcp_option_get_vector2f_x(parameter->value_option);
}

float rcp_parameter_get_value_vector2f32_y(rcp_value_parameter* parameter)
{
    if (parameter == NULL) return 0;

    // check if parameter is of correct type
    if (rcp_typedefinition_get_type_id(parameter->parameter_base.typedefinition) != DATATYPE_VECTOR2F32)
    {
        // error
        RCP_ERROR("value parameter of wrong type!\n");
        return 0;
    }

    return rcp_option_get_vector2f_y(parameter->value_option);
}


// vector options
void rcp_parameter_set_default_vector2f32(rcp_value_parameter* parameter, float x, float y)
{
    _set_typedefinition_option_v2f32(parameter, NUMBER_OPTIONS_DEFAULT, x, y);
}

void rcp_parameter_set_min_vector2f32(rcp_value_parameter* parameter, float x, float y)
{
    _set_typedefinition_option_v2f32(parameter, NUMBER_OPTIONS_MINIMUM, x, y);
}

void rcp_parameter_set_max_vector2f32(rcp_value_parameter* parameter, float x, float y)
{
    _set_typedefinition_option_v2f32(parameter, NUMBER_OPTIONS_MAXIMUM, x, y);
}

void rcp_parameter_set_multipleof_vector2f32(rcp_value_parameter* parameter, float x, float y)
{
    _set_typedefinition_option_v2f32(parameter, NUMBER_OPTIONS_MULTIPLEOF, x, y);
}





//------------------------
// string parameter
void rcp_parameter_set_value_string(rcp_value_parameter* parameter, const char* value)
{
    if (parameter == NULL) return;

    // check if parameter is of corrent type
    if (validate_value_parameter(parameter, DATATYPE_STRING, DATATYPE_INVALID))
    {
        if (rcp_option_copy_string(parameter->value_option, value, LONG_STRING))
        {
            rcp_manager_set_dirty(RCP_PARAMETER(parameter)->manager, RCP_PARAMETER(parameter));
        }
    }
    else
    {
        // error!
        RCP_PARAMETER_DEBUG("parameter not of type string");
    }
}

const char* rcp_parameter_get_value_string(rcp_value_parameter* parameter)
{
    if (parameter == NULL) return NULL;

    // check if parameter is of corrent type
    if (rcp_typedefinition_get_type_id(parameter->parameter_base.typedefinition) != DATATYPE_STRING)
    {
        // error
        RCP_ERROR("value parameter of wrong type!\n");
        return NULL;
    }

    return rcp_option_get_string(parameter->value_option, LONG_STRING);
}

//-------------------
// enum parameter
void rcp_parameter_set_value_enum(rcp_value_parameter* parameter, const char* value)
{
    if (parameter == NULL) return;

    // check if parameter is of corrent type
    if (validate_value_parameter(parameter, DATATYPE_ENUM, DATATYPE_INVALID))
    {
        if (rcp_option_copy_string(parameter->value_option, value, TINY_STRING))
        {
            rcp_manager_set_dirty(RCP_PARAMETER(parameter)->manager, RCP_PARAMETER(parameter));
        }
    }
    else
    {
        // error!
        RCP_PARAMETER_DEBUG("parameter not of type enum");
    }
}

void rcp_parameter_set_default_enum(rcp_value_parameter* parameter, const char* value)
{
    if (parameter == NULL) return;

    // check if parameter is of corrent type
    if (RCP_IS_TYPE(parameter, DATATYPE_ENUM))
	{
        if (rcp_typedefinition_set_option_string_tiny(RCP_PARAMETER(parameter)->typedefinition, ENUM_OPTIONS_DEFAULT, value))
		{
            rcp_manager_set_dirty(RCP_PARAMETER(parameter)->manager, RCP_PARAMETER(parameter));
        }
    }
    else
    {
        // error!
        RCP_PARAMETER_DEBUG("parameter not of type enum");
    }
}
void rcp_parameter_set_multiselect_enum(rcp_value_parameter* parameter, bool value)
{
    if (parameter == NULL) return;

    // check if parameter is of corrent type
    if (RCP_IS_TYPE(parameter, DATATYPE_ENUM))
	{
        if (rcp_typedefinition_set_option_bool(RCP_PARAMETER(parameter)->typedefinition, ENUM_OPTIONS_MULTISELECT, value))
		{
            rcp_manager_set_dirty(RCP_PARAMETER(parameter)->manager, RCP_PARAMETER(parameter));
        }
    }
    else
    {
        // error!
        RCP_PARAMETER_DEBUG("parameter not of type enum");
    }
}

void rcp_parameter_set_entries_enum(rcp_value_parameter* parameter, int count, ...)
{
    if (parameter == NULL) return;

    // check if parameter is of corrent type
    if (RCP_IS_TYPE(parameter, DATATYPE_ENUM))
	{
        va_list valist;
        va_start(valist, count);
        if (rcp_typedefinition_set_option_stringlist(RCP_PARAMETER(parameter)->typedefinition, ENUM_OPTIONS_ENTRIES, count, valist))
		{
            rcp_manager_set_dirty(RCP_PARAMETER(parameter)->manager, RCP_PARAMETER(parameter));
        }
        va_end(valist);
    }
    else
    {
        // error!
        RCP_PARAMETER_DEBUG("parameter not of type enum");
    }
}

const char* rcp_parameter_get_value_enum(rcp_value_parameter* parameter)
{
    if (parameter == NULL) return NULL;

    // check if parameter is of corrent type
    if (rcp_typedefinition_get_type_id(parameter->parameter_base.typedefinition) != DATATYPE_ENUM)
    {
        // error
        RCP_ERROR("value parameter of wrong type!\n");
        return NULL;
    }

    return rcp_option_get_string(parameter->value_option, TINY_STRING);
}

const char* rcp_parameter_get_default_enum(rcp_value_parameter* parameter)
{
    if (parameter == NULL) return NULL;

    // check if parameter is of corrent type
    if (rcp_typedefinition_get_type_id(parameter->parameter_base.typedefinition) != DATATYPE_ENUM)
    {
        // error
        RCP_ERROR("value parameter of wrong type!\n");
        return NULL;
    }

    return rcp_typedefinition_get_option_string_tiny(RCP_PARAMETER(parameter)->typedefinition, ENUM_OPTIONS_DEFAULT);
}

bool rcp_parameter_get_multiselect_enum(rcp_value_parameter* parameter)
{
    if (parameter == NULL) return NULL;

    // check if parameter is of corrent type
    if (rcp_typedefinition_get_type_id(parameter->parameter_base.typedefinition) != DATATYPE_ENUM)
    {
        // error
        RCP_ERROR("value parameter of wrong type!\n");
        return NULL;
    }

    return rcp_typedefinition_get_option_bool(RCP_PARAMETER(parameter)->typedefinition, ENUM_OPTIONS_MULTISELECT, false);
}



//------------------------
//
size_t rcp_parameter_get_value_size(rcp_value_parameter* parameter)
{
    if (parameter == NULL) return 0;

    return rcp_option_get_data_size(parameter->value_option);
}

#ifdef RCP_OPTION_USE_EXTERNAL_GET_SET
void rcp_parameter_set_external_value_cb(rcp_value_parameter* parameter, void (*getCb)(void** out_data, size_t* out_size), bool (*setCb)(void* data, size_t size))
{
    if (parameter == NULL) return;

    if (parameter->value_option == NULL)
    {
        parameter->value_option = rcp_option_get_create(&parameter->parameter_base.options, PARAMETER_OPTIONS_VALUE);
    }

    rcp_option_set_external_cb(parameter->value_option, getCb, setCb);
}
#endif



/*
 * PARSE VALUE
 *
 *
 *
 */
const char* rcp_parameter_parse_value(rcp_parameter* parameter, const char* data, size_t* size)
{
    if (parameter == NULL) return NULL;

    if (rcp_typedefinition_get_type_id(parameter->typedefinition) == 0
            || rcp_typedefinition_get_type_id(parameter->typedefinition) >= DATATYPE_MAX_)
    {
        // invalid!
        return NULL;
    }

    if (data == NULL
            || *size == 0)
    {
        return NULL;
    }

    if (is_value_type(rcp_typedefinition_get_type_id(parameter->typedefinition)))
    {
        rcp_option* opt = rcp_option_get_create(&parameter->options, PARAMETER_OPTIONS_VALUE);

        switch (rcp_typedefinition_get_type_id(parameter->typedefinition))
        {
        case DATATYPE_BOOLEAN:
        case DATATYPE_INT8:
        case DATATYPE_UINT8:
        case DATATYPE_INT16:
        case DATATYPE_UINT16:
        case DATATYPE_INT32:
        case DATATYPE_UINT32:
        case DATATYPE_INT64:
        case DATATYPE_UINT64:
        case DATATYPE_FLOAT32:
        case DATATYPE_FLOAT64:
        case DATATYPE_VECTOR2F32:
        case DATATYPE_VECTOR2I32:
        case DATATYPE_VECTOR3F32:
        case DATATYPE_VECTOR3I32:
        case DATATYPE_VECTOR4F32:
        case DATATYPE_VECTOR4I32:
        case DATATYPE_RGB:
        case DATATYPE_IPV4:
        {
            data = rcp_typedefinition_parse_number_value(parameter->typedefinition, data, size, opt);
            if (data == NULL) return NULL;

            RCP_VALUE_PARAMETER(parameter)->value_option = opt;
            break;
        }

        case DATATYPE_STRING:
        case DATATYPE_ENUM:
        {
            data = rcp_typedefinition_parse_string_value(parameter->typedefinition, data, size, opt);
            if (data == NULL) return NULL;

            RCP_VALUE_PARAMETER(parameter)->value_option = opt;
            break;
        }

        case DATATYPE_CUSTOMTYPE:
        {
            uint32_t p_size = rcp_typedefinition_custom_get_size((rcp_typedefinition_custom*)parameter->typedefinition);

            if (*size < p_size)
            {
                return NULL;
            }

            rcp_option_copy_data(opt, data, p_size, false);
            RCP_VALUE_PARAMETER(parameter)->value_option = opt;

            return data + p_size;
        }

//        case DATATYPE_RANGE:
        default:
            RCP_PARAMETER_DEBUG("datatype not implemented\n");
            break;
        }

    }
    else
    {
        RCP_ERROR("parameter typedefinition is no value type!");
    }

    return data;
}


/*
 * PARSE OPTIONS
 *
 *
 *
 */
const char* rcp_parameter_parse_options(rcp_parameter* parameter, const char* data, size_t* size)
{
    if (parameter == NULL) return NULL;
    if (data == NULL || *size == 0) return data;

    if (rcp_typedefinition_get_type_id(parameter->typedefinition) == 0
            || rcp_typedefinition_get_type_id(parameter->typedefinition) >= DATATYPE_MAX_)
    {
        // invalid!
        RCP_PARAMETER_DEBUG("parse_parameter_options - invalid type\n");
        return NULL;
    }


    rcp_option* opt;
    uint8_t option_prefix = 0;

    while (*size > 0)
    {
        option_prefix = 0;

        data = rcp_read_u8(data, size, &option_prefix);
        if (data == NULL) return NULL;

        RCP_PARAMETER_DEBUG("parse_parameter_options - prefix: %d\n", option_prefix);

        if (option_prefix == RCP_TERMINATOR)
        {
            // terminator - end of parameter
            return data;
        }

        if (option_prefix < PARAMETER_OPTIONS_VALUE
                || option_prefix > PARAMETER_OPTIONS_READONLY)
        {
            // invalid option!
            return NULL;
        }

        if (*size == 0) return NULL;

        // ok - continue

        RCP_PARAMETER_DEBUG("parse_parameter_options - option_prefix: %d\n", option_prefix);

        switch ((rcp_parameter_options)option_prefix)
        {
        case PARAMETER_OPTIONS_VALUE:
        {
            const char* r_data = rcp_parameter_parse_value(parameter, data, size);
            if (r_data == NULL)
            {
                return NULL;
            }

            data = r_data;
            break;
        }

        case PARAMETER_OPTIONS_LABEL:
        {
            // language label
            if (*size <= RCP_LANGUAGE_CODE_SIZE) return NULL;

            rcp_language_str* lng_strs = NULL;
            char code[RCP_LANGUAGE_CODE_SIZE];

            while (*data > 0)
            {
                if (*size < RCP_LANGUAGE_CODE_SIZE)
                {
                    rcp_langstr_free_chain(lng_strs);
                    return NULL;
                }

                // enough data - get code
                strncpy(code, data, RCP_LANGUAGE_CODE_SIZE);

                // create language string
                rcp_language_str* lng_str = rcp_langstr_create(code);
                if (lng_str == NULL)
                {
                    rcp_langstr_free_chain(lng_strs);
                    return NULL;
                }

                // ok - continue
                data += RCP_LANGUAGE_CODE_SIZE;
                *size -= RCP_LANGUAGE_CODE_SIZE;

                // tiny string
                uint8_t str_len = 0;
                char* str = NULL;

                const char* r_data = rcp_read_tiny_string(data, size, &str, &str_len);
                if (r_data == NULL)
                {
                    rcp_langstr_free_chain(lng_strs);
                    rcp_langstr_free_chain(lng_str);
                    return NULL;
                }

                data = r_data;

                rcp_langstr_set_string(lng_str, str, str_len, TINY_STRING);
                rcp_langstr_set_next(lng_str, lng_strs);
                lng_strs = lng_str;                

                RCP_PARAMETER_DEBUG("label: %s: %s\n", rcp_langstr_get_code(lng_str), rcp_langstr_get_string(lng_str));
            }
            // step over terminator
            data++;
            *size -= 1;

            if (lng_strs != NULL)
            {
                // got strings...
                opt = rcp_option_get_create(&parameter->options, PARAMETER_OPTIONS_LABEL);
                rcp_option_move_langstr(opt, lng_strs);
            }

            break;
        }

        case PARAMETER_OPTIONS_DESCRIPTION:
        {
            // language description

            // language
            if (*size <= RCP_LANGUAGE_CODE_SIZE) return NULL;

            rcp_language_str* lng_strs = NULL;
            char code[RCP_LANGUAGE_CODE_SIZE];

            while (*data > 0)
            {
                if (*size < RCP_LANGUAGE_CODE_SIZE)
                {
                    rcp_langstr_free_chain(lng_strs);
                    return NULL;
                }

                // enough data - get code
                strncpy(code, data, RCP_LANGUAGE_CODE_SIZE);

                // create language string
                rcp_language_str* lng_str = rcp_langstr_create(code);
                if (lng_str == NULL)
                {
                    rcp_langstr_free_chain(lng_strs);
                    return NULL;
                }

                // ok - continue
                data += RCP_LANGUAGE_CODE_SIZE;
                *size -= RCP_LANGUAGE_CODE_SIZE;

                // short string
                uint16_t str_len;
                char* str;
                const char* r_data = rcp_read_short_string(data, size, &str, &str_len);
                if (r_data == NULL)
                {
                    rcp_langstr_free_chain(lng_strs);
                    rcp_langstr_free_chain(lng_str);
                    return NULL;
                }

                data = r_data;

                rcp_langstr_set_string(lng_str, str, str_len, SHORT_STRING);
                rcp_langstr_set_next(lng_str, lng_strs);
                lng_strs = lng_str;

                RCP_PARAMETER_DEBUG("description: %s: %s\n", rcp_langstr_get_code(lng_str), rcp_langstr_get_string(lng_str));
            }

            // step over terminator
            data++;
            *size -= 1;

            if (lng_strs != NULL)
            {
                // got strings...
                opt = rcp_option_get_create(&parameter->options, PARAMETER_OPTIONS_DESCRIPTION);
                rcp_option_move_langstr(opt, lng_strs);
            }

            break;
        }


        case PARAMETER_OPTIONS_TAGS:
        {
            const char* r_data = rcp_read_tiny_string_option(&parameter->options, data, size, PARAMETER_OPTIONS_TAGS);
            if (r_data == NULL) return NULL;

            data = r_data;
            break;
        }


        case PARAMETER_OPTIONS_ORDER:
        {
            // int32
            opt = rcp_option_get_create(&parameter->options, PARAMETER_OPTIONS_ORDER);
            rcp_option_free_data(opt);

            int32_t d;
            data = rcp_read_i32(data, size, &d);
            if (data == NULL) return NULL;

            rcp_option_set_i32(opt, d);
            break;
        }

        case PARAMETER_OPTIONS_PARENTID:
        {
            // int16
            opt = rcp_option_get_create(&parameter->options, PARAMETER_OPTIONS_PARENTID);
            rcp_option_free_data(opt);

            int16_t d;
            data = rcp_read_i16(data, size, &d);
            if (data == NULL) return NULL;

            RCP_PARAMETER_DEBUG("set parent-id: %d\n", d);
            rcp_option_set_i16(opt, d);
            break;
        }

        case PARAMETER_OPTIONS_WIDGET:
            // not supported
            RCP_PARAMETER_DEBUG("widget option is not supported!");
            return NULL;

        case PARAMETER_OPTIONS_USERDATA:
        {
            uint32_t data_size;
            data = rcp_read_i32(data, size, (int32_t*)&data_size);
            if (data == NULL) return NULL;

            if (*size < data_size)
            {
                RCP_ERROR("error - not enought data to read: %d bytes from %d\n", data_size, *size);
                return NULL;
            }

            // copy data
            opt = rcp_option_get_create(&parameter->options, PARAMETER_OPTIONS_USERDATA);
            if (opt != NULL)
            {
                rcp_option_copy_data(opt, data, data_size, true);

                data += data_size;
                *size -= data_size;
            }
            else
            {
                RCP_ERROR("could not create or get option for userdata\n");
                return NULL;
            }

            break;
        }

        case PARAMETER_OPTIONS_USERID:
        {
            const char* r_data = rcp_read_tiny_string_option(&parameter->options, data, size, PARAMETER_OPTIONS_USERID);
            if (r_data == NULL) return NULL;

            data = r_data;
            break;
        }

        case PARAMETER_OPTIONS_READONLY:
        {
            // int8
            opt = rcp_option_get_create(&parameter->options, PARAMETER_OPTIONS_READONLY);
            rcp_option_free_data(opt);

            int8_t d;
            data = rcp_read_i8(data, size, &d);
            if (data == NULL) return NULL;

            rcp_option_set_bool(opt, (d > 0));
            break;
        }
        }
    }

    // sane packets don't come here
    RCP_ERROR("error parsing parameteroptions\n");
    return NULL;
}


void rcp_parameter_log(rcp_parameter* parameter)
{
#ifdef RCP_LOG_INFO
    if (parameter == NULL) return;

    RCP_INFO("-- parameter id: %d\n", parameter->id);
    rcp_typedefinition_log(parameter->typedefinition);

    if (parameter->options)
    {
        RCP_INFO("  parameter options:\n");
        rcp_option* opt = parameter->options;

        while (opt)
        {
            RCP_INFO("\toption: 0x%02x - ", rcp_option_get_prefix(opt));
            RCP_INFO_ONLY("changed: %d - ", rcp_option_is_changed(opt));

            switch ((rcp_parameter_options)rcp_option_get_prefix(opt))
            {
            case PARAMETER_OPTIONS_VALUE:
                RCP_INFO_ONLY("VALUE: ");
                switch (RCP_TYPE_ID(parameter))
                {
                case DATATYPE_BOOLEAN:
                    RCP_INFO_ONLY("%d\n", rcp_option_get_bool(opt));
                    break;

                case DATATYPE_INT8:
                    RCP_INFO_ONLY("%d\n", rcp_option_get_i8(opt));
                    break;
                case DATATYPE_UINT8:
                    RCP_INFO_ONLY("%u\n", (uint8_t)rcp_option_get_i8(opt));
                    break;

                case DATATYPE_INT16:
                    RCP_INFO_ONLY("%d\n", rcp_option_get_i16(opt));
                    break;
                case DATATYPE_UINT16:
                    RCP_INFO_ONLY("%u\n", (uint16_t)rcp_option_get_i16(opt));
                    break;

                case DATATYPE_INT32:
                    RCP_INFO_ONLY("%d\n", rcp_option_get_i32(opt));
                    break;
                case DATATYPE_UINT32:
                    RCP_INFO_ONLY("%u\n", (uint32_t)rcp_option_get_i32(opt));
                    break;

                case DATATYPE_FLOAT32:
                    RCP_INFO_ONLY("%f\n", rcp_option_get_float(opt));
                    break;

                case DATATYPE_FLOAT64:
                    RCP_INFO_ONLY("%f\n", rcp_option_get_double(opt));
                    break;

                case DATATYPE_VECTOR2F32:
                    RCP_INFO_ONLY("%f,%f\n", rcp_option_get_vector2f_x(opt), rcp_option_get_vector2f_y(opt));
                    break;

                case DATATYPE_VECTOR2I32:
                case DATATYPE_VECTOR3F32:
                case DATATYPE_VECTOR3I32:
                case DATATYPE_VECTOR4F32:
                case DATATYPE_VECTOR4I32:
                    // TODO
                    break;

                case DATATYPE_STRING:
                    RCP_INFO_ONLY("%s\n", rcp_option_get_string(opt, LONG_STRING));
                    break;

                case DATATYPE_ENUM:
                    RCP_INFO_ONLY("%s\n", rcp_option_get_string(opt, TINY_STRING));
                    break;

                case DATATYPE_IPV4:
                {
                    uint32_t v = rcp_option_get_i32(opt);
                    RCP_INFO_ONLY("%d.%d.%d.%d\n", ((v >> 24) & 0xFF), ((v >> 16) & 0xFF), ((v >> 8) & 0xFF), ((v >> 0) & 0xFF));
                    break;
                }
                case DATATYPE_CUSTOMTYPE:
                {
                    RCP_INFO_ONLY("\n");
                    rcp_option_log(opt, NULL, false);
                    break;
                }

                default:
                    RCP_INFO_ONLY("\n");
                }


                break;
            case PARAMETER_OPTIONS_LABEL:
                RCP_INFO_ONLY("LABEL: ");
                rcp_langstr_log_chain(rcp_option_get_langstr(opt));
                break;

            case PARAMETER_OPTIONS_DESCRIPTION:
                RCP_INFO_ONLY("DESCRIPTION: ");
                rcp_langstr_log_chain(rcp_option_get_langstr(opt));
                break;

            case PARAMETER_OPTIONS_TAGS:
                RCP_INFO_ONLY("TAGS: %s\n", rcp_option_get_string(opt, TINY_STRING));
                break;

            case PARAMETER_OPTIONS_ORDER:
                RCP_INFO_ONLY("ORDER: %d\n", rcp_option_get_i32(opt));
                break;

            case PARAMETER_OPTIONS_PARENTID:
                RCP_INFO_ONLY("PARENT_ID: %d\n", rcp_option_get_i16(opt));
                break;

            case PARAMETER_OPTIONS_USERDATA:
            {
                char* d = NULL;
                size_t d_size = 0;
                rcp_option_get_data(opt, (void**)&d, &d_size);

                RCP_INFO_ONLY("USERDATA: ");

                if (d_size > 100)
                {
                    RCP_INFO_ONLY("(first 100 bytes): ");
                    d_size = 100;
                }

                if (d != NULL && d_size > 0)
                {
                    for (size_t i=0; i<d_size; i++)
                    {
                        RCP_INFO_ONLY("0x%02x ", *(d+i));
                    }
                }
                RCP_INFO_ONLY("\n");
                break;
            }

            case PARAMETER_OPTIONS_USERID:
                RCP_INFO_ONLY("USERID: %s\n", rcp_option_get_string(opt, TINY_STRING));
                break;

            default:
                RCP_INFO_ONLY("(not handled)\n");
                break;
            }

            opt = rcp_option_get_next(opt);
        }
        RCP_INFO("\n");
    }

    if (rcp_parameter_is_group(parameter))
    {
        rcp_group_parameter* gp = RCP_GROUP_PARAMETER(parameter);

        rcp_parameter_list* child = gp->children;

        if (child)
        {
            RCP_INFO("\tchildren: ");
            while (child)
            {
                RCP_INFO_ONLY("%d, ", rcp_parameter_get_id(child->parameter));
                child = child->next;
            }
            RCP_INFO_ONLY("\n");
        }

        RCP_INFO("\n");
    }
#endif
}



size_t rcp_parameter_get_size(rcp_parameter* parameter, bool all)
{
    if (parameter == NULL) return 0;

    // default size paramter-id(2) + terminator(1)
    size_t size = 3;

    // add up options
    rcp_option* opt = parameter->options;
    while (opt)
    {
        size += rcp_option_get_size(opt, all);
        opt = rcp_option_get_next(opt);
    }

    // TODO: add up removed options
//    opt = parameter->removed_options;
//    while (opt)
//    {
//        if (opt->flags & FLAG_PTR_DATA)
//        {
//            // how to deal with: string-values??
//            size += 1;
//        }
//        else
//        {
//            size += opt->data_size + 1;
//        }

//        opt = opt->next;
//    }

    size += rcp_typedefinition_get_size(parameter->typedefinition, all);

    return size;
}




size_t rcp_parameter_write(rcp_parameter* parameter, char* data, size_t size, bool all)
{
    if (parameter == NULL) return 0;
    if (data == NULL) return 0;

    if (size < 2)
    {
        RCP_PARAMETER_DEBUG("could not write id - buffer overflow\n");
        return 0;
    }

    // write data    
    size_t written = 0;

    // write parameter id
    RCP_PARAMETER_DEBUG("write parameter id: %d\n", parameter->id);

    _rcp_store16(data, (uint16_t)parameter->id);

    written += 2;

    if (written >= size) return 0;

    data += 2;


    size_t written_len = rcp_typedefinition_write(parameter->typedefinition, data, size - written, all);
    if (written_len == 0)
    {
        RCP_PARAMETER_DEBUG("error writing type definition\n");
        return 0;
    }

    written += written_len;

    if (written >= size)
    {
        RCP_PARAMETER_DEBUG("offset >= data_size! 1\n");
        return 0;
    }

    data += written_len;


    // write all parameter options
    rcp_option* opt = parameter->options;
    while (opt)
    {
        if (all || rcp_option_is_changed(opt))
        {
            written_len = rcp_option_write(opt, data, size - written, all);
            if (written_len == 0)
            {
                RCP_PARAMETER_DEBUG("error writing option: %d\n", rcp_option_get_prefix(opt));
                return 0;
            }

            if (written_len)
            {
                written += written_len;

                if (written >= size)
                {
                    RCP_PARAMETER_DEBUG("offset >= data_size! 2\n");
                    return 0;
                }

                data += written_len;
            }
        }

        opt = rcp_option_get_next(opt);
    }


    // TODO
    // write all removed parameter options!


    // write terminator
    memset(data, 0, 1);
    written += 1;

    return written;
}

size_t rcp_parameter_write_updatevalue(rcp_parameter* parameter, char* dst, size_t size)
{
    if (dst == NULL
            || parameter == NULL)
    {
        return 0;
    }

    if (size < 2)
    {
        RCP_PARAMETER_DEBUG("could not write id - buffer overflow\n");
        return 0;
    }

    // write data
    size_t written = 0;

    // write parameter id
    _rcp_store16(dst, (uint16_t)parameter->id);
    written += 2;
    // check
    if (written >= size) return 0;
    // move pointer
    dst += 2;


    // write mandatory typedefinition
    rcp_typedefinition* typedefinition = rcp_parameter_get_typedefinition(parameter);

    size_t written_len = rcp_typedefinition_write_mandatory(typedefinition, dst, size);
    if (written_len == 0) return 0;

    written += written_len;
    // check
    if (written >= size) return 0;
    // move pointer
    dst += written_len;

    if (rcp_parameter_is_value(parameter))
    {
        // write value
        written += rcp_option_write_value(RCP_VALUE_PARAMETER(parameter)->value_option, dst, size);
    }

    return written;
}


void rcp_parameter_all_options_changed(rcp_parameter* parameter)
{
    if (parameter == NULL) return;

    rcp_option* opt = parameter->options;
    while (opt)
    {
        rcp_option_set_changed(opt, true);
        opt = rcp_option_get_next(opt);
    }

    rcp_typedefinition_all_options_changed(parameter->typedefinition);
}

void rcp_parameter_all_options_unchanged(rcp_parameter* parameter)
{
    if (parameter == NULL) return;

    rcp_option* opt = parameter->options;
    while (opt)
    {
        rcp_option_set_changed(opt, false);
        opt = rcp_option_get_next(opt);
    }

    rcp_typedefinition_all_options_unchanged(parameter->typedefinition);
}


void rcp_parameter_set_manager(rcp_parameter* parameter, rcp_manager* manager)
{
    if (parameter == NULL) return;

    parameter->manager = manager;    
}

bool rcp_parameter_is_value(rcp_parameter* parameter)
{
    if (parameter != NULL)
    {
        rcp_datatype dt = RCP_TYPE_ID(parameter);

        return dt != DATATYPE_INVALID &&
                dt < DATATYPE_MAX_ &&
                dt != DATATYPE_GROUP &&
                dt != DATATYPE_BANG;
    }

    return  false;
}

bool rcp_parameter_is_group(rcp_parameter* parameter)
{
	return RCP_IS_GROUP(parameter);
}

bool rcp_parameter_is_number(rcp_parameter* parameter)
{
    return RCP_IS_TYPE(parameter, DATATYPE_INT8) ||
           RCP_IS_TYPE(parameter, DATATYPE_UINT8) ||
           RCP_IS_TYPE(parameter, DATATYPE_INT16) ||
           RCP_IS_TYPE(parameter, DATATYPE_UINT16) ||
           RCP_IS_TYPE(parameter, DATATYPE_INT32) ||
           RCP_IS_TYPE(parameter, DATATYPE_UINT32) ||
           RCP_IS_TYPE(parameter, DATATYPE_INT64) ||
           RCP_IS_TYPE(parameter, DATATYPE_UINT64) ||
           RCP_IS_TYPE(parameter, DATATYPE_FLOAT32) ||
           RCP_IS_TYPE(parameter, DATATYPE_FLOAT64) ||
           RCP_IS_TYPE(parameter, DATATYPE_VECTOR2F32) ||
           RCP_IS_TYPE(parameter, DATATYPE_VECTOR2I32) ||
           RCP_IS_TYPE(parameter, DATATYPE_VECTOR3F32) ||
           RCP_IS_TYPE(parameter, DATATYPE_VECTOR3I32) ||
           RCP_IS_TYPE(parameter, DATATYPE_VECTOR4F32) ||
           RCP_IS_TYPE(parameter, DATATYPE_VECTOR4I32);
}

bool rcp_parameter_only_value_changed(rcp_parameter* parameter)
{
	if (parameter == NULL) return false;
	if (!rcp_parameter_is_value(parameter) && !rcp_parameter_is_type(parameter, DATATYPE_BANG)) return false;
	if (rcp_typedefinition_changed(parameter->typedefinition)) return false;
	
	bool result = rcp_parameter_is_type(parameter, DATATYPE_BANG);

	rcp_value_parameter* vp = NULL;
    if (rcp_parameter_is_value(parameter))
    {
        vp = RCP_VALUE_PARAMETER(parameter);
    }
	
    rcp_option* opt = RCP_PARAMETER(parameter)->options;
    while (opt)
	{
        if (vp &&
                opt == vp->value_option)
		{
			result = rcp_option_is_changed(opt);
		}
		else
		{
			if (rcp_option_is_changed(opt))
			{
				// something else changed
				return false;
			}
		}
        opt = rcp_option_get_next(opt);
    }
	
	return result;
}
