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

#ifndef RCP_OPTION_H
#define RCP_OPTION_H


#ifdef __cplusplus
extern "C"{
#endif

#include "rcp.h"
#include "rcp_option_type.h"
#include "rcp_parameter_type.h"
#include "rcp_langstr.h"
#include "rcp_infodata.h"
#include "rcp_stringlist.h"

//#define RCP_OPTION_DEBUG_LOG
//#define RCP_OPTION_MALLOC_DEBUG_LOG


// create / free
rcp_option* rcp_option_create(char prefix);
rcp_option* rcp_option_get_create(rcp_option** options, char prefix);
rcp_option* rcp_option_get(rcp_option* options, char prefix);
rcp_option* rcp_option_add_or_update(rcp_option** options, rcp_option* new_option);
void rcp_option_free(rcp_option* opt);
void rcp_option_free_chain(rcp_option* opt);
void rcp_option_free_data(rcp_option* opt);

// get next option
rcp_option* rcp_option_get_next(rcp_option* opt);

// changed flag
bool rcp_option_is_changed(rcp_option* opt);
void rcp_option_set_changed(rcp_option* opt, bool state);

// prefix
char rcp_option_get_prefix(rcp_option* opt);

// setter / getter
bool rcp_option_set_bool(rcp_option* opt, bool value);
bool rcp_option_set_i8(rcp_option* opt, int8_t value);
bool rcp_option_set_i16(rcp_option* opt, int16_t value);
bool rcp_option_set_i32(rcp_option* opt, int32_t value);
bool rcp_option_set_i64(rcp_option* opt, int64_t value);
bool rcp_option_set_f32(rcp_option* opt, float value);
bool rcp_option_set_f64(rcp_option* opt, double value);
bool rcp_option_set_data(rcp_option* opt, void* data, size_t size, bool sizeprefixed); // no transfer, external data
bool rcp_option_copy_data(rcp_option* opt, void* data, size_t size, bool sizeprefixed); // copy data, takes ownership
// todo: rcp_option_move_data?

// vector
bool rcp_option_set_vector2f(rcp_option* opt, float x, float y);


bool rcp_option_get_bool(rcp_option* opt);
int8_t rcp_option_get_i8(rcp_option* opt);
int16_t rcp_option_get_i16(rcp_option* opt);
int32_t rcp_option_get_i32(rcp_option* opt);
int64_t rcp_option_get_i64(rcp_option* opt);
uint64_t rcp_option_get_u64(rcp_option* opt);
float rcp_option_get_float(rcp_option* opt);
double rcp_option_get_double(rcp_option* opt);
void rcp_option_get_data(rcp_option* opt, void** out_data, size_t* out_size); // no transfer

// vector
float rcp_option_get_vector2f_x(rcp_option* opt);
float rcp_option_get_vector2f_y(rcp_option* opt);

// string
bool rcp_option_move_string(rcp_option* opt, char* data, rcp_string_types type); // full transfer
bool rcp_option_copy_string(rcp_option* opt, const char* data, rcp_string_types type); // copies string, full transfer
const char* rcp_option_get_string(rcp_option* opt, rcp_string_types type); // no transfer

// language string
bool rcp_option_move_langstr(rcp_option* opt, rcp_language_str* data); // full transfer
bool rcp_option_copy_any_language(rcp_option* opt, const char* text, rcp_string_types type); // copies text, takes ownership
rcp_language_str* rcp_option_get_langstr(rcp_option* opt); // no transfer
const char* rcp_option_get_any_language(rcp_option* opt); // no transfer

// rcp data types
void rcp_option_set_infodata(rcp_option* opt, rcp_infodata* data); // no transfer
rcp_infodata* rcp_option_get_infodata(rcp_option* opt); // no transfer
void rcp_option_put_infodata(rcp_option* opt, rcp_infodata* data); // full transfer
rcp_infodata* rcp_option_take_infodata(rcp_option* opt); // full transfer

void rcp_option_set_parameter(rcp_option* opt, rcp_parameter* data); // no transfer
rcp_parameter* rcp_option_get_parameter(rcp_option* opt); // no transfer
void rcp_option_put_parameter(rcp_option* opt, rcp_parameter* data); // full transfer
rcp_parameter* rcp_option_take_parameter(rcp_option* opt); // full transfer

void rcp_option_put_stringlist(rcp_option* opt, rcp_stringlist* list); // full transfer
rcp_stringlist* rcp_option_get_stringlist(rcp_option* opt); // no transfer

// serializing
// get size when serialized
size_t rcp_option_get_size(rcp_option* opt, bool force);
// write option
size_t rcp_option_write(rcp_option* opt, char* dst, size_t size, bool force);
// write value into data, return size
size_t rcp_option_write_value(rcp_option* opt, char* dst, size_t size);
// get size of data
size_t rcp_option_get_data_size(rcp_option* opt);


void rcp_option_log(rcp_option* opt, const char* prefix_str, bool isunsigned);

#ifdef RCP_OPTION_USE_EXTERNAL_GET_SET
// external fetch
void rcp_option_set_external_cb(rcp_option *opt, void (*getCb)(void** out_data, size_t* out_size), bool (*setCb)(void* data, size_t size));
#endif


#ifdef __cplusplus
} // extern "C"
#endif

#endif
