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

#include "rcp_infodata.h"

#include <string.h>

#include "rcp_memory.h"
#include "rcp_option.h"
#include "rcp_string.h"
#include "rcp_parser.h"
#include "rcp_logging.h"

struct rcp_infodata
{
    // mandatory
    char* version; // 0-terminated c-string

    // optional
    rcp_option* applicationId;
};

rcp_infodata* rcp_infodata_create(const char* version, const char* applicationId)
{
    rcp_infodata* info_data = RCP_CALLOC(1, sizeof(rcp_infodata));

    if (info_data != NULL)
    {
        RCP_DEBUG("*** infodata: %p\n", info_data);

        if (version)
        {
            size_t str_len = strlen(version);

            if (str_len > 0)
            {
                info_data->version = RCP_CALLOC(1, str_len + 1);

                if (info_data->version)
                {
                    RCP_DEBUG("*** infodata version: %p\n", info_data->version);
                    strncpy(info_data->version, version, str_len);
                }
            }
        }

        if (applicationId)
        {
            info_data->applicationId = rcp_option_create(INFODATA_OPTIONS_APPLICATIONID);
            rcp_option_copy_string(info_data->applicationId, applicationId, TINY_STRING);
        }
    }

    return info_data;
}

void rcp_infodata_log(rcp_infodata* data)
{
#ifdef RCP_LOG_INFO
    RCP_INFO("infodata: %p\n", data);

    if (data)
    {
        RCP_INFO("version: %s\n", data->version);
        RCP_INFO("version appid: %s\n", rcp_option_get_string(data->applicationId, TINY_STRING));
    }
#endif
}

void rcp_infodata_free(rcp_infodata* data)
{
    RCP_DEBUG("||| free infodata: %p\n", data);

    if (data)
    {
        if (data->version)
        {
            RCP_DEBUG("+++ infodata version: %p\n", data->version);
            RCP_FREE(data->version);
        }

        if (data->applicationId)
        {
            rcp_option_free(data->applicationId);
        }

        RCP_DEBUG("+++ infodata: %p\n", data);
        RCP_FREE(data);
    }
}

size_t rcp_infodata_get_size(rcp_infodata* data)
{
    if (data == NULL) return 0;

    // min size for mandatory tiny string
    size_t size = 2; // tiny string prefix + terminator

    if (data->version)
    {
        size += strlen(data->version);
    }

    if (data->applicationId)
    {
        size += rcp_option_get_size(data->applicationId, true);
    }

    return size;
}

size_t rcp_infodata_write(rcp_infodata* infodata, char* dst, size_t size)
{
    if (infodata == NULL)
    {
        return 0;
    }

    size_t written = 0;

    // write mandatory version
    size_t str_len = rcp_write_tiny_string(dst, size, infodata->version);

    written += str_len;

    if (written >= size) return 0;

    dst += str_len;

    if (infodata->applicationId)
    {
        size_t written_len = rcp_option_write(infodata->applicationId, dst, size - written, true);
        if (written_len == 0)
        {
            // something went wrong
            return 0;
        }

        written += written_len;

        if (written >= size)
        {
            return 0;
        }

        dst += written_len;
    }

    // write terminator
    *dst = RCP_TERMINATOR;
    written += 1;

    return written;
}

const char* rcp_infodata_get_version(rcp_infodata* data)
{
    return data->version;
}

const char* rcp_infodata_get_application_id(rcp_infodata* data)
{
    return rcp_option_get_string(data->applicationId, TINY_STRING);
}


rcp_infodata* rcp_infodata_parse(const char** data, size_t* size)
{
    // get tinystring<
    char *version = NULL;
    char *appid = NULL;
    uint8_t version_len = 0;
    uint8_t appid_len = 0;

    const char* r_data = rcp_read_tiny_string(*data, size, &version, &version_len);
    if (r_data == NULL) return NULL;

    // return data ok
    *data = r_data;

    // parse options
    // we only expect one option

    uint8_t option_prefix = 0;
    r_data = rcp_read_u8(*data, size, &option_prefix);
    if (r_data == NULL)
    {
        if (version != NULL)
        {
            RCP_DEBUG("+++ version string: %p\n", version);
            RCP_FREE(version);
        }
        return NULL;
    }

    *data = r_data;

    if ((rcp_infodata_options)option_prefix == INFODATA_OPTIONS_APPLICATIONID)
    {
        r_data = rcp_read_tiny_string(*data, size, &appid, &appid_len);
        if (r_data == NULL)
        {
            if (version != NULL)
            {
                RCP_DEBUG("+++ version string: %p\n", version);
                RCP_FREE(version);
            }
            return NULL;
        }

        *data = r_data;

        // expect terminator for end of infodata

        r_data = rcp_read_u8(*data, size, &option_prefix);
        if (r_data == NULL)
        {
            RCP_ERROR("infodata: error reading terminator\n");

            if (version != NULL)
            {
                RCP_DEBUG("+++ version string: %p\n", version);
                RCP_FREE(version);
            }

            if (appid != NULL)
            {
                RCP_DEBUG("+++ appid string: %p\n", appid);
                RCP_FREE(appid);
            }

            return NULL;
        }

        *data = r_data;
    }


    if (option_prefix == RCP_TERMINATOR)
    {
        // ok - end of infodata

        if (version != NULL)
        {
            rcp_infodata * info_data = rcp_infodata_create(version, appid);

            // cleanup

            RCP_DEBUG("+++ version string: %p\n", version);
            RCP_FREE(version);

            if (appid != NULL)
            {
                RCP_DEBUG("+++ appid string: %p\n", appid);
                RCP_FREE(appid);
            }

            return info_data;
        }

        if (appid != NULL)
        {
            RCP_DEBUG("+++ appid string: %p\n", appid);
            RCP_FREE(appid);
        }
    }

    // sane packets do not come here
    return NULL;
}
