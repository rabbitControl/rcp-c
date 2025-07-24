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

#include "rcp_logging.h"

#include <stdio.h>
#include <stdarg.h>

#ifdef RCP_LOG_INFO
__attribute__((format(printf, 1, 2)))
void rcp_info_only(const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    vfprintf(stdout, format, argptr);
    va_end(argptr);
    fflush(stdout);
}

__attribute__((format(printf, 1, 2)))
void rcp_info(const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    fprintf(stdout, "INFO - ");
    vfprintf(stdout, format, argptr);
    va_end(argptr);
    fflush(stdout);
}
#endif


#ifdef RCP_LOG_DEBUG
__attribute__((format(printf, 1, 2)))
void rcp_debug(const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    fprintf(stdout, "DEBUG - ");
    vfprintf(stdout, format, argptr);
    va_end(argptr);
    fflush(stdout);
}
#endif

#ifdef RCP_LOG_ERROR
__attribute__((format(printf, 1, 2)))
void rcp_error(const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    fprintf(stderr, "ERROR - ");
    vfprintf(stderr, format, argptr);
    va_end(argptr);
    fflush(stderr);
}
#endif
