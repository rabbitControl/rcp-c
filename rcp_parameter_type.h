/*
********************************************************************
* rabbitcontrol - a protocol and data-format for remote control.
*
* https://rabbitcontrol.cc
* https://github.com/rabbitcontrol/rcp-c
*
* This file is part of rabbitcontrol for c.
*
* Written by Ingo Randolf, 2021 - 2022
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*********************************************************************
*/

#ifndef RCP_PARAMETER_TYPE_H
#define RCP_PARAMETER_TYPE_H

#ifdef __cplusplus
extern "C"{
#endif

// types
typedef struct rcp_parameter rcp_parameter;
typedef struct rcp_group_parameter rcp_group_parameter;
typedef struct rcp_value_parameter rcp_value_parameter;
typedef struct rcp_bang_parameter rcp_bang_parameter;

// parameter list
typedef struct rcp_parameter_list rcp_parameter_list;

struct rcp_parameter_list
{
    rcp_parameter_list* next;
    rcp_parameter* parameter;
};


#ifdef __cplusplus
} // extern "C"
#endif

#endif // RCP_PARAMETER_TYPE_H
