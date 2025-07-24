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

#ifndef RCP_LOGGING_H
#define RCP_LOGGING_H

#ifdef __cplusplus
extern "C"{
#endif


//#define RCP_LOG_INFO

// debug logging
// per module logging set in implementation files
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
