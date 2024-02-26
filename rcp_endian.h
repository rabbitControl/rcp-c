/*
********************************************************************
* rabbitcontrol - a protocol and data-format for remote control.
*
* https://rabbitcontrol.cc
* https://github.com/rabbitcontrol/rcp-c
*
* This file is part of rabbitcontrol for c.
*
* Written by Ingo Randolf, 2021 - 2024
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
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
