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

#ifndef RCP_H
#define RCP_H

/*
TODO:
- functions to remove parameter-options and typedefintion-options
*/

#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "rcp_types.h"

// version of rcp protocol implemented
#define RCP_VERSION "0.1.0"
#define RCP_VERSION_MAJOR 0
#define RCP_VERSION_MINOR 1
#define RCP_VERSION_PATCH 0

// version of c implementation
#define RCP_C_VERSION "1.1.0"
#define RCP_C_VERSION_MAJOR 1
#define RCP_C_VERSION_MINOR 1
#define RCP_C_VERSION_PATCH 0

#define RCP_SETFLAG(x, f) (x |= (1 << f))
#define RCP_CLEARFLAG(x, f) (x &= ~(1 << f))
#define RCP_CHECKFLAG(x, f) (x & (1 << f))


// rcp compile switches

// enable external getter and setter
//#define RCP_OPTION_USE_EXTERNAL_GET_SET


typedef enum rcp_datatype_t rcp_datatype;
typedef enum rcp_number_options_t rcp_number_options;
typedef enum rcp_number_scale_t rcp_number_scale;
typedef enum rcp_string_options_t rcp_string_options;
typedef enum rcp_parameter_options_t rcp_parameter_options;
typedef enum rcp_command_t rcp_packet_command;
typedef enum rcp_packet_options_t rcp_packet_options;
typedef enum rcp_infodata_options_t rcp_infodata_options;
typedef enum rcp_string_types_t rcp_string_types;
typedef enum rcp_enum_options_t rcp_enum_options;
typedef enum rcp_customtype_options_t rcp_customtype_options;

#ifdef __cplusplus
} // extern "C"
#endif

#endif
