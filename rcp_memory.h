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

#ifndef RCP_MEMORY_H
#define RCP_MEMORY_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stdlib.h>

//#define RCP_MEM_CHECK

#ifdef RCP_MEM_CHECK

    extern void rcp_malloc_cb(void* ptr);
    extern void rcp_realloc_cb(void* ptr, void* oldptr); // oldptr might have been freed
    extern void rcp_free_cb(void* ptr);

    #define RCP_MALLOC(...) ({void* ptr = malloc(__VA_ARGS__); rcp_malloc_cb(ptr); ptr;})
    #define RCP_CALLOC(...) ({void* ptr = calloc(__VA_ARGS__); rcp_malloc_cb(ptr); ptr;})
    #define RCP_REALLOC(p, s) ({void* ptr = realloc(p, s); rcp_realloc_cb(ptr, p); ptr;})
    #define RCP_FREE(ptr) ({rcp_free_cb(ptr); free(ptr);})

#else

    #define RCP_MALLOC(...) malloc(__VA_ARGS__)
    #define RCP_CALLOC(...) calloc(__VA_ARGS__)
    #define RCP_REALLOC(...) realloc(__VA_ARGS__)
    #define RCP_FREE(ptr) free(ptr)

#endif


#ifdef __cplusplus
} // extern "C"
#endif

#endif
