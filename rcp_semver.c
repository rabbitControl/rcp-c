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

#include "rcp_semver.h"

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "rcp_logging.h"
#include "rcp_memory.h"

static bool isValidAsciiNumber(char* str)
{
    if (str == NULL) return false;

    for (size_t i=0; i<strlen(str); i++)
    {
        if (str[i] < 48 || str[i] > 57) return false;
    }

    return true;
}

bool rcp_semver_parse(const char* str, rcp_semver* semver)
{
    // semver: major.minor.patch +|- additional_info

    bool ret = false;

    if (str == NULL) return ret;
    if (semver == NULL) return ret;

    char* major = NULL;
    char* minor = NULL;
    char* patch = NULL;
    char* rest = NULL;
    char* tofree = NULL;
    char* input = NULL;
    char* token = NULL;

    // init
    semver->major = 0;
    semver->minor = 0;
    semver->patch = 0;

    tofree = input = strdup(str);

    token = strtok_r(input, ".", &rest);

    while (token)
    {
        if (major == NULL)
        {
            major = token;
            token = strtok_r(NULL, ".", &rest);
        }
        else if (minor == NULL)
        {
            minor = token;
            token = strtok_r(NULL, ".", &rest);
        }
        else if (patch == NULL)
        {
            patch = token;
            token = strtok_r(patch, "-", &rest);
        }
        else {
            token = strtok_r(patch, "+", &rest);
            break;
        }
    }

    if (isValidAsciiNumber(major) &&
            isValidAsciiNumber(minor) &&
            isValidAsciiNumber(patch))
    {
        semver->major = strtol(major, NULL, 10);
        semver->minor = strtol(minor, NULL, 10);
        semver->patch = strtol(patch, NULL, 10);

        ret = true;
    }
    else
    {
        RCP_DEBUG("invalid data - could not parse semver");
    }

    free(tofree);

    return ret;
}

void rcp_semver_log(rcp_semver* semver)
{
#ifdef RCP_LOG_INFO
    if (semver)
    {
        RCP_INFO("rcp_semver: %d.%d.%d\n", semver->major, semver->minor, semver->patch);
    }
#endif
}
