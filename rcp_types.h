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

#ifndef RCP_TYPES_H
#define RCP_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define RCP_TERMINATOR 0
#define RCP_OPTIONS_DEFAULT 48

enum rcp_enum_options_t {
    ENUM_OPTIONS_DEFAULT = 48,
    ENUM_OPTIONS_ENTRIES = 49,
    ENUM_OPTIONS_MULTISELECT = 50
};

enum rcp_numberbox_options_t {
    NUMBERBOX_OPTIONS_PRECISION = 86,
    NUMBERBOX_OPTIONS_FORMAT = 87,
    NUMBERBOX_OPTIONS_STEPSIZE = 88,
    NUMBERBOX_OPTIONS_CYCLIC = 89
};

enum rcp_customtype_options_t {
    CUSTOMTYPE_OPTIONS_DEFAULT = 48,
    CUSTOMTYPE_OPTIONS_UUID = 49,
    CUSTOMTYPE_OPTIONS_CONFIG = 50
};

enum rcp_widget_options_t {
    WIDGET_OPTIONS_ENABLED = 80,
    WIDGET_OPTIONS_LABEL_VISIBLE = 81,
    WIDGET_OPTIONS_VALUE_VISIBLE = 82,
    WIDGET_OPTIONS_NEEDS_CONFIRMATION = 83
};

enum rcp_color_options_t {
    COLOR_OPTIONS_DEFAULT = 48
};

enum rcp_parameter_options_t {
    PARAMETER_OPTIONS_VALUE = 32,
    PARAMETER_OPTIONS_LABEL = 33,
    PARAMETER_OPTIONS_DESCRIPTION = 34,
    PARAMETER_OPTIONS_TAGS = 35,
    PARAMETER_OPTIONS_ORDER = 36,
    PARAMETER_OPTIONS_PARENTID = 37,
    PARAMETER_OPTIONS_WIDGET = 38,
    PARAMETER_OPTIONS_USERDATA = 39,
    PARAMETER_OPTIONS_USERID = 40,
    PARAMETER_OPTIONS_READONLY = 41
};

enum rcp_ipv4_options_t {
    IPV4_OPTIONS_DEFAULT = 48
};

enum rcp_vector_options_t {
    VECTOR_OPTIONS_DEFAULT = 48,
    VECTOR_OPTIONS_MINIMUM = 49,
    VECTOR_OPTIONS_MAXIMUM = 50,
    VECTOR_OPTIONS_MULTIPLEOF = 51,
    VECTOR_OPTIONS_SCALE = 52,
    VECTOR_OPTIONS_UNIT = 53
};

enum rcp_boolean_options_t {
    BOOLEAN_OPTIONS_DEFAULT = 48
};

enum rcp_widgettype_t {
    WIDGETTYPE_DEFAULT = 1,
    WIDGETTYPE_CUSTOM = 2,
    WIDGETTYPE_INFO = 16,
    WIDGETTYPE_TEXTBOX = 17,
    WIDGETTYPE_BANG = 18,
    WIDGETTYPE_PRESS = 19,
    WIDGETTYPE_TOGGLE = 20,
    WIDGETTYPE_NUMBERBOX = 21,
    WIDGETTYPE_DIAL = 22,
    WIDGETTYPE_SLIDER = 23,
    WIDGETTYPE_SLIDER2D = 24,
    WIDGETTYPE_RANGE = 25,
    WIDGETTYPE_DROPDOWN = 26,
    WIDGETTYPE_RADIOBUTTON = 27,
    WIDGETTYPE_COLORBOX = 28,
    WIDGETTYPE_TABLE = 29,
    WIDGETTYPE_FILECHOOSER = 30,
    WIDGETTYPE_DIRECTORYCHOOSER = 31,
    WIDGETTYPE_IP = 32,
    WIDGETTYPE_LIST = 32768,
    WIDGETTYPE_LISTPAGE = 32769,
    WIDGETTYPE_TABS = 32770
};

enum rcp_command_t {
    COMMAND_INVALID = 0,
    COMMAND_INFO = 1,
    COMMAND_INITIALIZE = 2,
    COMMAND_DISCOVER = 3,
    COMMAND_UPDATE = 4,
    COMMAND_REMOVE = 5,
    COMMAND_UPDATEVALUE = 6,
    COMMAND_MAX_
};

enum rcp_number_scale_t {
    NUMBER_SCALE_LINEAR = 0,
    NUMBER_SCALE_LOGARITHMIC = 1,
    NUMBER_SCALE_EXP2 = 2
};

enum rcp_dial_options_t {
    DIAL_OPTIONS_CYCLIC = 86
};

enum rcp_range_options_t {
    RANGE_OPTIONS_DEFAULT = 48
};

enum rcp_label_position_t {
    LABEL_POSITION_LEFT = 1,
    LABEL_POSITION_RIGHT = 2,
    LABEL_POSITION_TOP = 3,
    LABEL_POSITION_BOTTOM = 4,
    LABEL_POSITION_CENTER = 5
};

enum rcp_uri_options_t {
    URI_OPTIONS_DEFAULT = 48,
    URI_OPTIONS_FILTER = 49,
    URI_OPTIONS_SCHEMA = 50
};

enum rcp_slider_options_t {
    SLIDER_OPTIONS_HORIZONTAL = 86
};

enum rcp_client_status_t {
    CLIENT_STATUS_DISCONNECTED = 0,
    CLIENT_STATUS_CONNECTED = 1,
    CLIENT_STATUS_VERSION_MISSMATCH = 2,
    CLIENT_STATUS_OK = 3
};

enum rcp_string_options_t {
    STRING_OPTIONS_DEFAULT = 48,
    STRING_OPTIONS_REGULAR_EXPRESSION = 49
};

enum rcp_infodata_options_t {
    INFODATA_OPTIONS_APPLICATIONID = 26
};

enum rcp_array_options_t {
    ARRAY_OPTIONS_DEFAULT = 48
};

enum rcp_numberbox_format_t {
    NUMBERBOX_FORMAT_DEC = 1,
    NUMBERBOX_FORMAT_HEX = 2,
    NUMBERBOX_FORMAT_BIN = 3
};

enum rcp_datatype_t {
    DATATYPE_INVALID = 0,
    DATATYPE_CUSTOMTYPE = 1,
    DATATYPE_BOOLEAN = 16,
    DATATYPE_INT8 = 17,
    DATATYPE_UINT8 = 18,
    DATATYPE_INT16 = 19,
    DATATYPE_UINT16 = 20,
    DATATYPE_INT32 = 21,
    DATATYPE_UINT32 = 22,
    DATATYPE_INT64 = 23,
    DATATYPE_UINT64 = 24,
    DATATYPE_FLOAT32 = 25,
    DATATYPE_FLOAT64 = 26,
    DATATYPE_VECTOR2I32 = 27,
    DATATYPE_VECTOR2F32 = 28,
    DATATYPE_VECTOR3I32 = 29,
    DATATYPE_VECTOR3F32 = 30,
    DATATYPE_VECTOR4I32 = 31,
    DATATYPE_VECTOR4F32 = 32,
    DATATYPE_STRING = 33,
    DATATYPE_RGB = 34,
    DATATYPE_RGBA = 35,
    DATATYPE_ENUM = 36,
    DATATYPE_ARRAY = 37,
    DATATYPE_LIST = 38,
    DATATYPE_BANG = 39,
    DATATYPE_GROUP = 40,
    DATATYPE_URI = 42,
    DATATYPE_IPV4 = 43,
    DATATYPE_IPV6 = 44,
    DATATYPE_RANGE = 45,
    DATATYPE_IMAGE = 46,
    DATATYPE_MAX_
};

enum rcp_number_options_t {
    NUMBER_OPTIONS_DEFAULT = 48,
    NUMBER_OPTIONS_MINIMUM = 49,
    NUMBER_OPTIONS_MAXIMUM = 50,
    NUMBER_OPTIONS_MULTIPLEOF = 51,
    NUMBER_OPTIONS_SCALE = 52,
    NUMBER_OPTIONS_UNIT = 53
};

enum rcp_ipv6_options_t {
    IPV6_OPTIONS_DEFAULT = 48
};

enum rcp_packet_options_t {
    PACKET_OPTIONS_TIMESTAMP = 17,
    PACKET_OPTIONS_DATA = 18
};

enum rcp_textbox_options_t {
    TEXTBOX_OPTIONS_MULTILINE = 86,
    TEXTBOX_OPTIONS_WORDWRAP = 87,
    TEXTBOX_OPTIONS_PASSWORD = 88
};

enum rcp_list_options_t {
    LIST_OPTIONS_DEFAULT = 48,
    LIST_OPTIONS_MINIMUM = 49,
    LIST_OPTIONS_MAXIMUM = 50
};

enum rcp_customwidget_options_t {
    CUSTOMWIDGET_OPTIONS_UUID = 86,
    CUSTOMWIDGET_OPTIONS_CONFIG = 87
};


enum rcp_string_types_t {
    TINY_STRING = 1,
    SHORT_STRING = 2,
    LONG_STRING = 4
};

#ifdef __cplusplus
} // extern "C"
#endif

#endif
