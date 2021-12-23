/*
********************************************************************
* rabbitcontrol - a protocol and data-format for remote control.
*
* https://rabbitcontrol.cc
* https://github.com/rabbitcontrol/rcp-c
*
* This file is part of rabbitcontrol for c.
*
* Written by Ingo Randolf, 2021
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*********************************************************************
*/

#ifndef RCP_SEMVER_H
#define RCP_SEMVER_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef struct rcp_semver
{
    int major;
    int minor;
    int patch;
} rcp_semver;

bool rcp_semver_parse(const char* semver, rcp_semver* outSemver);
void rcp_semver_log(rcp_semver* semver);

#ifdef __cplusplus
} // extern "C"
#endif


#endif // RCP_SEMVER_H
