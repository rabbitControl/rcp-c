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

#include "rcp_semver.h"

#include <string.h>
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

    char* input = NULL;
    char* tofree = NULL;
    char* token = NULL;

    // init
    semver->major = 0;
    semver->minor = 0;
    semver->patch = 0;

    tofree = input = strdup(str);

    if (tofree)
    {
        token = strchr(input, '.');
        while (token)
        {
            // write terminator
            token[0] = 0;

            if (major == NULL)
            {
                major = input;
                input = token + 1;

                token = strchr(input, '.');
            }
            else if (minor == NULL)
            {
                minor = input;
                patch = token + 1;

                // break on - or +
                token = strchr(patch, '-');
                if (token == NULL) token = strchr(patch, '+');
            }
            else
            {
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
    }


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
