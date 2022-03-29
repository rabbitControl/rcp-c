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

#include "rcp_slip.h"

#include <string.h>
#include <stdbool.h>

#include "rcp_logging.h"
#include "rcp_memory.h"

struct rcp_slip
{
  char* buffer;
  size_t bufferSize;
  size_t writePos;
  bool lastCharEsc;
  bool bufferExtern;

  void (*packetCb)(char* data, size_t size, void* user);
  void* user;
};

rcp_slip* rcp_slip_create(size_t size)
{
  rcp_slip* s = (rcp_slip*)RCP_CALLOC(1, sizeof(rcp_slip));

  if (s != NULL)
  {
      RCP_DEBUG("*** slip: %p\n", s);
      rcp_slip_create_buffer(s, size);
  }

  return s;
}

void rcp_slip_free_buffer(rcp_slip* s)
{
  if (s &&
          !s->bufferExtern &&
          s->buffer)
  {
    RCP_DEBUG("+++ slip buffer: %p\n", s->buffer);
    RCP_FREE(s->buffer);
    s->buffer = NULL;
    s->bufferSize = 0;
    s->writePos = 0;
    s->lastCharEsc = false;
  }
}

void rcp_slip_free(rcp_slip* s)
{
  if (s)
  {
    rcp_slip_free_buffer(s);

    RCP_DEBUG("+++ slip: %p\n", s);
    RCP_FREE(s);
  }
}

void rcp_slip_create_buffer(rcp_slip* s, size_t size)
{
  if (s == NULL) return;

  rcp_slip_free_buffer(s);

  if (size > 0)
  {
    s->buffer = RCP_MALLOC(size);
    if (s->buffer)
    {
      RCP_DEBUG("*** slip buffer: %p\n", s->buffer);

      s->bufferSize = size;
      s->bufferExtern = false;
    }
  }

}

void rcp_slip_set_buffer(rcp_slip* s, char* buffer, size_t size)
{
  if (s == NULL) return;
  if (buffer == NULL) return;
  if (size == 0) return;

  rcp_slip_free_buffer(s);

  s->buffer = buffer;
  s->bufferSize = size;
  s->bufferExtern = true;
}

void rcp_slip_set_user(rcp_slip* s, void* user)
{
  if (s)
  {
    s->user = user;
  }
}

void rcp_slip_set_packet_cb(rcp_slip* s, void (*packetCb)(char*, size_t, void*))
{
  if (s)
  {
    s->packetCb = packetCb;
  }
}

void rcp_slip_append(rcp_slip* s, unsigned char c)
{
  if (s == NULL) return;
  if (s->buffer == NULL) return;
  if (s->bufferSize == 0) return;

  if (c == END)
  {
    if (s->writePos > 0)
    {
      if (s->packetCb)
      {
        // call cb
        s->packetCb(s->buffer, s->writePos, s->user);
      }

      s->writePos = 0;
    }
    return;
  }


  if (c == ESC )
  {
    s->lastCharEsc = true;
    return;
  }


  if (s->lastCharEsc)
  {
    s->lastCharEsc = false;
    switch (c)
    {
      case ESC_END:
        c = END;
        break;
      case ESC_ESC:
        c = ESC;
        break;
    }
  }


  // TODO: handle edge case is packetsize == buffer size !!
  // append character
  s->buffer[s->writePos] = (char)c;
  s->writePos++;
  if (s->writePos >= s->bufferSize)
  {
    // overflow!
    s->writePos = 0;
  }
}

void rcp_slip_append_data(rcp_slip* s, char* data, size_t size)
{
    if (s == NULL) return;
    if (data == NULL) return;
    if (size == 0) return;

    for (size_t i=0; i<size; i++ )
    {
        rcp_slip_append(s, data[i]);
    }
}


// encode data to slip, "send" data to dataCb(char, void*)
void rcp_slip_encode(char* data, size_t size, void (*dataCb)(char, void*), void* user)
{
  if (data == NULL) return;
  if (dataCb == NULL) return;
  if (size == 0) return;

  /* send an initial END character to flush out any data that may
        have accumulated in the receiver due to line noise
  */
  dataCb(END, user);

  /* for each byte in the packet, send the appropriate character
     sequence
  */
  while (size--)
  {
    switch ((unsigned char)*data)
    {
      /* if it's the same code as an END character, we send a
         special two character code so as not to make the
         receiver think we sent an END
      */
      case END:
        dataCb(ESC, user);
        dataCb(ESC_END, user);
        break;

      /* if it's the same code as an ESC character,
         we send a special two character code so as not
         to make the receiver think we sent an ESC
      */
      case ESC:
        dataCb(ESC, user);
        dataCb(ESC_ESC, user);
        break;

      /* otherwise, we just send the character
      */
      default:
        dataCb(*data, user);
    }

    data++;
  }

  dataCb(END, user);
}
