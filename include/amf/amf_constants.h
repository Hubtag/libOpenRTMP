/*
    amf_constants.h

    Copyright (C) 2016 Hubtag LLC.

    ----------------------------------------

    This file is part of libOpenRTMP.

    libOpenRTMP is free software: you can redistribute it and/or modify
    it under the terms of version 3 of the GNU Affero General Public License
    as published by the Free Software Foundation.

    libOpenRTMP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with libOpenRTMP. If not, see <http://www.gnu.org/licenses/>.


    ----------------------------------------

    All references in this file are relevant to the official AMF0 and AMF3 specifications.

    AMF0:
        Download: http://wwwimages.adobe.com/content/dam/Adobe/en/devnet/amf/pdf/amf0-file-format-specification.pdf
        Information Page: http://www.adobe.com/devnet/swf.html
        CRC32: f74e4446
        Date: June 29th, 2016

    AMF3:
        Download: http://wwwimages.adobe.com/content/dam/Adobe/en/devnet/amf/pdf/amf-file-format-spec.pdf
        Information Page: http://www.adobe.com/devnet/swf.html
        CRC32: 8e55cb19
        Date: June 29th, 2016

*/

#ifndef RTMP_H_AMF_CONSTANTS_H
#define RTMP_H_AMF_CONSTANTS_H

#include "rtmp/rtmp_types.h"

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#define AMF_MAX_KEY 1000

typedef enum AMF_ERR {
    AMF_ERR_NONE,
    AMF_ERR_INVALID_DATA = -10,
    AMF_ERR_INCOMPLETE,
    AMF_ERR_NEED_NAME,
    AMF_ERR_OOM,
    AMF_ERR_BAD_ALLOC
} amf_err_t;

#define AMF_SIZE(n) ((amf_err_t)(n))

//AMF0 Type Markers             See AMF0 Specification
typedef enum AMF0_TYPE {
    AMF0_TYPE_NUMBER,           //Page 5, §2.2
    AMF0_TYPE_BOOLEAN,          //Page 5, §2.3
    AMF0_TYPE_STRING,           //Page 5, §2.4
    AMF0_TYPE_OBJECT,           //Page 5, §2.5
    AMF0_TYPE_MOVIECLIP,        //Page 5, §2.6
    AMF0_TYPE_NULL,             //Page 6, §2.7
    AMF0_TYPE_UNDEFINED,        //Page 6, §2.8
    AMF0_TYPE_REFERENCE,        //Page 6, §2.9
    AMF0_TYPE_ECMA_ARRAY,       //Page 6, §2.10
    AMF0_TYPE_OBJECT_END,       //Page 6, §2.11
    AMF0_TYPE_STRICT_ARRAY,     //Page 7, §2.12
    AMF0_TYPE_DATE,             //Page 7, §2.13
    AMF0_TYPE_LONG_STRING,      //Page 7, §2.14
    AMF0_TYPE_UNSUPPORTED,      //Page 7, §2.15
    AMF0_TYPE_RECORDSET,        //Page 7, §2.16
    AMF0_TYPE_XML_DOCUMENT,     //Page 7, §2.17
    AMF0_TYPE_TYPED_OBJECT,     //Page 8, §2.18
    AMF0_TYPE_AVMPLUS,          //Page 8, §3.1
    AMF0_TYPE_NUMBER_INT,       //Not a part of the spec, for internal use
    AMF0_TYPE_NONE,
    AMF0_TYPE_COMPLEX,
    AMF0_TYPE_ECMA_ARRAY_ASSOC_END
} amf0_type_t;

//AMF3 Type Markers             See AMF3 Specification
typedef enum AMF3_TYPE {
    AMF3_TYPE_UNDEFINED,        //Page 6, §3.2
    AMF3_TYPE_NULL,             //Page 6, §3.3
    AMF3_TYPE_FALSE,            //Page 7, §3.4
    AMF3_TYPE_TRUE,             //Page 7, §3.5
    AMF3_TYPE_INTEGER,          //Page 7, §3.6
    AMF3_TYPE_DOUBLE,           //Page 7, §3.7
    AMF3_TYPE_STRING,           //Page 7, §3.8
    AMF3_TYPE_XML_DOCUMENT,     //Page 8, §3.9
    AMF3_TYPE_DATE,             //Page 8, §3.10
    AMF3_TYPE_ARRAY,            //Page 9, §3.11
    AMF3_TYPE_OBJECT,           //Page 9, §3.12
    AMF3_TYPE_XML,              //Page 11, §3.13
    AMF3_TYPE_BYTE_ARRAY,       //Page 11, §3.14
    AMF3_TYPE_VECTOR_INT,       //Page 12, §3.15
    AMF3_TYPE_VECTOR_UINT,      //Page 12, §3.15
    AMF3_TYPE_VECTOR_DOUBLE,    //Page 12, §3.15
    AMF3_TYPE_VECTOR_OBJECT,    //Page 12, §3.15
    AMF3_TYPE_DICTIONARY        //Page 13, §3.16
} amf3_type_t;

//AMF Object Type Markers
typedef enum AMF_TYPE {
    AMF_TYPE_UNDEFINED,
    AMF_TYPE_NULL,
    AMF_TYPE_BOOLEAN,
    AMF_TYPE_INTEGER,
    AMF_TYPE_DOUBLE,
    AMF_TYPE_DATE,
    AMF_TYPE_STRING,
    AMF_TYPE_XML_DOCUMENT,
    AMF_TYPE_OBJECT,
    AMF_TYPE_TYPED_OBJECT,
    AMF_TYPE_OBJECT_END,
    AMF_TYPE_ARRAY,
    AMF_TYPE_VECTOR_INT,
    AMF_TYPE_VECTOR_UINT,
    AMF_TYPE_VECTOR_DOUBLE,
    AMF_TYPE_VECTOR_OBJECT,
    AMF_TYPE_BYTE_ARRAY,
    AMF_TYPE_MOVIECLIP,
    AMF_TYPE_RECORDSET,
    AMF_TYPE_REFERENCE,
    AMF_TYPE_UNSUPPORTED,
    AMF_TYPE_AVMPLUS,
    AMF_TYPE_NONE,
    AMF_TYPE_COMPLEX,
    AMF_TYPE_ECMA_ARRAY_ASSOC_END,
    AMF_TYPE_INTEGER24,
    AMF_TYPE_INTEGER16,
    AMF_TYPE_LONG_STRING,
} amf_type_t;


#define AMF_SIZE_AMF_TYPE_UNDEFINED 0
#define AMF_SIZE_AMF_TYPE_NULL 0
#define AMF_SIZE_AMF_TYPE_BOOLEAN 1
#define AMF_SIZE_AMF_TYPE_INTEGER 4
#define AMF_SIZE_AMF_TYPE_INTEGER16 2
#define AMF_SIZE_AMF_TYPE_INTEGER24 3
#define AMF_SIZE_AMF_TYPE_DOUBLE 8
#define AMF_SIZE_AMF_TYPE_UNSUPPORTED 0
#define AMF_SIZE_AMF_TYPE_STRING(amf_len) (2+amf_len)
#define AMF_SIZE_AMF_TYPE_LONG_STRING(amf_len) (4+amf_len)


#define AMF_PASTE2(A,B) A ## B
#define AMF_PASTE(A,B) AMF_PASTE2(A,B)

#define AMF_SIZE_OF(A) AMF_PASTE(AMF_SIZE_, A)

#define AMF_SIZES_0() 0
#define AMF_SIZES_1(A, B) AMF_SIZE_OF(A)
#define AMF_SIZES_2(A, B, C, D) (AMF_SIZES_1(A,B) + AMF_SIZES_1(C,D))
#define AMF_SIZES_3(A, B, C, D, E, F) (AMF_SIZES_1(A,B) + AMF_SIZES_2(C,D,E,F))
#define AMF_SIZES_INTERNAL( x, A, B, C, D, E, F, FUNC, ... ) (FUNC)
#define AMF_SIZES(...) AMF_SIZES_INTERNAL(  __VA_ARGS__,                \
                                            AMF_SIZES_3( __VA_ARGS__ ), \
                                            AMF_SIZES_3( __VA_ARGS__ ), \
                                            AMF_SIZES_2( __VA_ARGS__ ), \
                                            AMF_SIZES_2( __VA_ARGS__ ), \
                                            AMF_SIZES_1( __VA_ARGS__ ), \
                                            AMF_SIZES_1( __VA_ARGS__ ), \
                                            AMF_SIZES_0(             ), \
                                            AMF_SIZES_0(             ) )


#define AMF_WRITE_AMF_TYPE_UNDEFINED(data,offset,val) do{\
}while(0)

#define AMF_WRITE_AMF_TYPE_NULL(data,offset,val) do{\
}while(0)

#define AMF_WRITE_AMF_TYPE_BOOLEAN(data,offset,val) do{\
    data[offset]=val;\
    offset += AMF_SIZE_AMF_TYPE_BOOLEAN; \
}while(0)

#define AMF_WRITE_AMF_TYPE_INTEGER(data,offset,val) do{\
    ntoh_write_ud( data + offset, val ); \
    offset += AMF_SIZE_AMF_TYPE_INTEGER;\
}while(0)

#define AMF_WRITE_AMF_TYPE_INTEGER16(data,offset,val) do{\
    ntoh_write_s( data + offset, val ); \
    offset += AMF_SIZE_AMF_TYPE_INTEGER16;\
}while(0)

#define AMF_WRITE_AMF_TYPE_INTEGER24(data,offset,val) do{\
    ntoh_write_ud3( data + offset, val ); \
    offset += AMF_SIZE_AMF_TYPE_INTEGER24;\
}while(0)

#define AMF_WRITE_AMF_TYPE_DOUBLE(data,offset,val) do{\
    byte flipped[8];\
    write_double_ieee( flipped, val );\
    ntoh_memcpy(data + offset, flipped, 8);\
    offset+=AMF_SIZE_AMF_TYPE_DOUBLE;\
}while(0)

#define AMF_WRITE_AMF_TYPE_UNSUPPORTED(data,offset,val) do{\
}while(0)

#define AMF_WRITE_AMF_TYPE_STRING2(data,offset,val) do{\
    ntoh_write_us( data + offset, AMF_LEN ); \
    offset += AMF_SIZE_AMF_TYPE_INTEGER16;\
    memcpy(data+offset,val,AMF_LEN); \
    offset+=AMF_LEN;\
}while(0)

#define AMF_WRITE_AMF_TYPE_STRING(len) size_t AMF_LEN = len; AMF_WRITE_AMF_TYPE_STRING2

#define AMF_WRITE_AMF_TYPE_LONG_STRING2(data,offset,val) do{\
    ntoh_write_ud( data + offset, AMF_LEN ); \
    offset += AMF_SIZE_AMF_TYPE_INTEGER;\
    memcpy(data+offset,val,AMF_LEN); \
    offset+=AMF_LEN;\
}while(0)

#define AMF_WRITE_AMF_TYPE_LONG_STRING(len) size_t AMF_LEN = len; AMF_WRITE_AMF_TYPE_LONG_STRING2

#define AMF_WRITE_0(data) do{\
    AMF_OFFSET_VALUE += 0;\
}while(0)

#define AMF_WRITE_1(data,A,B) do{\
    AMF_PASTE(AMF_WRITE_,A)(data,AMF_OFFSET_VALUE,B);\
}while(0)

#define AMF_WRITE_2(data,A,B,C,D) do{\
    AMF_WRITE_1(data,A,B);\
    AMF_WRITE_1(data,C,D);\
}while(0)

#define AMF_WRITE_3(data,A,B,C,D,E,F) do{\
    AMF_WRITE_1(data,A,B);\
    AMF_WRITE_2(data,C,D,E,F);\
}while(0)

#define AMF_WRITE_INTERNAL(x, A, B, C, D, E, F, FUNC, ...) do{ \
    size_t AMF_OFFSET_VALUE = 0; \
    FUNC; \
}while(0)

#define AMF_WRITE(data,...) AMF_WRITE_INTERNAL(  __VA_ARGS__,                   \
                                            AMF_WRITE_3( (data),__VA_ARGS__ ),  \
                                            AMF_WRITE_3( (data),__VA_ARGS__ ),  \
                                            AMF_WRITE_2( (data),__VA_ARGS__ ),  \
                                            AMF_WRITE_2( (data),__VA_ARGS__ ),  \
                                            AMF_WRITE_1( (data),__VA_ARGS__ ),  \
                                            AMF_WRITE_1( (data),__VA_ARGS__ ),  \
                                            AMF_WRITE_0( (data)             ),  \
                                            AMF_WRITE_0( (data)             ) )

#define AMF0_DESCRIBE_ENCODE( data, data_len, type, ... )  \
do{                                                 \
    bool offset = type == AMF0_TYPE_NONE ? 0 : 1;   \
    size_t len = AMF_SIZES( __VA_ARGS__ ) + offset; \
    if( data == nullptr ){                          \
        return AMF_SIZE(len);                       \
    }                                               \
    if( data_len < len ){                           \
        return AMF_ERR_INCOMPLETE;                  \
    }                                               \
    if( offset > 0 ){                               \
        data[0] = type;                             \
    }                                               \
    AMF_WRITE(data+offset, __VA_ARGS__);            \
    return AMF_SIZE(len);                           \
} while(0)

#define AMF_READ_AMF_TYPE_UNDEFINED(data,offset,val) do{\
}while(0)

#define AMF_READ_AMF_TYPE_NULL(data,offset,val) do{\
}while(0)

#define AMF_READ_AMF_TYPE_BOOLEAN(data,offset,val) do{\
    val = data[offset];\
    offset += AMF_SIZE_AMF_TYPE_BOOLEAN;\
}while(0)

#define AMF_READ_AMF_TYPE_INTEGER(data,offset,val) do{\
    val = ntoh_read_ud( data + offset ); \
    offset += AMF_SIZE_AMF_TYPE_INTEGER;\
}while(0)

#define AMF_READ_AMF_TYPE_INTEGER16(data,offset,val) do{\
    val = ntoh_read_s( data + offset );\
    offset += AMF_SIZE_AMF_TYPE_INTEGER16;\
}while(0)

#define AMF_READ_AMF_TYPE_INTEGER24(data,offset,val) do{\
    val = ntoh_read_ud3( data + offset );\
    offset += AMF_SIZE_AMF_TYPE_INTEGER24;\
}while(0)

#define AMF_READ_AMF_TYPE_DOUBLE(data,offset,val) do{\
    byte flipped[8];\
    ntoh_memcpy(flipped, data + offset, AMF_SIZE_AMF_TYPE_DOUBLE);\
    val = read_double_ieee( flipped );\
    offset+=AMF_SIZE_AMF_TYPE_DOUBLE;\
}while(0)

#define AMF_READ_AMF_TYPE_UNSUPPORTED(data,offset,val) do{\
}while(0)
#include <stdio.h>
#define AMF_READ_AMF_TYPE_STRING2(data,offset,val) do{\
    size_t len = ntoh_read_us( data + offset ); \
    offset += AMF_SIZE_AMF_TYPE_INTEGER16;\
    memcpy(val,data+offset,MIN(len,AMF_BUFLEN)); \
    offset+=len;\
}while(0)

#define AMF_READ_AMF_TYPE_STRING(buflen) size_t AMF_BUFLEN = buflen; AMF_READ_AMF_TYPE_STRING2

#define AMF_READ_AMF_TYPE_LONG_STRING2(data,offset,val) do{\
    size_t len = ntoh_read_ud( data + offset ); \
    offset += AMF_SIZE_AMF_TYPE_INTEGER;\
    memcpy(val,data+offset,MIN(len,AMF_BUFLEN)); \
    offset+=len;\
}while(0)

#define AMF_READ_AMF_TYPE_LONG_STRING(buflen) size_t AMF_BUFLEN = buflen; AMF_READ_AMF_TYPE_LONG_STRING2

#define AMF_READ_0(data) do{\
    AMF_OFFSET_VALUE += 0;\
}while(0)

#define AMF_READ_1(data,A,B) do{\
    AMF_PASTE(AMF_READ_,A)(data,AMF_OFFSET_VALUE,B);\
}while(0)

#define AMF_READ_2(data,A,B,C,D) do{\
    AMF_READ_1(data,A,B);\
    AMF_READ_1(data,C,D);\
}while(0)

#define AMF_READ_3(data,A,B,C,D,E,F) do{\
    AMF_READ_1(data,A,B);\
    AMF_READ_2(data,C,D,E,F);\
}while(0)

#define AMF_READ_INTERNAL(x, A, B, C, D, E, F, FUNC, ...) do{ \
    size_t AMF_OFFSET_VALUE = 0; \
    FUNC; \
}while(0)

#define AMF_READ(data,...) AMF_READ_INTERNAL(  __VA_ARGS__,                    \
                                            AMF_READ_3( (data),__VA_ARGS__ ),  \
                                            AMF_READ_3( (data),__VA_ARGS__ ),  \
                                            AMF_READ_2( (data),__VA_ARGS__ ),  \
                                            AMF_READ_2( (data),__VA_ARGS__ ),  \
                                            AMF_READ_1( (data),__VA_ARGS__ ),  \
                                            AMF_READ_1( (data),__VA_ARGS__ ),  \
                                            AMF_READ_0( (data)             ),  \
                                            AMF_READ_0( (data)             ) )


#define AMF0_DESCRIBE_DECODE_SIZE( type, ... ) (AMF_SIZES( __VA_ARGS__ ) + (type == AMF0_TYPE_NONE ? 0 : 1))

#define AMF0_DESCRIBE_DECODE_I( count, data, data_len, type, ... )  \
do{                                                 \
    bool offset = type == AMF0_TYPE_NONE ? 0 : 1;   \
    size_t len = AMF0_DESCRIBE_DECODE_SIZE(type, __VA_ARGS__); \
    byte buffer[AMF0_DESCRIBE_DECODE_SIZE(type, __VA_ARGS__)];\
    if( data_len < len ){ \
        return AMF_ERR_INCOMPLETE; \
    } \
    memcpy(buffer, data, len); \
    count=len;\
    if( type != AMF0_TYPE_NONE && *buffer != type ){ \
        return AMF_ERR_INVALID_DATA; \
    }\
    AMF_READ(data+offset, __VA_ARGS__);            \
} while(0)

#define AMF0_DESCRIBE_DECODE( data, data_len, type, ... )   \
do{                        \
    size_t count = 0;                            \
    AMF0_DESCRIBE_DECODE_I(count, data, data_len, type, __VA_ARGS__); \
    return AMF_SIZE(count);                           \
} while(0)

#define AMF0_DESCRIBE_DECODE_PEEK( data, data_len, type, ... ) do{      \
    size_t count = 0;                                                   \
    AMF0_DESCRIBE_DECODE_I(count, data, data_len, type, __VA_ARGS__);   \
    return AMF_SIZE(0);                                                 \
} while(0)



#define AMF_ARG_AMF_TYPE_UNDEFINED(name)
#define AMF_ARG_AMF_TYPE_NULL(name)
#define AMF_ARG_AMF_TYPE_BOOLEAN(name) , int name
#define AMF_ARG_AMF_TYPE_INTEGER(name) , int name
#define AMF_ARG_AMF_TYPE_INTEGER16(name) , short name
#define AMF_ARG_AMF_TYPE_INTEGER24(name) , int name
#define AMF_ARG_AMF_TYPE_DOUBLE(name) , double name
#define AMF_ARG_AMF_TYPE_UNSUPPORTED(name)
#define AMF_ARG_AMF_TYPE_STRING(name) , void* name, size_t AMF_PASTE(name, _len)
#define AMF_ARG_AMF_TYPE_LONG_STRING(name) , void* name, size_t AMF_PASTE(name, _len)

#define AMF_DUMMY_AMF_TYPE_UNDEFINED(name) int name = 0
#define AMF_DUMMY_AMF_TYPE_NULL(name) int name = 0
#define AMF_DUMMY_AMF_TYPE_BOOLEAN(name)
#define AMF_DUMMY_AMF_TYPE_INTEGER(name)
#define AMF_DUMMY_AMF_TYPE_INTEGER16(name)
#define AMF_DUMMY_AMF_TYPE_INTEGER24(name)
#define AMF_DUMMY_AMF_TYPE_DOUBLE(name)
#define AMF_DUMMY_AMF_TYPE_UNSUPPORTED(name) int name = 0
#define AMF_DUMMY_AMF_TYPE_STRING(name)
#define AMF_DUMMY_AMF_TYPE_LONG_STRING(name)

#define AMF_PASS_AMF_TYPE_UNDEFINED(name) AMF_TYPE_UNDEFINED, name
#define AMF_PASS_AMF_TYPE_NULL(name) AMF_TYPE_NULL, name
#define AMF_PASS_AMF_TYPE_BOOLEAN(name) AMF_TYPE_BOOLEAN, name
#define AMF_PASS_AMF_TYPE_INTEGER(name) AMF_TYPE_INTEGER, name
#define AMF_PASS_AMF_TYPE_INTEGER16(name) AMF_TYPE_INTEGER16, name
#define AMF_PASS_AMF_TYPE_INTEGER24(name) AMF_TYPE_INTEGER24, name
#define AMF_PASS_AMF_TYPE_DOUBLE(name) AMF_TYPE_DOUBLE, name
#define AMF_PASS_AMF_TYPE_UNSUPPORTED(name) AMF_TYPE_UNSUPPORTED, name
#define AMF_PASS_AMF_TYPE_STRING(name) AMF_TYPE_STRING(name_len), name
#define AMF_PASS_AMF_TYPE_LONG_STRING(name) AMF_TYPE_LONG_STRING(name_len), name

#define AMF_ARG_0( data, data_len, type ) ) { \
    AMF0_DESCRIBE_ENCODE( data, data_len, type );\
}

#define AMF_ARG_1(data, data_len, type, A) \
AMF_PASTE(AMF_ARG_, A)(argA) ) {\
    AMF_PASTE(AMF_DUMMY_,A)(argA);    \
    AMF0_DESCRIBE_ENCODE( data, data_len, type, \
                            AMF_PASTE(AMF_PASS_,A)(argA));\
}

#define AMF_ARG_2(data, data_len, type, A,B) \
AMF_PASTE(AMF_ARG_, A)(argA),    \
AMF_PASTE(AMF_ARG_, B)(argB) ) {\
    AMF_PASTE(AMF_DUMMY_,A)(argA);    \
    AMF_PASTE(AMF_DUMMY_,B)(argB);    \
    AMF0_DESCRIBE_ENCODE( data, data_len, type, \
                            AMF_PASTE(AMF_PASS_,A)(argA),\
                            AMF_PASTE(AMF_PASS_,B)(argB));\
}

#define AMF_ARG_3(data, data_len, type, A,B,C) \
AMF_PASTE(AMF_ARG_, A)(argA),    \
AMF_PASTE(AMF_ARG_, B)(argB),    \
AMF_PASTE(AMF_ARG_, C)(argC) ) {\
    AMF_PASTE(AMF_DUMMY_,A)(argA);    \
    AMF_PASTE(AMF_DUMMY_,B)(argB);    \
    AMF_PASTE(AMF_DUMMY_,C)(argC);    \
    AMF0_DESCRIBE_ENCODE( data, data_len, type, \
                            AMF_PASTE(AMF_PASS_,A)(argA),\
                            AMF_PASTE(AMF_PASS_,B)(argB),\
                            AMF_PASTE(AMF_PASS_,C)(argC));\
}

#define AMF_ARG_INTERNAL(A, B, C, FUNC, ...) FUNC

#define AMF_ARG(data, data_len, type,...) AMF_ARG_INTERNAL(  __VA_ARGS__,              \
                                            AMF_ARG_3( data, data_len, type, __VA_ARGS__ ),  \
                                            AMF_ARG_2( data, data_len, type, __VA_ARGS__ ),  \
                                            AMF_ARG_1( data, data_len, type, __VA_ARGS__ ),  \
                                            AMF_ARG_0( data, data_len, type              ) )
#define AMF0_DESCRIBE( name, type, ... )\
amf_err_t AMF_PASTE( amf0_write_, name ) (byte * data, size_t data_len AMF_ARG(data, data_len, type, __VA_ARGS__)

/*
//Returns an IEEE 754 float from the data
amf_err_t amf0_get_number( const byte* data, size_t data_len, double *value ){
    size_t count = 0;
    AMF0_HARVEST_LENGTH(data, data_len, count, 9);
    AMF0_CHECK_TYPE( buffer, AMF0_TYPE_NUMBER, AMF_ERR_INVALID_DATA );
    byte flipped[8];
    ntoh_memcpy( flipped, buffer + 1, 8 );
    *value = read_double_ieee( flipped );
    return 9;
}

amf_err_t amf0_get_boolean( const byte* data, size_t data_len, int *value ){
    size_t count = 0;
    AMF0_HARVEST_LENGTH(data, data_len, count, 2);
    AMF0_CHECK_TYPE( buffer, AMF0_TYPE_BOOLEAN, AMF_ERR_INVALID_DATA );
    *value = buffer[1];
    return 2;
}

//String functions are used for normal and long strings, as well as XML documents
//Used to verify how much storage to allocate for the upcoming string.
amf_err_t amf0_get_string_length( const byte* data, size_t data_len, size_t *value ){
    if( data_len < 3 ){
        return AMF_ERR_INCOMPLETE;
    }
    if( *data == AMF0_TYPE_STRING ){
        *value = ntoh_read_us(data + 1);
        return 3;
    }
    if( data_len < 5 ){
        return AMF_ERR_INCOMPLETE;
    }
    if( *data == AMF0_TYPE_LONG_STRING || *data == AMF0_TYPE_XML_DOCUMENT ){
        *value = ntoh_read_ud(data + 1);
        return 5;
    }
    return AMF_ERR_INVALID_DATA;
}

//String functions are used for normal and long strings, as well as XML documents
amf_err_t amf0_get_string( const byte* data, size_t data_len, void *value, size_t value_len, size_t *continuation ){
    size_t len;
    int offset = amf0_get_string_length(data, data_len, &len);
    if( offset < 0 ){
        return offset;
    }
    if( (size_t)offset >= data_len ){
        return 0;
    }
    value_len = len > value_len ? value_len : len;
    value_len = data_len - offset > value_len ? value_len : data_len - offset;
    memcpy( value, data + offset, value_len );
    if( continuation ){
        *continuation = value_len;
    }
    return offset + value_len;
}

//Mostly a dummy; this is used to verify and skip an object start marker
amf_err_t amf0_get_object( const byte* data, size_t data_len ){
    size_t count = 0;
    AMF0_HARVEST_LENGTH(data, data_len, count, 1);
    AMF0_CHECK_TYPE( buffer, AMF0_TYPE_OBJECT, AMF_ERR_INVALID_DATA );
    return 1;
}

//If inside an object, use this to obtain the length of a property name
amf_err_t amf0_get_prop_length( const byte* data, size_t data_len, size_t *value ){
    AMF0_PEEK_LENGTH(data, data_len, 2);
    *value = ntoh_read_us(buffer);
    return 0;
}

//If inside an object, use this to obtain a copy of the property name
amf_err_t amf0_get_prop_name( const byte* data, size_t data_len, void *value, size_t value_len ){
    size_t count = 0;
    AMF0_HARVEST_LENGTH(data, data_len, count, 2);
    size_t len = ntoh_read_us(buffer);

    value_len = len > value_len ? value_len : len;
    value_len = data_len - 2 > value_len ? value_len : data_len - 2;

    memcpy( value, data + 2, value_len );
    return 2 + value_len;
}

//Dummy; do not use.
amf_err_t amf0_get_movieclip( const byte* data, size_t data_len ){
    emit_err("[Erroneous Data] Trying to read movieclip from AMF!");
    return AMF_ERR_INVALID_DATA;
}

//Basically a dummy; used to verify that the next item is indeed a null value.
amf_err_t amf0_get_null( const byte* data, size_t data_len ){
    size_t count = 0;
    AMF0_HARVEST_LENGTH(data, data_len, count, 1);
    AMF0_CHECK_TYPE( buffer, AMF0_TYPE_NULL, AMF_ERR_INVALID_DATA );
    return 1;
}


//Basically a dummy; used to verify that the next item is indeed an undefined value.
amf_err_t amf0_get_undefined( const byte* data, size_t data_len ){
    size_t count = 0;
    AMF0_HARVEST_LENGTH(data, data_len, count, 1);
    AMF0_CHECK_TYPE( buffer, AMF0_TYPE_UNDEFINED, AMF_ERR_INVALID_DATA );
    return 1;
}

amf_err_t amf0_get_reference( const byte* data, size_t data_len, uint32_t *value){
    size_t count = 0;
    AMF0_HARVEST_LENGTH(data, data_len, count, 3);
    AMF0_CHECK_TYPE( buffer, AMF0_TYPE_REFERENCE, AMF_ERR_INVALID_DATA );
    *value = ntoh_read_us(buffer + 1);
    return 3;
}

amf_err_t amf0_get_ecma_array( const byte* data, size_t data_len, uint32_t *num_memb ){
    size_t count = 0;
    AMF0_HARVEST_LENGTH(data, data_len, count, 5);
    AMF0_CHECK_TYPE( buffer, AMF0_TYPE_ECMA_ARRAY, AMF_ERR_INVALID_DATA );
    *num_memb = ntoh_read_ud(buffer + 1);
    return 5;
}

//Mostly a dummy; this is used to verify and skip an object end marker
amf_err_t amf0_get_object_end( const byte* data, size_t data_len ){
    size_t count = 0;
    AMF0_HARVEST_LENGTH(data, data_len, count, 1);
    AMF0_CHECK_TYPE( buffer, AMF0_TYPE_OBJECT_END, AMF_ERR_INVALID_DATA );
    return 1;
}

//Unimplemented. Will implement if necessary.
amf_err_t amf0_get_strict_array( const byte* data, size_t data_len ){
    emit_err("[Unimplemented] Trying to read Strict Array from AMF!");
    return 1;
}

//Returns a timezone offset as well as a double essentially representing a Unix timestamp
//(Resolution is 1:1 with seconds, epoch is 1970 Jan 1 00:00:00.000)
amf_err_t amf0_get_date( const byte* data, size_t data_len, int* timezone, double* timestamp ){
    size_t count = 0;
    AMF0_HARVEST_LENGTH(data, data_len, count, 11);
    AMF0_CHECK_TYPE( buffer, AMF0_TYPE_DATE, AMF_ERR_INVALID_DATA );
    *timezone = ntoh_read_s(buffer+1);
    byte temp[8];
    ntoh_memcpy(temp, buffer + 3, 8);
    *timestamp = read_double_ieee(temp);
    return 11;
}

//Dummy
amf_err_t amf0_get_unsupported( const byte* data, size_t data_len ){
    emit_err("[Error] Trying to read an unsupported type from AMF!");
    return 1;
}

//Alias around amf0_get_string_length
amf_err_t amf0_get_long_string_length( const byte* data, size_t data_len, size_t *value){
    return amf0_get_string_length(data, data_len, value);
}
//Alias around amf0_get_string
amf_err_t amf0_get_long_string( const byte* data, size_t data_len, void *value, size_t value_len, size_t *continuation){
    return amf0_get_string(data, data_len, value, value_len, continuation);
}

//Alias around amf0_get_string_length
amf_err_t amf0_get_xmldocument_length( const byte* data, size_t data_len, size_t *value){
    return amf0_get_string_length(data, data_len, value);
}
//Alias around amf0_get_string
amf_err_t amf0_get_xmldocument( const byte* data, size_t data_len, void *value, size_t value_len, size_t *continuation){
    return amf0_get_string(data, data_len, value, value_len, continuation);
}

//Unimplemented. Will implement if necessary.
amf_err_t amf0_get_recordset( const byte* data, size_t data_len ){
    emit_err("[Unimplemented] Trying to read Record Set from AMF!");
    return 1;
}
//Unimplemented. Will implement if necessary.
amf_err_t amf0_get_typed_object( const byte* data, size_t data_len ){
    emit_err("[Unimplemented] Trying to read Typed Object from AMF!");
    return 1;
}
amf_err_t amf0_get_continue( const byte* data, size_t data_len, void *value, size_t value_len, size_t *continuation ){
    value_len = value_len <= data_len ? value_len : data_len;
    value_len = value_len < *continuation ? value_len : *continuation;
    memcpy( value, data, value_len );
    *continuation -= value_len;
    return value_len;
}*/

#endif
