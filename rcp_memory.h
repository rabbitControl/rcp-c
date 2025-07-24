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
