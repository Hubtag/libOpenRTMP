/*
    amf_object.c

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

*/

#include "amf/amf_object.h"
#include "amf/amf.h"
#include "memutil.h"
#include <stdlib.h>
#include <string.h>

struct amf_value{
    unsigned char type;
};
typedef union amf_v amf_v_t;

typedef struct amf_v_number{
    unsigned char type;
    double number;
} amf_v_number_t;

typedef struct amf_v_boolean{
    unsigned char type;
    bool boolean;
} amf_v_boolean_t;

typedef struct amf_v_string{
    unsigned char type;
    size_t length;
    char *data;
} amf_v_string_t;



typedef struct amf_v_null{
    unsigned char type;
} amf_v_null_t;

typedef struct amf_v_undefined{
    unsigned char type;
} amf_v_undefined_t;

typedef struct amf_v_reference{
    unsigned char type;
    size_t ref_num;
    amf_v_t *ref;
} amf_v_reference_t;

typedef struct amf_v_date{
    unsigned char type;
    double timestamp;
    signed char timezone;
} amf_v_date_t;

typedef struct amf_v_member amf_v_member_t;

typedef struct amf_v_object{
    unsigned char type;
    uint32_t write_offset;
    VEC_DECLARE(amf_v_member_t) members;
} amf_v_object_t;

union amf_v{
    amf_v_number_t number;
    amf_v_boolean_t boolean;
    amf_v_string_t string;
    amf_v_object_t object;
    amf_v_null_t null;
    amf_v_undefined_t undefined;
    amf_v_reference_t reference;
    amf_v_date_t date;
    unsigned char type;
    struct amf_value value;
};

struct amf_v_member{
    size_t length;
    char *name;
    amf_v_t value;
};

struct amf_object{
    char type;
    size_t depth;
    VEC_DECLARE(amf_v_t) items;

    VEC_DECLARE(amf_v_t*) ref_table;

    void *allocation;
    size_t allocation_len;

    bool member_ready;
};


void amf_free_value( amf_v_t * val){
    switch( val->type ){
        case AMF0_TYPE_AVMPLUS:
        case AMF0_TYPE_BOOLEAN:
        case AMF0_TYPE_DATE:
        case AMF0_TYPE_MOVIECLIP:
        case AMF0_TYPE_NULL:
        case AMF0_TYPE_NUMBER:
        case AMF0_TYPE_RECORDSET:
        case AMF0_TYPE_REFERENCE:
        case AMF0_TYPE_UNDEFINED:
        case AMF0_TYPE_UNSUPPORTED:

        //Complex but not supported
        case AMF0_TYPE_ECMA_ARRAY:
        case AMF0_TYPE_STRICT_ARRAY:
        case AMF0_TYPE_TYPED_OBJECT:
            break;

        case AMF0_TYPE_STRING:
        case AMF0_TYPE_LONG_STRING:
        case AMF0_TYPE_XML_DOCUMENT:
            free( val->string.data );
            break;
        case AMF0_TYPE_OBJECT:
            for( int i = 0; i < VEC_SIZE(val->object.members); ++i ){
                free( VEC_AT(val->object.members, i).name );
                amf_free_value( &VEC_AT(val->object.members, i).value );
            }
            VEC_DESTROY( val->object.members );
            break;
        default:
            break;
    }
}

amf_t amf_create( char type ){
    amf_t ret = calloc( 1, sizeof( struct amf_object ) );
    VEC_INIT(ret->items);
    VEC_INIT(ret->ref_table);
    return ret;
}

void amf_destroy( amf_t amf ){
    for( int i = 0; i < VEC_SIZE(amf->items); ++i ){
        amf_free_value( &amf->items[i] );
    }
    VEC_DESTROY( amf->items );
    VEC_DESTROY( amf->ref_table );
    free( amf->allocation );
}

amf_err_t amf_write_value( amf_value_t value, byte *dest, size_t size ){
    signed char temp_b;
    double temp_d;
    size_t temp_lu;
    const char *temp_pc;
    int amt = 0;
    int amt2 = 0;
    amf_v_t *val = (amf_v_t*) value;


    switch( val->type ){
        case AMF0_TYPE_AVMPLUS:
            return amf0_write_unsupported( dest, size );
        case AMF0_TYPE_BOOLEAN:
            return amf0_write_boolean( dest, size, amf_value_get_bool( value ) );
        case AMF0_TYPE_DATE:
            temp_d = amf_value_get_date( value, &temp_b );
            return amf0_write_date( dest, size, temp_b, temp_d );
        case AMF0_TYPE_MOVIECLIP:
            return amf0_write_movieclip( dest, size );
        case AMF0_TYPE_NULL:
            return amf0_write_null( dest, size );
        case AMF0_TYPE_NUMBER:
            return amf0_write_number( dest, size, amf_value_get_number( value ) );
        case AMF0_TYPE_RECORDSET:
            return amf0_write_recordset( dest, size );
        case AMF0_TYPE_REFERENCE:
            return amf0_write_reference( dest, size, amf_value_get_ref( value ) );
        case AMF0_TYPE_UNDEFINED:
            return amf0_write_undefined( dest, size );
        case AMF0_TYPE_UNSUPPORTED:
            return amf0_write_unsupported( dest, size );

        case AMF0_TYPE_ECMA_ARRAY:
            return amf0_write_ecma_array( dest, size );
        case AMF0_TYPE_STRICT_ARRAY:
            return amf0_write_strict_array( dest, size );
        case AMF0_TYPE_TYPED_OBJECT:
            return amf0_write_typed_object( dest, size );

        case AMF0_TYPE_STRING:
            temp_pc = amf_value_get_string( value, &temp_lu );
            return amf0_write_string( dest, size, temp_pc, temp_lu );
        case AMF0_TYPE_LONG_STRING:
            temp_pc = amf_value_get_string( value, &temp_lu );
            return amf0_write_long_string( dest, size, temp_pc, temp_lu );
        case AMF0_TYPE_XML_DOCUMENT:
            temp_pc = amf_value_get_string( value, &temp_lu );
            return amf0_write_xmldocument( dest, size, temp_pc, temp_lu );
        case AMF0_TYPE_OBJECT:
            amt = amf0_write_object( dest, size );
            if( amt < 0 ){
                return amt;
            }
            if( dest ){
                dest += amt;
                size -= amt;
            }

            for( int i = 0; i < VEC_SIZE(val->object.members); ++i ){
                amt2 = amf0_write_prop_name( dest, size, VEC_AT(val->object.members, i).name, VEC_AT(val->object.members, i).length );
                if( amt2 < 0 ){
                    return amt2;
                }
                amt += amt2;
                if( dest ){
                    dest += amt2;
                    size -= amt2;
                }
                amt2 = amf_write_value( &VEC_AT(val->object.members, i).value.value, dest, size );
                if( amt2 < 0 ){
                    return amt2;
                }
                amt += amt2;
                if( dest ){
                    dest += amt2;
                    size -= amt2;
                }
            }
            amt2 = amf0_write_prop_name( dest, size, "", 0 );
            if( amt2 < 0 ){
                return amt2;
            }
            if( dest ){
                dest += amt2;
                size -= amt2;
            }
            amt += amt2;
            amt2 = amf0_write_object_end( dest, size );
            if( amt2 < 0 ){
                return amt2;
            }
            if( dest ){
                dest += amt2;
                size -= amt2;
            }
            amt += amt2;
            return amt;
        default:
            break;
    }
    return AMF_ERR_NONE;
}

amf_err_t amf_write( amf_t amf, byte *dest, size_t size, size_t *written ){
    size_t offset = 0;
    size_t total_len = 0;
    size_t i;

    if( written ){
        i = *written;
    }
    for( i = 0; i < VEC_SIZE(amf->items); ++i ){
        if( dest && offset > size ){
            break;
        }
        int result = amf_write_value( &amf->items[i].value, dest + offset, size - offset );
        if( result < 0 ){
            goto end;
        }
        total_len += result;
        if( dest ){
            offset = total_len;
        }
    }
    end:
    if( written ){
        *written = i;
    }
    return total_len;
}

amf_err_t amf_read( amf_t amf, const byte *src, size_t size, size_t *read ){
    size_t offset = 0;
    void *buffer = nullptr;
    amf_err_t result = AMF_ERR_NONE;
    int temp_d;
    uint32_t temp_d32;
    double temp_f;
    size_t temp_size;
    while( offset < size ){
        if( amf->depth > 0 ){
            if( ( result = amf0_get_prop_length( src + offset, size - offset, &temp_size )) < 0 ){
                goto aborted;
            }
            if( (result = amf_push_string_alloc( amf, &buffer, temp_size )) < 0 ){
                goto aborted;
            }
            if( (result = amf0_get_prop_name(src + offset, size - offset, buffer, temp_size ) ) < 0 ){
                goto aborted;
            }
            offset += result;
            if( (result = amf_push_member( amf, buffer ) ) < 0 ){
                goto aborted;
            }

        }
        amf0_type_t type = amf0_next_type(src + offset, size - offset);
        result = AMF_ERR_NONE;
        switch( type ){
            case AMF0_TYPE_AVMPLUS:
                break;
            case AMF0_TYPE_BOOLEAN:
                result = amf0_get_boolean( src + offset, size - offset, &temp_d );
                break;
            case AMF0_TYPE_DATE:
                result = amf0_get_date( src + offset, size - offset, &temp_d, &temp_f );
                break;
            case AMF0_TYPE_ECMA_ARRAY:
                result = amf0_get_ecma_array( src + offset, size - offset );
                break;
            case AMF0_TYPE_LONG_STRING:
                amf0_get_long_string_length( src + offset, size - offset, &temp_size );
                break;
            case AMF0_TYPE_MOVIECLIP:
                result = amf0_get_movieclip( src + offset, size - offset );
                break;
            case AMF0_TYPE_NULL:
                result = amf0_get_null( src + offset, size - offset );
                break;
            case AMF0_TYPE_NUMBER:
                result = amf0_get_number( src + offset, size - offset, &temp_f );
                break;
            case AMF0_TYPE_OBJECT:
                result = amf0_get_object( src + offset, size - offset );
                break;
            case AMF0_TYPE_OBJECT_END:
                result = amf0_get_object_end( src + offset, size - offset );
                break;
            case AMF0_TYPE_RECORDSET:
                result = amf0_get_recordset( src + offset, size - offset );
                break;
            case AMF0_TYPE_REFERENCE:
                result = amf0_get_reference( src + offset, size - offset, &temp_d32 );
                break;
            case AMF0_TYPE_STRICT_ARRAY:
                result = amf0_get_strict_array( src + offset, size - offset );
                break;
            case AMF0_TYPE_STRING:
                amf0_get_string_length( src + offset, size - offset, &temp_size );
                break;
            case AMF0_TYPE_TYPED_OBJECT:
                result = amf0_get_typed_object( src + offset, size - offset );
                break;
            case AMF0_TYPE_UNDEFINED:
                result = amf0_get_undefined( src + offset, size - offset );
                break;
            case AMF0_TYPE_UNSUPPORTED:
                result = amf0_get_unsupported( src + offset, size - offset );
                break;
            case AMF0_TYPE_XML_DOCUMENT:
                amf0_get_xmldocument_length( src + offset, size - offset, &temp_size );
                break;
            default:
                result = AMF_ERR_INVALID_DATA;
                break;
        }
        if( result < 0 ){
            goto aborted;
        }
        offset += result;
        switch( type ){
            case AMF0_TYPE_LONG_STRING:
            case AMF0_TYPE_STRING:
            case AMF0_TYPE_XML_DOCUMENT:
                result = amf_push_string_alloc( amf, &buffer, temp_size );
                break;
            default:
                result = AMF_ERR_NONE;
                break;
        }
        if( result < 0 ){
            goto aborted;
        }
        offset += result;
        switch( type ){
            case AMF0_TYPE_LONG_STRING:
                result = amf0_get_long_string( src + offset, size - offset, buffer, temp_size, nullptr );
                break;
            case AMF0_TYPE_STRING:
                result = amf0_get_string( src + offset, size - offset, buffer, temp_size, nullptr );
                break;
            case AMF0_TYPE_XML_DOCUMENT:
                result = amf0_get_xmldocument( src + offset, size - offset, buffer, temp_size, nullptr );
                break;
            default:
                result = AMF_ERR_NONE;
                break;
        }
        if( result < 0 ){
            goto aborted;
        }
        offset += result;
        switch( type ){
            case AMF0_TYPE_AVMPLUS:
                break;
            case AMF0_TYPE_BOOLEAN:
                result = amf_push_boolean( amf, temp_d );
                break;
            case AMF0_TYPE_DATE:
                result = amf_push_date( amf, temp_f, temp_d );
                break;
            case AMF0_TYPE_ECMA_ARRAY:
                result = AMF_ERR_INVALID_DATA;
                break;
            case AMF0_TYPE_LONG_STRING:
                result = amf_push_long_string( amf, buffer );
                break;
            case AMF0_TYPE_MOVIECLIP:
                result = AMF_ERR_INVALID_DATA;
                break;
            case AMF0_TYPE_NULL:
                result = amf_push_null( amf );
                break;
            case AMF0_TYPE_NUMBER:
                result = amf_push_number( amf, temp_f );
                break;
            case AMF0_TYPE_OBJECT:
                result = amf_push_object_start( amf );
                break;
            case AMF0_TYPE_OBJECT_END:
                result = amf_push_object_end( amf );
                break;
            case AMF0_TYPE_RECORDSET:
                result = AMF_ERR_INVALID_DATA;
                break;
            case AMF0_TYPE_REFERENCE:
                result = amf_push_reference( amf, temp_d32 );
                break;
            case AMF0_TYPE_STRICT_ARRAY:
                result = AMF_ERR_INVALID_DATA;
                break;
            case AMF0_TYPE_STRING:
                result = amf_push_string( amf, buffer );
                break;
            case AMF0_TYPE_TYPED_OBJECT:
                result = AMF_ERR_INVALID_DATA;
                break;
            case AMF0_TYPE_UNDEFINED:
                result = amf_push_undefined( amf );
                break;
            case AMF0_TYPE_UNSUPPORTED:
                result = amf_push_unsupported( amf );
                break;
            case AMF0_TYPE_XML_DOCUMENT:
                result = amf_push_xml( amf, buffer );
                break;
            default:
                result = AMF_ERR_INVALID_DATA;
                break;
        }
        if( result < 0 ){
            goto aborted;
        }
        offset += result;
    }
    return offset;

    aborted:
    if( read ){
        *read = offset;
    }
    return result;
}

static amf_v_t * amf_v_get_object( amf_t amf ){
    if( VEC_SIZE(amf->items) > 0 ){
        size_t depth = 1;
        amf_v_t *obj = &VEC_BACK(amf->items);
        while( amf_value_is( &obj->value, AMF0_TYPE_OBJECT ) && VEC_SIZE(obj->object.members) > 0 && depth < amf->depth ){
            obj = &VEC_BACK(obj->object.members).value;
            ++depth;
        }
        if( depth != amf->depth ){
            return nullptr;
        }
        if( amf_value_is( &obj->value, AMF0_TYPE_OBJECT ) ){
            return obj;
        }
    }
    return nullptr;
}

static amf_v_member_t * amf_v_push_member( amf_t amf ){
    amf_v_t* obj = amf_v_get_object( amf );
    if( obj ){
        return VEC_PUSH( obj->object.members );
    }
    return nullptr;
}

static amf_v_t *amf_push_item( amf_t amf ){
    if( amf->depth == 0 ){
        return VEC_PUSH(amf->items);
    }
    else if( VEC_SIZE(amf->items) > 0 ){
        amf_v_t* obj = amf_v_get_object( amf );
        if( obj ){
            return &VEC_BACK(obj->object.members).value;
        }
    }
    return nullptr;
}

#define PUSH_PREP(a,name) \
    if( a->depth > 0 && !a->member_ready ){\
        return AMF_ERR_NEED_NAME;\
    }\
    a->member_ready = false;\
    amf_v_t * name = amf_push_item( a );\
    if( !name ){\
        return AMF_ERR_OOM;\
    }

static amf_err_t push_str( amf_t a, const void * str, amf0_type_t t) {
    if( str != a->allocation ){
        size_t len = strlen( str );
        void * ptr = malloc( len + 1 );
        if( !ptr ){
            return AMF_ERR_OOM;
        }
        memcpy( ptr, str, len + 1 );
        PUSH_PREP(a,target);
        target->string.data = ptr;
        target->string.length = len;
        target->string.type = t;
        return AMF_ERR_NONE;
    }
    else{
        PUSH_PREP(a,target);
        target->string.data = a->allocation;
        target->string.length = a->allocation_len;
        target->string.type = t;
        a->allocation = nullptr;
        a->allocation_len = 0;
        return AMF_ERR_NONE;
    }
}

amf_err_t amf_push_number( amf_t amf, double number ){
    PUSH_PREP( amf, target );
    target->number.number = number;
    target->number.type = AMF0_TYPE_NUMBER;
    return AMF_ERR_NONE;
}

amf_err_t amf_push_boolean( amf_t amf, char boolean ){
    PUSH_PREP( amf, target );
    target->boolean.boolean = boolean;
    target->boolean.type = AMF0_TYPE_BOOLEAN;
    return AMF_ERR_NONE;
}
amf_err_t amf_push_string_alloc( amf_t amf, void** destination, size_t length ){
    if( amf->allocation ){
        free( amf->allocation );
    }
    amf->allocation = malloc( length * sizeof( char ) );
    if( amf->allocation == nullptr ){
        return AMF_ERR_OOM;
    }
    amf->allocation_len = length;
    *destination = amf->allocation;
    return AMF_ERR_NONE;
}
amf_err_t amf_push_string( amf_t amf, const void *str ){
    return push_str( amf, str, AMF0_TYPE_STRING );
}
amf_err_t amf_push_object_start( amf_t amf ){
    PUSH_PREP( amf, target );
    amf->depth ++;
    target->object.type = AMF0_TYPE_OBJECT;
    target->object.write_offset = 0;
    VEC_INIT(target->object.members);

    amf_v_t** temp = VEC_PUSH(amf->ref_table);
    if( !temp ){
        return AMF_ERR_OOM;
    }
    *temp = target;
    return AMF_ERR_NONE;
}
amf_err_t amf_push_member( amf_t amf, const void *str ){
    if( amf->member_ready ){
        return AMF_ERR_INCOMPLETE;
    }
    amf_v_member_t* mem = amf_v_push_member( amf );
    if( mem ){
        mem->length = amf->allocation_len;
        mem->name = amf->allocation;
        amf->allocation_len = 0;
        amf->allocation = nullptr;
        amf->member_ready = true;
        return AMF_ERR_NONE;
    }
    return AMF_ERR_OOM;
}
amf_err_t amf_push_null( amf_t amf ){
    PUSH_PREP( amf, target );
    target->null.type = AMF0_TYPE_NULL;
    return AMF_ERR_NONE;
}
amf_err_t amf_push_undefined( amf_t amf ){
    PUSH_PREP( amf, target );
    target->undefined.type = AMF0_TYPE_UNDEFINED;
    return AMF_ERR_NONE;
}
amf_err_t amf_push_unsupported( amf_t amf ){
    PUSH_PREP( amf, target );
    target->undefined.type = AMF0_TYPE_UNSUPPORTED;
    return AMF_ERR_NONE;
}
amf_err_t amf_push_reference( amf_t amf, unsigned int ref ){
    PUSH_PREP( amf, target );
    target->reference.ref = nullptr;
    target->reference.ref_num = ref;
    target->reference.type = AMF0_TYPE_REFERENCE;
    if( ref < VEC_SIZE(amf->ref_table) ){
        target->reference.ref = amf->ref_table[ref];
    }
    return AMF_ERR_NONE;
}
amf_err_t amf_push_object_end( amf_t amf ){
    if( amf->member_ready ){
        amf->member_ready = false;
        amf_v_t* obj = amf_v_get_object( amf );
        if( obj && VEC_SIZE(obj->object.members) > 0 ){
            free( VEC_BACK(obj->object.members).name );
            VEC_POP( obj->object.members );
        }
    }
    if( amf->depth > 0 ){
        amf->depth--;
        return AMF_ERR_NONE;
    }
    return AMF_ERR_INVALID_DATA;
}
amf_err_t amf_push_date( amf_t amf, double timestamp, char timezone ){
    PUSH_PREP( amf, target );
    target->date.timestamp = timestamp;
    target->date.timezone = timezone;
    target->string.type = AMF0_TYPE_DATE;
    return AMF_ERR_NONE;
}
amf_err_t amf_push_long_string( amf_t amf, const void *str ){
    return push_str( amf, str, AMF0_TYPE_LONG_STRING );
}
amf_err_t amf_push_xml( amf_t amf, const void *xml ){
    return push_str( amf, xml, AMF0_TYPE_XML_DOCUMENT );
}


size_t amf_get_count( amf_t amf ){
    return VEC_SIZE(amf->items);
}
amf_value_t amf_get_item( amf_t amf, size_t idx ){
    return (amf_value_t) (amf->items + idx);
}
bool amf_value_is( amf_value_t value, amf0_type_t type ){
    if( type == value->type ){
        return true;
    }
    if( value->type == AMF0_TYPE_REFERENCE ){
        amf_v_reference_t *ref = (amf_v_reference_t *) value;
        if( ref->ref->type == type ){
            return true;
        }
    }
    return false;
}

bool amf_value_is_like( amf_value_t value, amf0_type_t type ){
    if( amf_value_is( value, type ) ){
        return true;
    }
    if( value->type == AMF0_TYPE_REFERENCE ){
        amf_v_reference_t *ref = (amf_v_reference_t *) value;
        value = (amf_value_t)ref->ref;
    }
    switch( value->type ){
        //Numerics
        case AMF0_TYPE_NULL:
        switch( type ){
            case AMF0_TYPE_OBJECT:
            case AMF0_TYPE_TYPED_OBJECT:
            return true;
            default: break;
        }
        case AMF0_TYPE_NUMBER:
        case AMF0_TYPE_DATE:
        switch( type ){
            case AMF0_TYPE_NUMBER:
            case AMF0_TYPE_DATE:
            return true;
            default: break;
        }
        break;

        //Strings
        case AMF0_TYPE_STRING:
        case AMF0_TYPE_LONG_STRING:
        case AMF0_TYPE_XML_DOCUMENT:
        switch( type ){
            case AMF0_TYPE_STRING:
            case AMF0_TYPE_LONG_STRING:
            case AMF0_TYPE_XML_DOCUMENT:
            return true;
            default: break;
        }
        break;

        //Objects
        case AMF0_TYPE_OBJECT:
        case AMF0_TYPE_TYPED_OBJECT:
        switch( type ){
            case AMF0_TYPE_OBJECT:
            case AMF0_TYPE_TYPED_OBJECT:
            return true;
            default: break;
        }
        break;

        //Arrays
        case AMF0_TYPE_ECMA_ARRAY:
        case AMF0_TYPE_STRICT_ARRAY:
        switch( type ){
            case AMF0_TYPE_ECMA_ARRAY:
            case AMF0_TYPE_STRICT_ARRAY:
            return true;
            default: break;
        }
        default:
        break;
    }
    return false;
}

double amf_value_get_number( amf_value_t target ){
    const amf_v_t *v = (const amf_v_t*) target;
    return v->number.number;
}

bool amf_value_get_bool( amf_value_t target ){
    const amf_v_t *v = (const amf_v_t*) target;
    return v->boolean.boolean;
}

const char* amf_value_get_string( amf_value_t target, size_t *length ){
    const amf_v_t *v = (const amf_v_t*) target;
    if( length ){
        *length = v->string.length;
    }
    return v->string.data;
}

size_t amf_value_get_ref( amf_value_t target ){
    const amf_v_t *v = (const amf_v_t*) target;
    return v->reference.ref_num;
}

double amf_value_get_date( amf_value_t target, signed char *timezone ){
    const amf_v_t *v = (const amf_v_t*) target;
    if( timezone ){
        *timezone = v->date.timezone;
    }
    return v->date.timestamp;
}

const char* amf_value_get_xml( amf_value_t target, size_t *length ){
    const amf_v_t *v = (const amf_v_t*) target;
    if( length ){
        *length = v->string.length;
    }
    return v->string.data;
}


amf_value_t amf_obj_get_value( amf_value_t target, const char *key ){
    const amf_v_t *v = (const amf_v_t*) target;
    for( int i = 0; i < VEC_SIZE(v->object.members); ++i ){
        if( memcmp( key, VEC_AT(v->object.members, i).name, VEC_AT(v->object.members, i).length ) == 0 ){
            return &VEC_AT(v->object.members, i).value.value;
        }
    }
    return nullptr;
}
amf_value_t amf_obj_get_value_idx( amf_value_t target, size_t idx, char **key, size_t *key_len ){
    const amf_v_t *v = (const amf_v_t*) target;
    if( v->type == AMF0_TYPE_REFERENCE ){
        v = v->reference.ref;
    }
    if( idx >= VEC_SIZE(v->object.members) ){
        return nullptr;
    }
    if( key ){
        *key = VEC_AT(v->object.members, idx).name;
    }
    if( key_len ){
        *key_len = v->object.members[idx].length;
    }
    return &v->object.members[idx].value.value;
}

size_t amf_obj_get_count( const amf_value_t target ){
    const amf_v_t *v = (const amf_v_t*) target;
    return VEC_SIZE(v->object.members);
}

#include <stdio.h>

void amf_print_value_internal( amf_v_t * val, int depth ){

    switch( val->type ){
        case AMF0_TYPE_AVMPLUS:
            break;
        case AMF0_TYPE_BOOLEAN:
            printf( "Boolean: %s", val->boolean.boolean ? "true" : "false" );
            break;
        case AMF0_TYPE_DATE:
            printf( "Date: %f +%d", val->date.timestamp, val->date.timezone );
            break;
        case AMF0_TYPE_MOVIECLIP:
            printf( "Movie clip" );
            break;
        case AMF0_TYPE_NULL:
            printf("Null");
            break;
        case AMF0_TYPE_NUMBER:
            printf( "Number: %f", val->number.number );
            break;
        case AMF0_TYPE_RECORDSET:
            printf( "Recordset");
            break;
        case AMF0_TYPE_REFERENCE:
            printf( "Reference to %lu", val->reference.ref_num );
            break;
        case AMF0_TYPE_UNDEFINED:
            printf( "Undefined");
            break;
        case AMF0_TYPE_UNSUPPORTED:
            printf( "Unsupported");
            break;

        case AMF0_TYPE_ECMA_ARRAY:
            printf( "ECMA array");
            break;
        case AMF0_TYPE_STRICT_ARRAY:
            printf( "Strict array");
            break;
        case AMF0_TYPE_TYPED_OBJECT:
            printf( "Typed Object");
            break;

        case AMF0_TYPE_STRING:
            printf( "String: %s", val->string.data );
            break;
        case AMF0_TYPE_LONG_STRING:
            printf( "Long String: %s", val->string.data );
            break;
        case AMF0_TYPE_XML_DOCUMENT:
            printf( "XML document: %s", val->string.data );
            break;
        case AMF0_TYPE_OBJECT:
            printf( "Object: {\n");
            for( int i = 0; i < VEC_SIZE(val->object.members); ++i ){
                for( int i = 0; i <= depth; ++i ){
                    printf("    ");
                }
                printf( "%.*s: ", (int) val->object.members[i].length, val->object.members[i].name );
                amf_print_value_internal( &val->object.members[i].value, depth+1 );
            }
            for( int i = 0; i < depth; ++i ){
                printf("    ");
            }
            printf( "}");
            break;
        default:
            break;
    }
    printf("\n");
}
void amf_print_value( amf_value_t val ){
    return amf_print_value_internal( (amf_v_t*) val, 0 );
}

void amf_print( amf_t amf ){
    for( int i = 0; i < amf_get_count( amf ); ++i ){
        amf_print_value( amf_get_item( amf, i ) );
    }
}




static amf_err_t amf_push_simple_list_memb( amf_t amf, const char * restrict member_name ){
    size_t len;
    void *buffer;
    amf_err_t ret = AMF_ERR_NONE;
    if( member_name ){
        len = strlen( member_name );
        ret = amf_push_string_alloc( amf, &buffer, len );
        if( ret != AMF_ERR_NONE ){
            return ret;
        }
        memcpy( buffer, member_name, len );
        ret = amf_push_member( amf, buffer );
        if( ret != AMF_ERR_NONE ){
            return ret;
        }
    }
    return ret;
}
static amf_err_t amf_push_simple_list_str( amf_t amf, const char * restrict member_name, const char * restrict member_value ){
    size_t len;
    void *buffer;
    amf_err_t ret = amf_push_simple_list_memb( amf, member_name );
    len = strlen( member_value );
    ret = amf_push_string_alloc( amf, &buffer, len );
    if( ret != AMF_ERR_NONE ){
        return ret;
    }
    memcpy( buffer, member_value, len );
    return amf_push_string( amf, buffer );
}

static amf_err_t amf_push_simple_list_bool( amf_t amf, const char * restrict member_name, int member_value ){
    amf_err_t ret = amf_push_simple_list_memb( amf, member_name );
    if( ret != AMF_ERR_NONE ){
        return ret;
    }
    return amf_push_boolean( amf, member_value );
}

static amf_err_t amf_push_simple_list_dbl( amf_t amf, const char * restrict member_name, double member_value ){
    amf_err_t ret = amf_push_simple_list_memb( amf, member_name );
    if( ret != AMF_ERR_NONE ){
        return ret;
    }

    return amf_push_number( amf, member_value );
}
static amf_err_t amf_push_simple_list_start( amf_t amf, const char * restrict member_name ){
    amf_err_t ret = amf_push_simple_list_memb( amf, member_name );
    if( ret != AMF_ERR_NONE ){
        return ret;
    }

    return amf_push_object_start( amf );
}

static amf_err_t amf_push_simple_list_end( amf_t amf, const char * restrict member_name ){
    amf_err_t ret = amf_push_simple_list_memb( amf, member_name );
    if( ret != AMF_ERR_NONE ){
        return ret;
    }

    return amf_push_object_end( amf );
}

amf_err_t amf_push_simple_list( amf_t amf, va_list list ){
    amf_err_t ret;
    amf0_type_t arg = AMF0_TYPE_NONE;
    int depth = 0;
    while( true ){
        arg = va_arg( list, amf0_type_t );
        if( arg == AMF0_TYPE_NONE ){
            break;
        }
        const char * membname = nullptr;
        if( depth > 0 ){
                membname = va_arg( list, const char* );
        }
        switch( arg ){
            case AMF0_TYPE_BOOLEAN: ret = amf_push_simple_list_bool( amf, membname, va_arg( list, int ) ); break;
            case AMF0_TYPE_STRING: ret = amf_push_simple_list_str( amf, membname, va_arg( list, const char* ) ); break;
            case AMF0_TYPE_NUMBER: ret = amf_push_simple_list_dbl( amf, membname, va_arg( list, double ) ); break;
            case AMF0_TYPE_NUMBER_INT: ret = amf_push_simple_list_dbl( amf, membname, va_arg( list, int ) ); break;
            case AMF0_TYPE_NULL: ret = amf_push_null( amf ); break;
            case AMF0_TYPE_UNDEFINED: ret = amf_push_undefined( amf ); break;
            case AMF0_TYPE_UNSUPPORTED: ret = amf_push_unsupported( amf ); break;
            case AMF0_TYPE_OBJECT:
                ++ depth;
                ret = amf_push_simple_list_start( amf, membname );
                break;
            case AMF0_TYPE_OBJECT_END:
                -- depth;
                ret = amf_push_simple_list_end( amf, membname );
                break;
            break;
            default: return AMF_ERR_INVALID_DATA;
        }
        if( ret != AMF_ERR_NONE ){
            return ret;
        }
    }
    return ret;
}


amf_err_t amf_push_simple( amf_t amf, ... ){
    va_list list;
    va_start( list, amf );
    amf_err_t ret = amf_push_simple_list( amf, list );
    va_end(list);
    return ret;
}
