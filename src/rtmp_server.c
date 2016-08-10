/*
    rtmp_server.c

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

#include "rtmp_server.h"
#include "rtmp/rtmp_types.h"
#include "rtmp/rtmp_stream.h"
#include "rtmp/rtmp_private.h"
#include <stdio.h>
#include <stdlib.h>


struct rtmp_server{
    struct rtmp_stream stream;
    rtmp_app_list_t applist;
    rtmp_app_t app;
};


#define LOAD_ARG_S(a) \
{ \
    amf_value_t val = amf_obj_get_value( args, #a ); \
    if( val ){ \
        size_t len; \
        const char *str = amf_value_get_string(val, &len); \
        self->params. a = str_dupl(str, len); \
    } \
}

#define LOAD_ARG_D(a) \
{ \
    amf_value_t val = amf_obj_get_value( args, #a ); \
    if( val ){ \
        self->params. a = amf_value_get_number( val ); \
    } \
}

rtmp_cb_status_t rtmp_server_shake_done( rtmp_stream_t stream, rtmp_event_t event, void * restrict user ){
    rtmp_err_t err = RTMP_ERR_NONE;
    err = err ? err : rtmp_chunk_conn_set_window_ack_size( rtmp_stream_get_conn( stream ), RTMP_DEFAULT_WINDOW_SIZE );
    err = err ? err : rtmp_chunk_conn_set_peer_bwidth( rtmp_stream_get_conn( stream ), RTMP_DEFAULT_WINDOW_SIZE, RTMP_LIMIT_DYNAMIC );
    err = err ? err : rtmp_chunk_conn_set_chunk_size( rtmp_stream_get_conn( stream ), RTMP_DEFAULT_CHUNK_SIZE );

    if( err != RTMP_ERR_NONE ){
        return RTMP_CB_ABORT;
    }
    return RTMP_CB_CONTINUE;
}

rtmp_cb_status_t rtmp_server_shake_fail( rtmp_stream_t conn, rtmp_event_t event, void * restrict user ){
    return RTMP_CB_ABORT;
}

rtmp_cb_status_t rtmp_server_onconnect(
    rtmp_stream_t stream,
    rtmp_message_type_t message,
    amf_t object,
    void *user
){
    rtmp_server_t self = user;
    if( amf_get_count( object ) < 3 ){
        return RTMP_CB_ABORT;
    }

    if( amf_value_get_number(amf_get_item( object, 1 )) != 1 ){
        return RTMP_CB_ABORT;
    }

    amf_value_t args = amf_get_item( object, 2 );
    if( !amf_value_is( args, AMF0_TYPE_OBJECT ) ){
        return RTMP_CB_ABORT;
    }

    amf_value_t val = amf_obj_get_value( args, "app" );
    if( !val ){
        goto fail;
    }
    size_t len;
    const char *str = amf_value_get_string(val, &len);

    if( !str || self->applist == nullptr || self->app != nullptr ){
        goto fail;
    }
    printf("Got connection for app %s\n", str );

    self->app = rtmp_app_list_get( self->applist, str );

    if( !self->app ){
        goto fail;
    }

    rtmp_cb_status_t status = rtmp_app_connect( stream, self->app, args );
    if( status != RTMP_CB_CONTINUE ){
        goto fail;
    }

    rtmp_err_t err = RTMP_ERR_NONE;
    err = err ? err : rtmp_stream_send_stream_begin( stream, 0 );
    err = err ? err : rtmp_stream_respond( stream, "_result", 1,
        AMF(
            AMF_OBJ(
                AMF_STR("fmsVer", RTMP_FMSVER_STR),
                AMF_INT("capabilities", RTMP_CAPABILITIES),
                AMF_INT("mode", RTMP_MODE)
            ),
            AMF_OBJ(
                AMF_STR("level", "status"),
                AMF_STR("code", RTMP_NETCON_ACCEPT),
                AMF_STR("description", "Connection Accepted"),
                AMF_OBJ( "data",
                        AMF_STR("string", RTMP_FMSVER)
                ),
                AMF_INT("objectEncoding", 0)
            )
        )
    );
    if( err != RTMP_ERR_NONE ){
        return RTMP_CB_ABORT;
    }
    return RTMP_CB_CONTINUE;

    fail:
    rtmp_stream_respond( stream, "_result", 1,
        AMF(
            AMF_OBJ(
                AMF_STR("fmsVer", RTMP_FMSVER_STR),
                AMF_INT("capabilities", RTMP_CAPABILITIES),
                AMF_INT("mode", RTMP_MODE)
            ),
            AMF_OBJ(
                AMF_STR("level", "status"),
                AMF_STR("code", RTMP_NETCON_REJECT),
                AMF_STR("description", "Connection Rejected"),
                AMF_OBJ( "data",
                        AMF_STR("string", RTMP_FMSVER)
                ),
                AMF_INT("objectEncoding", 0)
            )
        )
    );
    return RTMP_CB_ABORT;
}

rtmp_cb_status_t rtmp_server_onreleaseStream( rtmp_stream_t stream, rtmp_message_type_t message, amf_t object, void *user ){
    rtmp_server_t self = user;
    if(!self->app){
        return RTMP_CB_ERROR;
    }
    if( amf_get_count( object ) < 4 || !amf_value_is( amf_get_item( object, 3 ), AMF0_TYPE_STRING ) ){
        return RTMP_CB_ERROR;
    }
    return rtmp_app_release( stream, self->app, amf_get_item( object, 2 ) );
}

rtmp_cb_status_t rtmp_server_onFCPublish( rtmp_stream_t stream, rtmp_message_type_t message, amf_t object, void *user ){
    rtmp_server_t self = user;
    if(!self->app){
        return RTMP_CB_ERROR;
    }
    if( amf_get_count( object ) < 4 ||
       !amf_value_is( amf_get_item( object, 1 ), AMF0_TYPE_NUMBER ) ||
       !amf_value_is( amf_get_item( object, 3 ), AMF0_TYPE_STRING ) ){
        return RTMP_CB_ERROR;
    }

    char buffer[RTMP_TEMP_BUFF_SIZE+22] = "FCPublish to stream ";
    const char * target = amf_value_get_string( amf_get_item( object, 3 ), nullptr );

    size_t len = rtmp_app_fcpublish( stream, self->app, target, buffer + strlen(buffer), RTMP_TEMP_BUFF_SIZE );
    if( len >= RTMP_TEMP_BUFF_SIZE || len <= 0 ){
        return RTMP_CB_ERROR;
    }
    rtmp_err_t err = RTMP_ERR_NONE;
    err = err ? err : rtmp_stream_respond( stream, "onFCPublish", amf_value_get_number(amf_get_item( object, 1 )),
        AMF(
            AMF_NULL(),
            AMF_OBJ(
                AMF_STR("level", "status"),
                AMF_STR("code", RTMP_NETSTREAM_START),
                AMF_STR("description", buffer)
            )
        )
    );
    return err ? RTMP_CB_ERROR : RTMP_CB_CONTINUE;
}

rtmp_cb_status_t rtmp_server_onpublish( rtmp_stream_t stream, rtmp_message_type_t message, amf_t object, void *user ){
    rtmp_server_t self = user;
    if(!self->app){
        return RTMP_CB_ERROR;
    }
    if( amf_get_count( object ) < 4 ||
       !amf_value_is( amf_get_item( object, 1 ), AMF0_TYPE_NUMBER ) ||
       !amf_value_is( amf_get_item( object, 3 ), AMF0_TYPE_STRING ) ||
       !amf_value_is( amf_get_item( object, 4 ), AMF0_TYPE_STRING ) ){
        return RTMP_CB_ERROR;
    }

    char buffer[RTMP_TEMP_BUFF_SIZE+22] = "Publishing ";
    size_t offset = strlen(buffer);
    const char * target = amf_value_get_string( amf_get_item( object, 3 ), nullptr );
    const char * type = amf_value_get_string( amf_get_item( object, 4 ), nullptr );

    size_t len = rtmp_app_publish( stream, self->app, target, type, buffer + offset, RTMP_TEMP_BUFF_SIZE );
    if( len >= RTMP_TEMP_BUFF_SIZE || len <= 0 ){
        return RTMP_CB_ERROR;
    }
    offset += len;
    buffer[offset++] = '.';
    buffer[offset++] = 0;

    rtmp_err_t err = RTMP_ERR_NONE;
    err = err ? err : rtmp_stream_respond2( stream, 3, 1, "onStatus", amf_value_get_number(amf_get_item( object, 1 )),
        AMF(
            AMF_NULL(),
            AMF_OBJ(
                AMF_STR("level", "status"),
                AMF_STR("code", RTMP_NETSTREAM_START),
                AMF_STR("description", buffer)
            )
        )
    );

    return err ? RTMP_CB_ERROR : RTMP_CB_CONTINUE;
}

rtmp_cb_status_t rtmp_server_oncreateStream( rtmp_stream_t stream, rtmp_message_type_t message, amf_t object, void *user ){
    rtmp_server_t self = user;
    if(!self->app){
        return RTMP_CB_ERROR;
    }
    if( amf_get_count( object ) < 3 ||
       !amf_value_is( amf_get_item( object, 1 ), AMF0_TYPE_NUMBER )){
        return RTMP_CB_ERROR;
    }

    //TODO: Make this proper
    int stream_id = 1;

    rtmp_err_t err = RTMP_ERR_NONE;
    err = err ? err : rtmp_stream_respond( stream, "_result", amf_value_get_number(amf_get_item( object, 1 )),
        AMF(
            AMF_NULL(),
            AMF_INT(stream_id)
        )
    );
    err = err ? err : rtmp_stream_send_stream_begin( stream, stream_id );

    return err ? RTMP_CB_ERROR : RTMP_CB_CONTINUE;
}

rtmp_cb_status_t rtmp_server_onsetDataFrame( rtmp_stream_t stream, rtmp_message_type_t message, amf_t object, void *user ){
    return RTMP_CB_CONTINUE;
}


rtmp_server_t rtmp_server_create( void ){
    rtmp_server_t server = calloc( 1, sizeof( struct rtmp_server ) );
    rtmp_stream_create_at( &server->stream, false );
    rtmp_stream_reg_amf( &server->stream, RTMP_MSG_AMF0_CMD, "connect", rtmp_server_onconnect, server );
    rtmp_stream_reg_amf( &server->stream, RTMP_MSG_AMF0_CMD, "releaseStream", rtmp_server_onreleaseStream, server );
    rtmp_stream_reg_amf( &server->stream, RTMP_MSG_AMF0_CMD, "FCPublish", rtmp_server_onFCPublish, server );
    rtmp_stream_reg_amf( &server->stream, RTMP_MSG_AMF0_CMD, "publish", rtmp_server_onpublish, server );
    rtmp_stream_reg_amf( &server->stream, RTMP_MSG_AMF0_CMD, "createStream", rtmp_server_oncreateStream, server );
    //rtmp_stream_reg_amf( &server->stream, RTMP_MSG_AMF0_CMD, "@setDataFrame", rtmp_server_onsetDataFrame, server );
    rtmp_stream_reg_event( &server->stream, RTMP_EVENT_CONNECT_SUCCESS, rtmp_server_shake_done, server );
    rtmp_stream_reg_event( &server->stream, RTMP_EVENT_CONNECT_FAIL, rtmp_server_shake_fail, server );

    return server;
}

void rtmp_server_destroy( rtmp_server_t server ){
    rtmp_stream_destroy_at( &server->stream );
    free( server );
}

bool rtmp_server_connected( rtmp_server_t server ){
    return rtmp_chunk_conn_connected( rtmp_stream_get_conn( &server->stream ) );
}

rtmp_stream_t rtmp_server_stream( rtmp_server_t server ){
    return &server->stream;
}

void rtmp_server_set_app_list( rtmp_server_t server, rtmp_app_list_t list ){
    server->applist = list;
}
