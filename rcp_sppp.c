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

#include "rcp_sppp.h"

#include <string.h>

#include "rcp.h"
#include "rcp_memory.h"
#include "rcp_logging.h"


#define RCP_SPPP_BLOCK_SIZE 32


#if defined(RCP_SPPP_DEBUG_LOG) || defined(RCP_ALL_DEBUG)
#define RCP_SPPP_DEBUG(...) RCP_DEBUG(__VA_ARGS__)
#else
#define RCP_SPPP_DEBUG(...)
#endif

#if defined(RCP_SPPP_MALLOC_DEBUG_LOG) || defined(RCP_ALL_DEBUG)
#define RCP_SPPP_MALLOC_DEBUG(...) RCP_DEBUG(__VA_ARGS__)
#else
#define RCP_SPPP_MALLOC_DEBUG(...)
#endif


typedef enum rcp_sppp_flags
{
    SPPP_PACKET_BYPASS = 0, // lag to relay incoming data-bytes (size prefixed data)
    SPPP_PACKET_INVALID,    // flag if packet is invalid
    SPPP_REPORT_ZERO_PACKETS
} rcp_sppp_flags;

struct rcp_sppp
{
    char* buffer;                                   // buffer
    size_t buffer_size;                             // current buffer size
    size_t max_buffer_size;                         // max buffer size
    size_t current_idx;                             // current write index in buffer
    size_t packet_size;                             // next packet size
    unsigned char flags;                            // flags
    //
    void (*packet_cb)(const char*, size_t, void*);        // packet callback
    void* user;                                     // user field
    //
    void (*bypass_data_cb)(const char*, size_t, void*);   // callback for bypass bytes
    void (*bypass_done_cb)(void*);                  // callback triggers if: bypass > 0 and packet_size == 0
};


static char set_buffer_size(rcp_sppp* pp, size_t size)
{
    if (pp == NULL) return 0;

    if (size > pp->max_buffer_size)
    {
        // ERROR!
        return 0;
    }

    if (size < RCP_SPPP_BLOCK_SIZE)
    {
        size = RCP_SPPP_BLOCK_SIZE;
    }
    else if ((size % RCP_SPPP_BLOCK_SIZE) != 0)
    {
        // use a multiple of blocksize
        size_t new_size = size + RCP_SPPP_BLOCK_SIZE - (size % RCP_SPPP_BLOCK_SIZE);
        if (new_size != size
                && new_size < pp->max_buffer_size)
        {
            size = new_size;
        }
    }

    if (size == pp->buffer_size) return 1;

    // try to realloc
    RCP_SPPP_DEBUG("sppp try to realloc: %lu\n", size);

    char* new_p = NULL;
    new_p = RCP_REALLOC(pp->buffer, size);

    if (new_p == NULL)
    {
        RCP_SPPP_DEBUG("sppp could not realloc buffer: %lu\n", size);
        return 0;
    }

    RCP_SPPP_MALLOC_DEBUG("*** sppp buffer: %p\n", new_p);

    // all fine, set it
    pp->buffer = new_p;
    pp->buffer_size = size;
    return 1;
}

static void minimize_buffer(rcp_sppp* pp)
{
    if (pp == NULL) return;

    RCP_SPPP_DEBUG("sppp minimize: %lu\n", pp->current_idx);

    set_buffer_size(pp, pp->current_idx);
}


static char memcpy_data_to_buffer(rcp_sppp* pp, const void* data, size_t size)
{
    if (pp == NULL || data == NULL || size == 0) return 0;

    size_t new_size = pp->current_idx + size;

    if (new_size < pp->current_idx)
    {
        // type overflow
        return 0;
    }

    if (new_size > pp->buffer_size)
    {
        if (!set_buffer_size(pp, new_size))
        {
            // could not resize buffer
            return  0;
        }
    }

    RCP_SPPP_DEBUG("adding to buffer: %lu\n", size);

    memcpy(pp->buffer + pp->current_idx, data, size);
    pp->current_idx += size;

    RCP_SPPP_DEBUG("buffer index: %lu\n", pp->current_idx);

    return 1;
}


rcp_sppp* rcp_sppp_create(size_t max_buffer_size, void (*packet_cb)(const char*, size_t, void*), void* user)
{
    rcp_sppp* pp = RCP_CALLOC(1, sizeof(rcp_sppp));

    if (pp)
    {
        RCP_SPPP_MALLOC_DEBUG("*** sppp: %p\n", pp);

        pp->max_buffer_size = max_buffer_size;
        pp->packet_cb = packet_cb;
        pp->user = user;

        // set initial buffer
        set_buffer_size(pp, RCP_SPPP_BLOCK_SIZE);
    }

    return pp;
}

void rcp_sppp_free(rcp_sppp* pp)
{
    if (pp == NULL) return;

    if (pp->buffer != NULL)
    {
        RCP_SPPP_MALLOC_DEBUG("+++ sppp buffer: %p\n", pp->buffer);
        RCP_FREE(pp->buffer);
        pp->buffer = NULL;
        pp->buffer_size = 0;
    }

    RCP_SPPP_MALLOC_DEBUG("+++ sppp: %p\n", pp);
    RCP_FREE(pp);
}

void rcp_sppp_reset(rcp_sppp* pp)
{
    if (pp != NULL)
    {
        pp->current_idx = 0;
        pp->packet_size = 0;
        pp->flags = 0;

        minimize_buffer(pp);
    }
}

void rcp_sppp_set_packet_cb(rcp_sppp* pp, void (*packet_cb)(const char*, size_t, void*), void* user)
{
    if (pp == NULL) return;

    pp->packet_cb = packet_cb;
    pp->user = user;
}

void rcp_sppp_set_bypass_cb(rcp_sppp* pp, void (*data_cb)(const char*, size_t, void*), void (*done_cb)(void*))
{
    if (pp == NULL) return;

    pp->bypass_data_cb = data_cb;
    pp->bypass_done_cb = done_cb;
}

void rcp_sppp_data(rcp_sppp* pp, const char* data, size_t size)
{
    if (pp == NULL || data == NULL || size == 0) return;

    RCP_SPPP_DEBUG("rcp_sppp_push_data: %d - %d\n", size, data[0]);


    while (size > 0)
    {
        // make sure we know packet_size
        while (pp->packet_size == 0)
        {
            // read 4 bytes to get next packet size
            while (pp->current_idx < 4 &&
                   size > 0)
            {
                // read one
                pp->buffer[pp->current_idx] = *data;
                pp->current_idx++;
                size--;
                data++;
            }

            if (pp->current_idx == 4)
            {
                pp->packet_size = MACRO_U32_FROM_4U8(pp->buffer[3], pp->buffer[2], pp->buffer[1], pp->buffer[0]);
                pp->current_idx = 0;
                RCP_CLEARFLAG(pp->flags, SPPP_PACKET_INVALID);

                RCP_SPPP_DEBUG("new packet size: %d\n", pp->packet_size);

                if (pp->packet_size > pp->max_buffer_size &&
                        !(RCP_CHECKFLAG(pp->flags, SPPP_PACKET_BYPASS)))
                {
                    // buffer is too small to hold packet
                    // no bypass
                    // NOTE: at some point memcpy_data_to_buffer will fail and produce invalid packets
                    // set flag now to spare memory copy
                    RCP_SETFLAG(pp->flags, SPPP_PACKET_INVALID);
                }
                else if (pp->packet_size == 0 &&
                           RCP_CHECKFLAG(pp->flags, SPPP_REPORT_ZERO_PACKETS))
                {
                    // zero size packet
                    pp->packet_cb(NULL, 0, pp->user);
                }
            }

            if (size == 0)
            {
                return;
            }
        }

        // we got a packet size > 0 AND
        // we know the size is > 0

        RCP_SPPP_DEBUG("packet_size: %d\n", pp->packet_size);
        RCP_SPPP_DEBUG("current_idx: %d\n", pp->current_idx);


        if (RCP_CHECKFLAG(pp->flags, SPPP_PACKET_BYPASS))
        {
            // use packet_size as counter
            if (pp->packet_size <= size)
            {
                // bypass the amount of data we are waiting for...
                if (pp->bypass_data_cb != NULL)
                {
                    pp->bypass_data_cb(data, pp->packet_size, pp->user);
                }

                // reset variables now
                // bypass_done_cb might set bypass to 1 again
                pp->flags = 0;
                size -= pp->packet_size;
                data += pp->packet_size;

                pp->packet_size = 0;
                pp->current_idx = 0;
                minimize_buffer(pp);

                if (pp->bypass_done_cb != NULL)
                {
                    pp->bypass_done_cb(pp->user);
                }
            }
            else
            {
                if (pp->bypass_data_cb != NULL)
                {
                    pp->bypass_data_cb(data, size, pp->user);
                }

                pp->packet_size -= size;
                pp->current_idx += size;

                // make sure we break
                size = 0;
            }
        }
        else
        {
            if (pp->packet_size < pp->current_idx)
            {
                // error - can this even happen?
                // handle error, how?
            }

            size_t size_to_packet = pp->packet_size - pp->current_idx;
            RCP_SPPP_DEBUG("size_to_packet: %d\n", size_to_packet);

            if (size_to_packet <= size)
            {
                if (pp->current_idx == 0)
                {
                    if (!(RCP_CHECKFLAG(pp->flags, SPPP_PACKET_INVALID)))
                    {
                        // no need to copy - just output data
                        if (pp->packet_cb)
                        {
                            pp->packet_cb(data, pp->packet_size, pp->user);
                        }
                    }
                    else
                    {
                        RCP_SPPP_DEBUG("sppp - dropping invalid packet\n");
                    }
                }
                else
                {
                    // we need to append data to our buffer first
                    // copy the part we need
                    // copy only if packet is not invalid already

                    if (!(RCP_CHECKFLAG(pp->flags, SPPP_PACKET_INVALID)) &&
                            (size_to_packet == 0 || memcpy_data_to_buffer(pp, data, size_to_packet)))
                    {
                        // full packet in buffer - send it
                        // check packet validity
                        if (pp->packet_cb)
                        {
                            pp->packet_cb(pp->buffer, pp->current_idx, pp->user);
                        }
                    }
                    else if (size_to_packet > 0)
                    {
                        // invalid packet
                        RCP_SETFLAG(pp->flags, SPPP_PACKET_INVALID);
                    }

                    if (RCP_CHECKFLAG(pp->flags, SPPP_PACKET_INVALID))
                    {
                        RCP_SPPP_DEBUG("sppp - dropping data -> invalid packet\n");
                    }
                }

                // reset
                // leave bypass flag as it is, it may have been set in packet_cb
                pp->current_idx = 0;
                pp->packet_size = 0;
                RCP_CLEARFLAG(pp->flags, SPPP_PACKET_INVALID);

                minimize_buffer(pp);

                size -= size_to_packet;
                data += size_to_packet;
            }
            else
            {
                // if packet is valid, copy data to internal buffer
                if ((RCP_CHECKFLAG(pp->flags, SPPP_PACKET_INVALID)) ||
                        !memcpy_data_to_buffer(pp, data, size))
                {
                    // packet is invalid already or
                    // error copying data -> invalid packet

                    RCP_SPPP_DEBUG("packet invalid or could not copy data to buffer\n");

                    // reduce packet_size to know when all packet data arrived
                    pp->packet_size -= size;
                    RCP_SETFLAG(pp->flags, SPPP_PACKET_INVALID);
                }

                // make sure we break
                size = 0;
            }
        }


        RCP_SPPP_DEBUG("data_size: %d\n", size);
    }
}


void rcp_sppp_set_bypass(rcp_sppp* pp, unsigned char bypass)
{
    if (pp == NULL) return;

    //
    if (bypass > 1) bypass = 1;

    if ((unsigned char)(RCP_CHECKFLAG(pp->flags, SPPP_PACKET_BYPASS)) == bypass) return;

    if (bypass)
    {
        RCP_SETFLAG(pp->flags, SPPP_PACKET_BYPASS);
    }
    else
    {
        RCP_CLEARFLAG(pp->flags, SPPP_PACKET_BYPASS);
    }

    if (pp->packet_size > 0 &&
            pp->current_idx > 0)
    {
        if (RCP_CHECKFLAG(pp->flags, SPPP_PACKET_BYPASS))
        {
            // in case we have a packet_size AND
            // data is already in our buffer, output that data
            if (pp->bypass_data_cb != NULL)
            {
                pp->bypass_data_cb(pp->buffer, pp->current_idx, pp->user);
            }

            // reduce packet size
            if (pp->packet_size >= pp->current_idx)
            {
                pp->packet_size -= pp->current_idx;
            }
            else
            {
                // some state error
                // how did that happen??
                // ??

                // in any case, reset packet_size
                pp->packet_size = 0;
            }

            pp->current_idx = 0;
            minimize_buffer(pp);
        }
        else
        {
            // turning bypass off after having bypassed data produces invalid packets
            RCP_SETFLAG(pp->flags, SPPP_PACKET_INVALID);
            pp->current_idx = 0;
        }
    }
}

char rcp_sppp_get_bypass(rcp_sppp* pp)
{
    if (pp != NULL)
    {
        return (char)(RCP_CHECKFLAG(pp->flags, SPPP_PACKET_BYPASS));
    }

    return 0;
}

void rcp_sppp_set_report_zerosize(rcp_sppp* pp)
{
    if (pp == NULL) return;

    RCP_SETFLAG(pp->flags, SPPP_REPORT_ZERO_PACKETS);
}

void rcp_sppp_clear_report_zerosize(rcp_sppp* pp)
{
    if (pp == NULL) return;

    RCP_CLEARFLAG(pp->flags, SPPP_REPORT_ZERO_PACKETS);
}

size_t rcp_sppp_get_packet_size(rcp_sppp* pp)
{
    if (pp != NULL)
    {
        return pp->packet_size;
    }

    return 0;
}
