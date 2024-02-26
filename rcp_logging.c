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
