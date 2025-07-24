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

#ifndef RCP_PARAMETER_H
#define RCP_PARAMETER_H

#ifdef __cplusplus
extern "C"{
#endif

#include "rcp.h"
#include "rcp_parameter_type.h"
#include "rcp_option_type.h"
#include "rcp_manager_type.h"
#include "rcp_typedefinition_type.h"

//#define RCP_PARAMETER_DEBUG_LOG
//#define RCP_PARAMETER_MALLOC_DEBUG_LOG

#define RCP_TYPE_ID(x) (x != NULL ? rcp_typedefinition_get_type_id(rcp_parameter_get_typedefinition(x)) : DATATYPE_INVALID)
#define RCP_PARAMETER(x) ((rcp_parameter*)x)
#define RCP_VALUE_PARAMETER(x) ((rcp_value_parameter*)x)
#define RCP_GROUP_PARAMETER(x) ((rcp_group_parameter*)x)
#define RCP_BANG_PARAMETER(x) ((rcp_bang_parameter*)x)
#define RCP_IS_GROUP(x) (RCP_TYPE_ID(x) == DATATYPE_GROUP)
#define RCP_IS_TYPE(x, y) (RCP_TYPE_ID(RCP_PARAMETER(x)) == y)


// create parameter
rcp_group_parameter* rcp_group_parameter_create(int16_t id);
rcp_value_parameter* rcp_bool_parameter_create(int16_t id);

rcp_value_parameter* rcp_i8_parameter_create(int16_t id);
rcp_value_parameter* rcp_u8_parameter_create(int16_t id);
rcp_value_parameter* rcp_i16_parameter_create(int16_t id);
rcp_value_parameter* rcp_u16_parameter_create(int16_t id);
rcp_value_parameter* rcp_i32_parameter_create(int16_t id);
rcp_value_parameter* rcp_u32_parameter_create(int16_t id);
rcp_value_parameter* rcp_f32_parameter_create(int16_t id);

rcp_value_parameter* rcp_vector2f32_parameter_create(int16_t id);

rcp_value_parameter* rcp_string_parameter_create(int16_t id);
rcp_value_parameter* rcp_enum_parameter_create(int16_t id);
rcp_value_parameter* rcp_ipv4_parameter_create(int16_t id);

rcp_value_parameter* rcp_custom_parameter_create(int16_t id, uint32_t size);

rcp_bang_parameter* rcp_bang_parameter_create(int16_t id);





// free
void rcp_parameter_free(rcp_parameter* param);

// copy
void rcp_parameter_copy_from(rcp_parameter* dst, rcp_parameter* src);

// manager
void rcp_parameter_set_manager(rcp_parameter* parameter, rcp_manager* manager);

// getter id, typedefinition
int16_t rcp_parameter_get_id(rcp_parameter* parameter);
rcp_typedefinition* rcp_parameter_get_typedefinition(rcp_parameter* parameter);


// options
bool rcp_parameter_has_options(rcp_parameter* parameter);
void rcp_parameter_all_options_changed(rcp_parameter* parameter);
void rcp_parameter_all_options_unchanged(rcp_parameter* parameter);
bool rcp_parameter_has_option(rcp_parameter* parameter, rcp_parameter_options option);

// parameter types
bool rcp_parameter_is_type(rcp_parameter* parameter, rcp_datatype type);
bool rcp_parameter_is_value(rcp_parameter* parameter);
bool rcp_parameter_is_group(rcp_parameter* parameter);
bool rcp_parameter_is_number(rcp_parameter* parameter);

//-------------------
// value parameter
bool rcp_parameter_only_value_changed(rcp_parameter* parameter);

// bool
void rcp_parameter_set_value_bool(rcp_value_parameter* parameter, bool value);
bool rcp_parameter_get_value_bool(rcp_value_parameter* parameter);

// number
void rcp_parameter_set_number_scale(rcp_value_parameter* parameter, rcp_number_scale scale);
void rcp_parameter_set_number_unit(rcp_value_parameter* parameter, const char* unit);

// int8
void rcp_parameter_set_value_int8(rcp_value_parameter* parameter, int8_t value);
void rcp_parameter_set_default_int8(rcp_value_parameter* parameter, int8_t value);
void rcp_parameter_set_min_int8(rcp_value_parameter* parameter, int8_t value);
void rcp_parameter_set_max_int8(rcp_value_parameter* parameter, int8_t value);
void rcp_parameter_set_multipleof_int8(rcp_value_parameter* parameter, int8_t value);
int8_t rcp_parameter_get_value_int8(rcp_value_parameter* parameter);

// int16
void rcp_parameter_set_value_int16(rcp_value_parameter* parameter, int16_t value);
void rcp_parameter_set_default_int16(rcp_value_parameter* parameter, int16_t value);
void rcp_parameter_set_min_int16(rcp_value_parameter* parameter, int16_t value);
void rcp_parameter_set_max_int16(rcp_value_parameter* parameter, int16_t value);
void rcp_parameter_set_multipleof_int16(rcp_value_parameter* parameter, int16_t value);
int16_t rcp_parameter_get_value_int16(rcp_value_parameter* parameter);

// int32
void rcp_parameter_set_value_int32(rcp_value_parameter* parameter, int32_t value);
void rcp_parameter_set_default_int32(rcp_value_parameter* parameter, int32_t value);
void rcp_parameter_set_min_int32(rcp_value_parameter* parameter, int32_t value);
void rcp_parameter_set_max_int32(rcp_value_parameter* parameter, int32_t value);
void rcp_parameter_set_multipleof_int32(rcp_value_parameter* parameter, int32_t value);
int32_t rcp_parameter_get_value_int32(rcp_value_parameter* parameter);
int32_t rcp_parameter_get_default_int32(rcp_value_parameter* parameter);
int32_t rcp_parameter_get_min_int32(rcp_value_parameter* parameter);
int32_t rcp_parameter_get_max_int32(rcp_value_parameter* parameter);
int32_t rcp_parameter_get_multipleof_int32(rcp_value_parameter* parameter);

// float
void rcp_parameter_set_value_float(rcp_value_parameter* parameter, float value);
void rcp_parameter_set_default_float(rcp_value_parameter* parameter, float value);
void rcp_parameter_set_min_float(rcp_value_parameter* parameter, float value);
void rcp_parameter_set_max_float(rcp_value_parameter* parameter, float value);
void rcp_parameter_set_multipleof_float(rcp_value_parameter* parameter, float value);
float rcp_parameter_get_value_float(rcp_value_parameter* parameter);
float rcp_parameter_get_default_float(rcp_value_parameter* parameter);
float rcp_parameter_get_min_float(rcp_value_parameter* parameter);
float rcp_parameter_get_max_float(rcp_value_parameter* parameter);
float rcp_parameter_get_multipleof_float(rcp_value_parameter* parameter);


// vector
// vector2 float
void rcp_parameter_set_value_vector2f32(rcp_value_parameter* parameter, float x, float y);
void rcp_parameter_set_default_vector2f32(rcp_value_parameter* parameter, float x, float y);
void rcp_parameter_set_min_vector2f32(rcp_value_parameter* parameter, float x, float y);
void rcp_parameter_set_max_vector2f32(rcp_value_parameter* parameter, float x, float y);
void rcp_parameter_set_multipleof_vector2f32(rcp_value_parameter* parameter, float x, float y);
float rcp_parameter_get_value_vector2f32_x(rcp_value_parameter* parameter);
float rcp_parameter_get_value_vector2f32_y(rcp_value_parameter* parameter);


// string
void rcp_parameter_set_value_string(rcp_value_parameter* parameter, const char* value);
const char* rcp_parameter_get_value_string(rcp_value_parameter* parameter);


// unsigned
#define rcp_parameter_set_value_uint8(param, value) (rcp_parameter_set_value_int8(param, (int8_t)value))
#define rcp_parameter_get_value_uint8(param) ((uint8_t)rcp_parameter_get_value_int8(param))
#define rcp_parameter_set_default_uint8(param, value) (rcp_parameter_set_default_int8(param, (int8_t)value))
#define rcp_parameter_set_min_uint8(param, value) (rcp_parameter_set_min_int8(param, (int8_t)value))
#define rcp_parameter_set_max_uint8(param, value) (rcp_parameter_set_max_int8(param, (int8_t)value))
#define rcp_parameter_set_multipleof_uint8(param, value) (rcp_parameter_set_multipleof_int8(param, (int8_t)value))

#define rcp_parameter_set_value_uint16(param, value) (rcp_parameter_set_value_int16(param, (int16_t)value))
#define rcp_parameter_get_value_uint16(param) ((uint16_t)rcp_parameter_get_value_int16(param))
#define rcp_parameter_set_default_uint16(param, value) (rcp_parameter_set_default_int16(param, (int16_t)value))
#define rcp_parameter_set_min_uint16(param, value) (rcp_parameter_set_min_int16(param, (int16_t)value))
#define rcp_parameter_set_max_uint16(param, value) (rcp_parameter_set_max_int16(param, (int16_t)value))
#define rcp_parameter_set_multipleof_uint16(param, value) (rcp_parameter_set_multipleof_int16(param, (int16_t)value))

#define rcp_parameter_set_value_uint32(param, value) (rcp_parameter_set_value_int32(param, (int32_t)value))
#define rcp_parameter_get_value_uint32(param) ((uint32_t)rcp_parameter_get_value_int32(param))
#define rcp_parameter_set_default_uint32(param, value) (rcp_parameter_set_default_int32(param, (int32_t)value))
#define rcp_parameter_set_min_uint32(param, value) (rcp_parameter_set_min_int32(param, (int32_t)value))
#define rcp_parameter_set_max_uint32(param, value) (rcp_parameter_set_max_int32(param, (int32_t)value))
#define rcp_parameter_set_multipleof_uint32(param, value) (rcp_parameter_set_multipleof_int32(param, (int32_t)value))


//-------------------
// bang parameter
void rcp_bang_parameter_set_bang_cb(rcp_bang_parameter* parameter, void (*fn)(rcp_bang_parameter* parameter, void* user));
void rcp_bang_parameter_call_bang_cb(rcp_bang_parameter* parameter);
void rcp_bang_parameter_set_dirty(rcp_bang_parameter* parameter);

//-------------------
// group parameter
rcp_parameter_list* rcp_group_get_children(rcp_group_parameter* group);

//-------------------
// enum parameter
void rcp_parameter_set_value_enum(rcp_value_parameter* parameter, const char* value);
void rcp_parameter_set_default_enum(rcp_value_parameter* parameter, const char* value);
void rcp_parameter_set_multiselect_enum(rcp_value_parameter* parameter, bool value);
void rcp_parameter_set_entries_enum(rcp_value_parameter* parameter, int count, ...); // options as const char*
const char* rcp_parameter_get_value_enum(rcp_value_parameter* parameter);
const char* rcp_parameter_get_default_enum(rcp_value_parameter* parameter);
bool rcp_parameter_get_multiselect_enum(rcp_value_parameter* parameter);


//-------------------
// ipv4
void rcp_parameter_set_value_ipv4(rcp_value_parameter* parameter, uint32_t value);
void rcp_parameter_set_default_ipv4(rcp_value_parameter* parameter, uint32_t value);
uint32_t rcp_parameter_get_value_ipv4(rcp_value_parameter* parameter);
uint32_t rcp_parameter_get_default_ipv4(rcp_value_parameter* parameter);


//-------------------
// custom
void rcp_parameter_copy_value_data(rcp_value_parameter* parameter, const char* data, size_t size); // copy
void rcp_parameter_set_default_data(rcp_value_parameter* parameter, const char* data, size_t size); // no copy
void rcp_parameter_set_uuid(rcp_value_parameter* parameter, const char* uuid, size_t size); // no copy
void rcp_parameter_set_config(rcp_value_parameter* parameter, const char* data, size_t size); // no copy

void rcp_parameter_get_value_data(rcp_value_parameter* parameter, const char** out_data, size_t* out_size); // no copy
void rcp_parameter_get_default_data(rcp_value_parameter* parameter, const char** out_data, size_t* out_size); // no copy
void rcp_parameter_get_uuid(rcp_value_parameter* parameter, const char** out_uuid, size_t* out_size); // no copy
void rcp_parameter_get_config(rcp_value_parameter* parameter, const char** out_data, size_t* out_size); // no copy

//-------------------
// parameter options

// label
void rcp_parameter_set_label(rcp_parameter* parameter, const char* label);
const char* rcp_parameter_get_label(rcp_parameter* parameter);

// description
void rcp_parameter_set_description(rcp_parameter* parameter, const char* label);
const char* rcp_parameter_get_description(rcp_parameter* parameter);

// tags
void rcp_parameter_set_tags(rcp_parameter* parameter, const char* tags);
const char* rcp_parameter_get_tags(rcp_parameter* parameter);

// order
void rcp_parameter_set_order(rcp_parameter* parameter, int32_t order);
int32_t rcp_parameter_get_order(rcp_parameter* parameter);

// readonly
void rcp_parameter_set_readonly(rcp_parameter* parameter, bool ro);
bool rcp_parameter_get_readonly(rcp_parameter* parameter);

// parent
void rcp_parameter_set_parent(rcp_parameter* parameter, rcp_group_parameter* group);
rcp_group_parameter* rcp_parameter_get_parent(rcp_parameter* parameter);
void rcp_parameter_resolve_parent(rcp_parameter* parameter);

// userdata
void rcp_parameter_set_userdata(rcp_parameter* parameter, void* data, size_t size);
void rcp_parameter_copy_userdata(rcp_parameter* parameter, void* data, size_t size);
void rcp_parameter_get_userdata(rcp_parameter* parameter, void** out_data, size_t* out_size); // no copy

// userid
void rcp_parameter_set_userid(rcp_parameter* parameter, const char* userid);
const char* rcp_parameter_get_userid(rcp_parameter* parameter);

// parsing
char* rcp_parameter_parse_value(rcp_parameter* parameter, char* data, size_t* size);
char* rcp_parameter_parse_options(rcp_parameter* parameter, char* data, size_t* size);

// size and writing
size_t rcp_parameter_get_size(rcp_parameter* parameter, bool all);
size_t rcp_parameter_get_value_size(rcp_value_parameter* parameter);

size_t rcp_parameter_write(rcp_parameter* parameter, char* dst, size_t size, bool all);
size_t rcp_parameter_write_updatevalue(rcp_parameter* parameter, char* dst, size_t size);

// callbacks
void rcp_parameter_set_user(rcp_parameter* parameter, void* user);
void rcp_parameter_set_updated_cb(rcp_parameter* parameter, void (*cb)(rcp_parameter*, void*));
void rcp_parameter_set_value_updated_cb(rcp_value_parameter* parameter, void (*cb)(rcp_value_parameter*, void*));

#ifdef RCP_OPTION_USE_EXTERNAL_GET_SET
// external data
void rcp_parameter_set_external_value_cb(rcp_value_parameter* parameter, void (*getCb)(void** out_data, size_t* out_size), bool (*setCb)(void* data, size_t size));
#endif

// logging
void rcp_parameter_log(rcp_parameter* parameter);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
