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

#ifndef RCP_LOGGING_H
#define RCP_LOGGING_H

#ifdef __cplusplus
extern "C"{
#endif


//#define RCP_LOG_INFO

// debug logging
// per module logging set in implementatin files
// RCP_LOG_DEBUG needs to be turned on for module logging
//#define RCP_LOG_DEBUG
// or debug_log all
//#define RCP_ALL_DEBUG

//#define RCP_LOG_ERROR


#ifdef RCP_LOG_INFO

void rcp_info(const char* format, ...);
void rcp_info_only(const char* format, ...);
#define RCP_INFO(...) rcp_info(__VA_ARGS__);
#define RCP_INFO_ONLY(...) rcp_info_only(__VA_ARGS__);
#else
#define RCP_INFO(...)
#define RCP_INFO_ONLY(...)

#endif


#ifdef RCP_LOG_DEBUG

void rcp_debug(const char* format, ...);
#define RCP_DEBUG(...) rcp_debug(__VA_ARGS__);
#else
#define RCP_DEBUG(...)

#endif

#ifdef RCP_LOG_ERROR

void rcp_error(const char* format, ...);
#define RCP_ERROR(...) rcp_error(__VA_ARGS__);
#else
#define RCP_ERROR(...)

#endif


#ifdef __cplusplus
}
#endif

#endif
