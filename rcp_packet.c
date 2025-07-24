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

#include "rcp_packet.h"

#include <string.h>

#include "rcp_memory.h"
#include "rcp_logging.h"
#include "rcp_parser.h"
#include "rcp_endian.h"
#include "rcp_option.h"
#include "rcp_parameter.h"
#include "rcp_typedefinition.h"


#if defined(RCP_PACKET_DEBUG_LOG) || defined(RCP_ALL_DEBUG)
#define RCP_PACKET_DEBUG(...) RCP_DEBUG(__VA_ARGS__)
#else
#define RCP_PACKET_DEBUG(...)
#endif

#if defined(RCP_PACKET_MALLOC_DEBUG_LOG) || defined(RCP_ALL_DEBUG)
#define RCP_PACKET_MALLOC_DEBUG(...) RCP_DEBUG(__VA_ARGS__)
#else
#define RCP_PACKET_MALLOC_DEBUG(...)
#endif


struct rcp_packet
{
    // mandatory
    rcp_packet_command command;

    // options
    rcp_option* options;
};

rcp_packet* rcp_packet_create(rcp_packet_command command)
{
    rcp_packet* packet = RCP_CALLOC(1, sizeof(rcp_packet));

    if (packet != NULL)
    {
        RCP_PACKET_MALLOC_DEBUG("*** packet: %p\n", packet);

        packet->command = command;
    }
    else
    {
        RCP_ERROR("could not malloc for packet");
    }

    return packet;
}

void rcp_packet_free(rcp_packet* packet)
{
    if (packet == NULL) return;

    RCP_PACKET_MALLOC_DEBUG("||| free packet: %p\n", packet);

    rcp_option* opt = packet->options;
    rcp_option* next = NULL;

    while (opt)
    {
        next = rcp_option_get_next(opt);
        rcp_option_free(opt);
        opt = next;
    }

    RCP_PACKET_MALLOC_DEBUG("+++ packet: %p\n", packet);
    RCP_FREE(packet);
}

void rcp_packet_set_command(rcp_packet* packet, rcp_packet_command command)
{
    if (packet == NULL) return;

    packet->command = command;
}

rcp_packet_command rcp_packet_get_command(rcp_packet* packet)
{
    if (packet == NULL) return COMMAND_INVALID;

    return packet->command;
}

//----------------------
// timestamp
void rcp_packet_set_timestamp(rcp_packet* packet, uint64_t timestamp)
{
    if (packet == NULL) return;

    rcp_option* opt = rcp_option_get_create(&packet->options, PACKET_OPTIONS_TIMESTAMP);

    rcp_option_set_i64(opt, (int64_t)timestamp);
}

uint64_t rcp_packet_get_timestamp(rcp_packet* packet)
{
    if (packet == NULL) return 0;

    rcp_option* opt = rcp_option_get(packet->options, PACKET_OPTIONS_TIMESTAMP);

    return (uint64_t)rcp_option_get_i64(opt);
}

//----------------------
// iddata
void rcp_packet_set_iddata(rcp_packet* packet, int16_t id)
{
    if (packet == NULL) return;

    rcp_option* opt = rcp_option_get_create(&packet->options, PACKET_OPTIONS_DATA);

    rcp_option_set_i16(opt, id);
}

int16_t rcp_packet_get_iddata(rcp_packet* packet)
{
    if (packet == NULL) return 0;

    rcp_option* opt = rcp_option_get(packet->options, PACKET_OPTIONS_DATA);

    return rcp_option_get_i16(opt);
}

//----------------------
// infodata

// no transfer
void rcp_packet_set_infodata(rcp_packet* packet, rcp_infodata* data)
{
    if (packet == NULL) return;
    if (data == NULL) return;

    rcp_option* opt = rcp_option_get_create(&packet->options, PACKET_OPTIONS_DATA);

    rcp_option_set_infodata(opt, data);
}

// no transfer
rcp_infodata* rcp_packet_get_infodata(rcp_packet* packet)
{
    if (packet == NULL) return NULL;

    rcp_option* opt = rcp_option_get(packet->options, PACKET_OPTIONS_DATA);

    return rcp_option_get_infodata(opt);
}

// full transfer
void rcp_packet_put_infodata(rcp_packet* packet, rcp_infodata* data)
{
    if (packet == NULL) return;
    if (data == NULL) return;

    rcp_option* opt = rcp_option_get_create(&packet->options, PACKET_OPTIONS_DATA);
    rcp_option_put_infodata(opt, data);
}

// full transfer
rcp_infodata* rcp_packet_take_infodata(rcp_packet* packet)
{
    if (packet == NULL) return NULL;

    rcp_option* opt = rcp_option_get(packet->options, PACKET_OPTIONS_DATA);

    return rcp_option_take_infodata(opt);
}

//----------------------
// parameter

// no transfer
void rcp_packet_set_parameter(rcp_packet* packet, rcp_parameter* parameter)
{
    if (packet == NULL) return;
    if (parameter == NULL) return;

    rcp_option* opt = rcp_option_get_create(&packet->options, PACKET_OPTIONS_DATA);

    rcp_option_set_parameter(opt, parameter);
}

// no transfer
rcp_parameter* rcp_packet_get_parameter(rcp_packet* packet)
{
    if (packet == NULL) return NULL;

    rcp_option* opt = rcp_option_get(packet->options, PACKET_OPTIONS_DATA);

    return rcp_option_get_parameter(opt);
}

// full transfer
void rcp_packet_put_parameter(rcp_packet* packet, rcp_parameter* parameter)
{
    if (packet == NULL) return;
    if (parameter == NULL) return;

    rcp_option* opt = rcp_option_get_create(&packet->options, PACKET_OPTIONS_DATA);

    rcp_option_put_parameter(opt, parameter);
}

// full transfer
rcp_parameter* rcp_packet_take_parameter(rcp_packet* packet)
{
    if (packet == NULL) return NULL;

    rcp_option* opt = rcp_option_get(packet->options, PACKET_OPTIONS_DATA);

    return rcp_option_take_parameter(opt);
}



//
void rcp_packet_log(rcp_packet* packet)
{
#ifdef RCP_LOG_INFO
    if (packet == NULL) return;

    RCP_INFO("- packet command: %d\n", packet->command);

    rcp_option* opt = rcp_option_get(packet->options, PACKET_OPTIONS_TIMESTAMP);
    if (opt)
    {
        RCP_INFO("\ttimestamp: %lu\n", (uint64_t)rcp_option_get_i64(opt));
    }

    opt = rcp_option_get(packet->options, PACKET_OPTIONS_DATA);
    if (opt != NULL)
    {
        if (packet->command == COMMAND_INFO)
        {
            rcp_infodata_log(rcp_option_get_infodata(opt));
        }
        else if (packet->command == COMMAND_UPDATE ||
                packet->command == COMMAND_UPDATEVALUE)
        {
            rcp_parameter_log(rcp_option_get_parameter(opt));
        }
        else if (packet->command == COMMAND_REMOVE ||
                   packet->command == COMMAND_DISCOVER ||
                   packet->command == COMMAND_INITIALIZE)
        {
            // ID Data
            RCP_INFO("id data: %d\n", rcp_packet_get_iddata(packet));
        }
        else
        {
            RCP_INFO("other data...!\n");
        }
    }
#endif
}

/**
* rcp_parse_packet
*   parse data to construct a rcp-packet
* data
*   data to read from
* size
*   available data to read from
* out_packet
*   the resulting packet, if any
* out_size
*   data size of data returned by this function
*
* return
*   a pointer to the data after this packet or NULL on error
*/
char* rcp_packet_parse(char* data, size_t size, rcp_packet** out_packet, size_t* out_size)
{
    if (data == NULL) return NULL;

    if (size < 2)
    {
        RCP_ERROR("parse_packet: not enough data");
        return NULL;
    }

    if (*out_packet != NULL)
    {
        RCP_PACKET_DEBUG("potential memory leak - out_packet != NULL");
    }

    rcp_packet* packet = NULL;
    rcp_packet_command command = 0;
    uint8_t option_prefix = 0;

    data = rcp_read_i8(data, &size, (int8_t*)&command);
    if (data == NULL) return NULL;

    if (command == COMMAND_INVALID
            || command >= COMMAND_MAX_)
    {
        RCP_ERROR("invalid command: %d\n", command);
        return NULL;
    }

    packet = rcp_packet_create(command);
    if (packet == NULL)
    {
        RCP_ERROR("could not create packet\n");
        return NULL;
    }

    if (command == COMMAND_UPDATEVALUE)
    {
        // handle update value command
        rcp_parameter* parameter = rcp_parse_value_update(&data, &size);
        if (parameter)
        {
            // NOTE: ownership is transfered
            // use rcp_packet_detach_parameter(packet) to get parameter and take ownership
            rcp_packet_put_parameter(packet, parameter);

            *out_packet = packet;
            *out_size = size;

            return data;
        }
        else
        {
            RCP_PACKET_DEBUG("could not parse parameter (updatevalue)\n");
            rcp_packet_free(packet);
            return NULL;
        }
    }

    while (size > 0)
    {
        option_prefix = 0;
        data = rcp_read_u8(data, &size, &option_prefix);
        if (data == NULL)
        {
            rcp_packet_free(packet);
            return NULL;
        }

        if (option_prefix == RCP_TERMINATOR)
        {
            // !!
            // end of packet
            // !!

            *out_packet = packet;
            *out_size = size;

            return data;
        }

        //
        switch ((rcp_packet_options)option_prefix)
        {
        case PACKET_OPTIONS_TIMESTAMP:
        {
            int64_t val = 0;
            data = rcp_read_i64(data, &size, &val);
            if (data == NULL)
            {
                rcp_packet_free(packet);
                return NULL;
            }

            rcp_packet_set_timestamp(packet, (uint64_t)val);
            break;
        }

        case PACKET_OPTIONS_DATA:

            switch (command)
            {
            case COMMAND_INITIALIZE:
            case COMMAND_DISCOVER:
            case COMMAND_REMOVE:
            {
                // id-data
                int16_t id = 0;
                data = rcp_read_i16(data, &size, &id);
                if (data == NULL)
                {
                    rcp_packet_free(packet);
                    return NULL;
                }

                rcp_packet_set_iddata(packet, id);
                break;
            }

            case COMMAND_INFO:
            {
                rcp_infodata* info_data = rcp_infodata_parse(&data, &size);
                if (info_data)
                {
                    // NOTE: ownership it transfered
                    // use rcp_packet_adetach_infodata() to get infodata and take ownership
                    rcp_packet_put_infodata(packet, info_data);
                }
                else
                {
                    // parsing error
                    RCP_PACKET_DEBUG("could not parse infodata\n");
                    rcp_packet_free(packet);
                    return NULL;
                }
                break;
            }

            case COMMAND_UPDATE:            
            {
                // expect parameter
                rcp_parameter* parameter = rcp_parse_parameter(&data, &size);

                if (parameter)
                {
                    // NOTE: ownership is transfered
                    // use rcp_packet_detach_parameter(packet) to get parameter and take ownership
                    rcp_packet_put_parameter(packet, parameter);
                }
                else
                {
                    // parsing error
                    RCP_PACKET_DEBUG("could not parse parameter\n");
                    rcp_packet_free(packet);
                    return NULL;
                }
                break;
            }

            case COMMAND_UPDATEVALUE:
                // handled above
                break;

            default:
                break;
            } // switch command

            break;
        } // switch packet options
    }

    // parsing error
    RCP_PACKET_DEBUG("packet parsing error\n");
    rcp_packet_free(packet);
    return NULL;
}


static size_t _packet_size(rcp_packet* packet, bool all)
{
    if (packet == NULL) return 0;

    // default command(1) + terminator(1)
    size_t size = 2;

    if (packet->command == COMMAND_UPDATEVALUE)
    {
        // parameter id(2) + type_id(1)
        size += 3;

        // value size
        rcp_parameter* parameter = rcp_packet_get_parameter(packet);
        if (rcp_parameter_is_value(parameter))
        {
            if (RCP_TYPE_ID(parameter) == DATATYPE_CUSTOMTYPE)
            {
                // add size of custom-data size (4 bytes)
                size += 4;
            }

            size += rcp_parameter_get_value_size(RCP_VALUE_PARAMETER(parameter));
        }
        return size;
    }

    // add up options
    rcp_option* opt = packet->options;
    while (opt)
    {
        size += rcp_option_get_size(opt, all);
        opt = rcp_option_get_next(opt);
    }

    return size;
}

// write into buffer
// returns bytes written
size_t rcp_packet_write_buf(rcp_packet* packet, char* data, size_t size, bool all)
{
    if (packet == NULL ||
            data == NULL)
    {
        return 0;
    }

    size_t packet_size = _packet_size(packet, all);

    if (size < packet_size)
    {
        RCP_PACKET_DEBUG("destination buffer not big enough\n");
        return 0;
    }

    size_t written = 0;
    size_t written_len = 0;

    // write commanad
    *data = packet->command;
    written += 1;
    data += 1;


    if (packet->command == COMMAND_UPDATEVALUE)
    {
        rcp_parameter* parameter = rcp_packet_get_parameter(packet);
        written_len = rcp_parameter_write_updatevalue(parameter, data, size - written);
        if (written_len == 0)
        {
            return 0;
        }

        RCP_PACKET_DEBUG("written update value bytes: %d\n", written_len);
        return written + written_len;
    }


    // write all options
    rcp_option* opt = packet->options;
    while (opt)
    {
        if (all || rcp_option_is_changed(opt))
        {
            written_len = rcp_option_write(opt, data, size - written, all);
            RCP_PACKET_DEBUG("packet options - written len: %d\n", written_len);
            if (written_len == 0)
            {
                return 0;
            }

            if (written_len)
            {
                written += written_len;
                data += written_len;
            }
        }

        opt = rcp_option_get_next(opt);
    }

    // write terminator
    *data = RCP_TERMINATOR;
    written += 1;

    return written;
}

// dynamically allocates destination buffer
// use RCP_FREE() to release *dst
// otherwise a memory leak will be reported if RCP_MEM_CHECK is defined (see rcp_memory.h)
// returns the number of bytes written
size_t rcp_packet_write(rcp_packet* packet, char** dst, bool all)
{
    if (packet == NULL)
    {
        return 0;
    }

    // check for potential memory leak
    if (*dst != NULL)
    {
        RCP_PACKET_DEBUG("potential memory leak - *dst != NULL\n");
    }


    // get serialized size
    size_t packet_data_size = _packet_size(packet, all);
    RCP_PACKET_DEBUG("packet size: %d\n", packet_data_size);

    // alloc memory
    *dst = RCP_CALLOC(1, packet_data_size);
    if (*dst == NULL)
    {
        RCP_ERROR("could not allocate for serialize data\n");
        return 0;
    }

    RCP_PACKET_MALLOC_DEBUG("*** data output: %p\n", *dst);

    size_t written = rcp_packet_write_buf(packet, *dst, packet_data_size, all);
    if (written == 0)
    {
        // free data
        RCP_PACKET_MALLOC_DEBUG("+++ data output: %p\n", *dst);
        RCP_FREE(*dst);
        *dst = NULL;
    }

    return written;
}
