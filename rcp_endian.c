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

#include "rcp_endian.h"

static __inline uint16_t
__bswap16(uint16_t _x)
{
  return ((uint16_t)((_x >> 8) | ((_x << 8) & 0xff00)));
}

static __inline uint32_t
__bswap32(uint32_t _x)
{
  return ((uint32_t)((_x >> 24) | ((_x >> 8) & 0xff00) |
      ((_x << 8) & 0xff0000) | ((_x << 24) & 0xff000000)));
}

static __inline uint64_t
__bswap64(uint64_t _x)
{
  return ((uint64_t)((_x >> 56) | ((_x >> 40) & 0xff00) |
      ((_x >> 24) & 0xff0000) | ((_x >> 8) & 0xff000000) |
      ((_x << 8) & ((uint64_t)0xff << 32)) |
      ((_x << 24) & ((uint64_t)0xff << 40)) |
      ((_x << 40) & ((uint64_t)0xff << 48)) | ((_x << 56))));
}

#ifndef _WIN32
  #ifndef ntohs
  uint16_t ntohs(uint16_t x)
  {
  #if _BYTE_ORDER == _LITTLE_ENDIAN
    // swap
    return __bswap16(x);
  #else
    return x;
  #endif
  }
  #endif

  #ifndef ntohl
  uint32_t ntohl(uint32_t x)
  {
  #if _BYTE_ORDER == _LITTLE_ENDIAN
    // swap
    return __bswap32(x);
  #else
    return x;
  #endif
  }
  #endif
#endif

#ifndef ntohll
uint64_t ntohll(uint64_t x)
{
#if _BYTE_ORDER == _LITTLE_ENDIAN
  // swap
  return __bswap64(x);
#else
  return x;
#endif
}
#endif
