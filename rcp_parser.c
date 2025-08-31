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

#include "rcp_parser.h"

#include <string.h>

#include "rcp_endian.h"
#include "rcp_parameter.h"
#include "rcp_typedefinition.h"
#include "rcp_logging.h"

const char* rcp_read_i8(const char* data, size_t* size, int8_t* target)
{
    if (data == NULL) return NULL;
    if (target == NULL) return NULL;
    if (*size < 1) return NULL;

    *target = data[0];
    *size -= 1;

    return data+1;
}

const char* rcp_read_u8(const char* data, size_t* size, uint8_t* target)
{
    return rcp_read_i8(data, size, (int8_t*)target);
}

const char* rcp_read_i16(const char* data, size_t* size, int16_t* target)
{
    if (data == NULL) return NULL;
    if (target == NULL) return NULL;
    if (*size < 2) return NULL;

    _rcp_load16(int16_t, data, target);

    *size -= 2;
    return data+2;
}



const char* rcp_read_i32(const char* data, size_t* size, int32_t* target)
{
    if (data == NULL) return NULL;
    if (target == NULL) return NULL;
    if (*size < 4) return NULL;

    _rcp_load32(int32_t, data, target);

    *size -= 4;
    return data+4;
}

const char* rcp_read_i64(const char* data, size_t* size, int64_t* target)
{
    if (data == NULL) return NULL;
    if (target == NULL) return NULL;
    if (*size < 8) return NULL;

    _rcp_load64(int64_t, data, target);

    *size -= 8;
    return data+8;
}

const char* rcp_read_f32(const char* data, size_t* size, float* target)
{
    if (data == NULL) return NULL;
    if (target == NULL) return NULL;
    if (*size < 4) return NULL;

	unsigned int x = 0x76543210;
	char* c = (char*) &x;
	if (*c == 0x10)
	{
		// little endian - swap
		char f[4];
		f[0] = data[3];
		f[1] = data[2];
		f[2] = data[1];
		f[3] = data[0];
		
		memcpy(target, f, sizeof(float));
	}
	else
	{
		// big endian - just copy
		memcpy(target, data, sizeof(float));
	}

    *size -= 4;
    return data+4;
}

const char* rcp_read_f64(const char* data, size_t* size, double* target)
{
    if (data == NULL) return NULL;
    if (target == NULL) return NULL;
    if (*size < 8) return NULL;

	unsigned int x = 0x76543210;
	char* c = (char*) &x;
	if (*c == 0x10)
	{
		// little endian - swap
		char f[8];
		f[0] = data[7];
		f[1] = data[6];
		f[2] = data[5];
		f[3] = data[4];
        f[4] = data[3];
		f[5] = data[2];
		f[6] = data[1];
		f[7] = data[0];

		memcpy(target, f, sizeof(double));
	}
	else
	{
		// big endian - just copy
		memcpy(target, data, sizeof(double));
	}

    *size -= 8;
    return data+8;
}



static rcp_parameter* _create_parameter_from_data(const char** data, size_t* size)
{
    if (data == NULL) return NULL;
    if (*data == NULL) return NULL;

    int16_t parameter_id = 0;
    rcp_datatype datatype_id = DATATYPE_INVALID;
    const char* r_data;

    // we need at least 3 bytes
    if (*size >= 3)
    {
        // get id
        r_data = rcp_read_i16(*data, size, &parameter_id);
        if (r_data == NULL) return NULL;

        *data = r_data;

        // get type-id
        r_data = rcp_read_i8(*data, size, (int8_t*)&datatype_id);
        if (r_data == NULL) return NULL;

        *data = r_data;


        if (parameter_id != 0)
        {
            switch (datatype_id)
            {
                case DATATYPE_BOOLEAN:
                    return RCP_PARAMETER(rcp_bool_parameter_create(parameter_id));
                case DATATYPE_INT8:
                    return RCP_PARAMETER(rcp_i8_parameter_create(parameter_id));
                case DATATYPE_UINT8:
                    return RCP_PARAMETER(rcp_u8_parameter_create(parameter_id));
                case DATATYPE_INT16:
                    return RCP_PARAMETER(rcp_i16_parameter_create(parameter_id));
                case DATATYPE_UINT16:
                    return RCP_PARAMETER(rcp_u16_parameter_create(parameter_id));
                case DATATYPE_INT32:
                    return RCP_PARAMETER(rcp_i32_parameter_create(parameter_id));
                case DATATYPE_UINT32:
                    return RCP_PARAMETER(rcp_u32_parameter_create(parameter_id));
                case DATATYPE_FLOAT32:
                    return RCP_PARAMETER(rcp_f32_parameter_create(parameter_id));
                case DATATYPE_VECTOR2F32:
                    return RCP_PARAMETER(rcp_vector2f32_parameter_create(parameter_id));
                case DATATYPE_STRING:
                    return RCP_PARAMETER(rcp_string_parameter_create(parameter_id));
                case DATATYPE_ENUM:
                    return RCP_PARAMETER(rcp_enum_parameter_create(parameter_id));
                case DATATYPE_BANG:
                    return RCP_PARAMETER(rcp_bang_parameter_create(parameter_id));
                case DATATYPE_GROUP:
                    return RCP_PARAMETER(rcp_group_parameter_create(parameter_id));
                case DATATYPE_IPV4:
                    return RCP_PARAMETER(rcp_ipv4_parameter_create(parameter_id));

                case DATATYPE_CUSTOMTYPE:
                {
                    // parse size
                    uint32_t p_size = 0;
                    r_data = rcp_read_i32(*data, size, (int32_t*)&p_size);

                    if (r_data == NULL) return NULL;

                    *data = r_data;

                    return RCP_PARAMETER(rcp_custom_parameter_create(parameter_id, p_size));
                }

                default:
                RCP_DEBUG("type id not implemented: %d\n", datatype_id);
                    break;
            }
        }

        return NULL;
    }

    // no data to create parameter
    return NULL;
}

rcp_parameter* rcp_parse_parameter(const char** data, size_t* size)
{
    // smalles possible parameter = 5 bytes (2byte id, 1byte typeid, term, term)
    if (*size < 5) return NULL;

    rcp_parameter* parameter = _create_parameter_from_data(data, size);

    if (parameter)
    {
        // parse type-options
        const char* r_data = rcp_typedefinition_parse_type_options(rcp_parameter_get_typedefinition(parameter), *data, size);
        if (r_data == NULL)
        {
            rcp_parameter_free(parameter);
            return NULL;
        }

        *data = r_data;

        if (*size > 0)
        {
            // parse parameter options
            const char* r_data = rcp_parameter_parse_options(parameter, *data, size);
            if (r_data == NULL)
            {
                rcp_parameter_free(parameter);
                return NULL;
            }

            *data = r_data;
        }
    }

    return parameter;
}


rcp_parameter* rcp_parse_value_update(const char** data, size_t* size)
{
    if (data == NULL) return NULL;
    if (*data == NULL) return NULL;

    /*
    command     0x06                byte                - 	n 	updateValue command

    data:
    parameter   id                  int16               0 	n 	parameter id
    mandatory part of datatype 		byte                0 	n 	datatype
    value                           type of datatype 	? 	n 	the value
    */

    // smallest data = 3 bytes (2 byte id, 1 byte typeid)
    if (*size < 3) return NULL;

    rcp_parameter* parameter = _create_parameter_from_data(data, size);

    if (parameter &&
            !rcp_parameter_is_type(parameter, DATATYPE_BANG))
    {
        // parse value
        const char* r_data = rcp_parameter_parse_value(parameter, *data, size);
        if (r_data == NULL)
        {
            rcp_parameter_free(parameter);
            return NULL;
        }

        *data = r_data;
    }

    return parameter;
}
