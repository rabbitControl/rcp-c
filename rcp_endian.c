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
