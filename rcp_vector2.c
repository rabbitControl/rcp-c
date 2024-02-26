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

#include "rcp_vector2.h"

#include <stdio.h>

#include "rcp_memory.h"
#include "rcp_logging.h"
#include "rcp_endian.h"

#if defined(RCP_VECTOR_DEBUG_LOG) || defined(RCP_ALL_DEBUG)
#define RCP_VECTOR_DEBUG(...) RCP_DEBUG(__VA_ARGS__)
#else
#define RCP_VECTOR_DEBUG(...)
#endif

#if defined(RCP_VECTOR_MALLOC_DEBUG_LOG) || defined(RCP_ALL_DEBUG)
#define RCP_VECTOR_MALLOC_DEBUG(...) RCP_DEBUG(__VA_ARGS__)
#else
#define RCP_VECTOR_MALLOC_DEBUG(...)
#endif

union rcp_vector2
{
    int32_t i[2];
    float f[2];
};


rcp_vector2* rcp_vector2_create()
{
    rcp_vector2* vector = (rcp_vector2*)RCP_CALLOC(1, sizeof(rcp_vector2));

    if (vector)
    {
        RCP_VECTOR_MALLOC_DEBUG("*** vector: %p\n", vector);
    }

    return vector;
}

void rcp_vector2_free(rcp_vector2* vector)
{
    if (vector != NULL)
    {
        RCP_VECTOR_MALLOC_DEBUG("+++ vector: %p\n", vector);
        RCP_FREE(vector);
    }
}

size_t rcp_vector2_write(rcp_vector2* vector, char* data, size_t size)
{
    if (vector == NULL) return 0;
    if (data == NULL) return 0;

    if (size < 2*sizeof(int32_t))
    {
        RCP_VECTOR_DEBUG("could not write vector - buffer overflow\n");
        return 0;
    }

    _rcp_store32(data, vector->i[0]);
    _rcp_store32(data + sizeof(int32_t), vector->i[1]);

    return 2*sizeof(int32_t);
}

void rcp_vector2_set_i(rcp_vector2* vector, int32_t x, int32_t y)
{
    if (vector == NULL) return;

    vector->i[0] = x;
    vector->i[1] = y;
}

int32_t rcp_vector2_get_i_x(rcp_vector2* vector)
{
    if (vector == NULL) return 0;

    return vector->i[0];
}

int32_t rcp_vector2_get_i_y(rcp_vector2* vector)
{
    if (vector == NULL) return 0;

    return vector->i[1];
}

void rcp_vector2_set_f(rcp_vector2* vector, float x, float y)
{
    if (vector == NULL) return;

    vector->f[0] = x;
    vector->f[1] = y;
}

float rcp_vector2_get_f_x(rcp_vector2* vector)
{
    if (vector == NULL) return 0.;

    return vector->f[0];
}

float rcp_vector2_get_f_y(rcp_vector2* vector)
{
    if (vector == NULL) return 0.;

    return vector->f[1];
}
