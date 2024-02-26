/*
********************************************************************
* rabbitcontrol - a protocol and data-format for remote control.
*
* https://rabbitcontrol.cc
* https://github.com/rabbitcontrol/rcp-c
*
* This file is part of rabbitcontrol for c.
*
* Written by Ingo Randolf, 2021 - 2024
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
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
#define RCP_C_VERSION "1.0.2"
#define RCP_C_VERSION_MAJOR 1
#define RCP_C_VERSION_MINOR 0
#define RCP_C_VERSION_PATCH 2

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

#ifdef __cplusplus
} // extern "C"
#endif

#endif
