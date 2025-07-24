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

#ifndef RCP_ENDIAN_H
#define RCP_ENDIAN_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef _WIN32
  #include <winsock.h>
#else
  #ifndef ntohs
  uint16_t ntohs(uint16_t x);
  #endif

  #ifndef ntohl
  uint32_t ntohl(uint32_t x);
  #endif
#endif

#ifndef ntohll
uint64_t ntohll(uint64_t x);
#endif

#ifndef htonl
#define htonl(x) ntohl(x)
#endif


#define _rcp_be16(x) ntohs(x)
#define _rcp_be32(x) ntohl(x)
#define _rcp_be64(x) ntohll(x)

#define _rcp_load16(cast, from, to) do {       \
    memcpy((cast*)(to), (from), sizeof(cast)); \
    *(to) = _rcp_be16(*(to));                  \
} while (0);
#define _rcp_load32(cast, from, to) do {       \
    memcpy((cast*)(to), (from), sizeof(cast)); \
    *(to) = (cast)_rcp_be32(*(to));                  \
} while (0);
#define _rcp_load64(cast, from, to) do {       \
    memcpy((cast*)(to), (from), sizeof(cast)); \
    *(to) = _rcp_be64(*(to));                  \
} while (0);


#define _rcp_store16(to, num) \
    do { uint16_t val = _rcp_be16(num); memcpy(to, &val, 2); } while(0)
#define _rcp_store32(to, num) \
    do { uint32_t val = _rcp_be32(num); memcpy(to, &val, 4); } while(0)
#define _rcp_store64(to, num) \
    do { uint64_t val = _rcp_be64(num); memcpy(to, &val, 8); } while(0)


#ifdef __cplusplus
} // extern "C"
#endif

#endif
