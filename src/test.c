/*
    test.c

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

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "amf.h"
#include "rtmp_chunk_conn.h"
#include "rtmp_constants.h"
#include "ringbuffer.h"
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif



rtmp_cb_status_t data_callback(
    rtmp_chunk_conn_t conn,
    const byte *contents,
    size_t available,
    size_t remaining,
    rtmp_chunk_stream_message_t *msg,
    void *user
){
    printf("Avail: %lu\tRemain: %lu\n", available, remaining );
    for( int i = 0; i < available; ++i ){
        printf("%c", contents[i]);
    }
    printf("\n");
    return RTMP_CB_CONTINUE;
}

int check( int value ){
    if( value < 0 ){
        printf("Failed!\n");
    }
    return value;
}
int check_chunk( int value, rtmp_chunk_stream_message_t **message ){
    rtmp_chunk_stream_message_t *msg = *message;
    if( value < 0 ){
        printf("Failed!\n");
        return -1;
    }
    printf( "Chunk Stream: %d\n"
            "Timestamp: %d\n"
            "Message Length: %lu\n"
            "Message Type: %s (%d)\n"
            "Steam ID: %u\n\n",
        msg->chunk_stream_id,
        msg->timestamp,
        msg->message_length,
        rtmp_get_message_type_name(msg->message_type), msg->message_type,
        msg->message_stream_id);
    return value;
}

void check_conn_err( rtmp_err_t err ){
    if( err != RTMP_ERR_NONE ){
        printf("Error - %s\n", rtmp_get_err_name( err ) );
    }
}


void logger( rtmp_err_t err, size_t line, const char* file, const char* msg, void* usr ){
    printf("Logged: [%s:%lu] %s\n", file, line, rtmp_get_err_name( err ) );
}

void service( rtmp_chunk_conn_t client, rtmp_chunk_conn_t server ){
    const void *from;
    size_t from_size;
    void *to;
    size_t to_size;
    size_t transferring;


    //xmit from client to server
    rtmp_chunk_conn_get_out_buff( client, &from, &from_size );
    rtmp_chunk_conn_get_in_buff( server, &to, &to_size );
    transferring = MIN( from_size, to_size );
    memcpy( to, from, transferring );
    rtmp_chunk_conn_commit_out_buff( client, transferring );
    rtmp_chunk_conn_commit_in_buff( server, transferring );

    //xmit from server to client
    rtmp_chunk_conn_get_out_buff( server, &from, &from_size );
    rtmp_chunk_conn_get_in_buff( client, &to, &to_size );
    transferring = MIN( from_size, to_size );
    memcpy( to, from, transferring );
    rtmp_chunk_conn_commit_out_buff( server, transferring );
    rtmp_chunk_conn_commit_in_buff( client, transferring );

    check_conn_err( rtmp_chunk_conn_service( client ) );
    check_conn_err( rtmp_chunk_conn_service( server ) );
}

#include "amf_object.h"
#define do_alloc(c) \
    amf_push_string_alloc( amf, &target, strlen(c) ); memcpy(target, c, strlen(c));

void printhex(byte* p, int len){
    while( len --> 0 ){
        for( int i = 0; i < 8; ++i ){
            printf("%d", ((*p) >> (7-i)) & 1 );
        }
        printf("\t%02X\n", *(p++));
    }
}

int main(){
    void* target;
    amf_t amf = amf_create( 3 );
    amf_t amf2 = amf_create( 3 );
    amf_push_number( amf, 1337.0 );
    amf_push_date( amf, 123, 10 );
    amf_push_boolean( amf, true );
    do_alloc("Hello there!");
    amf_push_long_string( amf, target);
    amf_push_null( amf );
    amf_push_object_start( amf );
        do_alloc( "Foo" );
            amf_push_member( amf, target );
            amf_push_number( amf, 45141 );
        do_alloc( "bar" );
            amf_push_member( amf, target );
            amf_push_object_start( amf );
                do_alloc( "Rawr" );
                amf_push_member( amf, target );
                amf_push_number( amf, 6667 );
                amf_push_object_end( amf );
        do_alloc( "Meow" );
            amf_push_member( amf, target );
            amf_push_number( amf, 6697 );
        amf_push_object_end( amf );
    amf_push_reference( amf, 0 );
    amf_push_undefined( amf );
    amf_push_object_start( amf );
        do_alloc( "Thing" );
            amf_push_member( amf, target );
            amf_push_number( amf, 45141 );
        do_alloc( "Meow" );
            amf_push_member( amf, target );
            amf_push_number( amf, 6697 );
        amf_push_object_end( amf );
    amf_push_undefined( amf );
    do_alloc( "The end!" );
    amf_push_string( amf, target );

    size_t len = amf_write( amf, nullptr, 0, nullptr );
    byte *buff = malloc( len );
    amf_write( amf, buff, len, nullptr );
    printf("amf_print!\n\n");
    amf_print( amf );
    printf("\n\namf0_print!\n\n");
    amf0_print( buff, len, rtmp_default_printer );
    amf_read( amf2, buff, len, nullptr );
    printf("\n\namf_print!\n\n");
    amf_print( amf2 );
    printf( "\n\nTest access: %f", amf_value_get_number( amf_obj_get_value( amf_obj_get_value( amf_get_item(amf, 5), "bar" ), "Rawr" ) ) );
    amf_destroy( amf );
    amf_destroy( amf2 );
    free(buff);


    return 0;

    rtmp_chunk_conn_t client = rtmp_chunk_conn_create( true );
    rtmp_chunk_conn_t server = rtmp_chunk_conn_create( false );

    rtmp_chunk_conn_register_callbacks( client, data_callback, nullptr, logger, nullptr );
    rtmp_chunk_conn_register_callbacks( server, data_callback, nullptr, logger, nullptr );


    int state = 0;

    while( true ){
        service( client, server );

        if( state == 0 && rtmp_chunk_conn_connected(client) && rtmp_chunk_conn_connected(server) ){
            state = 1;
            rtmp_chunk_conn_set_chunk_size( client, 200 );
            rtmp_chunk_conn_set_chunk_size( server, 1000 );
        }
        else if( state == 1 ){
            rtmp_chunk_conn_set_peer_bwidth( client, 2000, RTMP_LIMIT_HARD );
            ++state;
        }
        else if( state == 2 ){
            rtmp_chunk_conn_set_chunk_size( server, 1000 );
            size_t wrote = 0;
            unsigned char test[5000];
            unsigned char *teststr = (unsigned char*)"Hello there my friend!";
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 2, 1, 0, teststr, 22, nullptr);
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 200, 1, 0, teststr, 22, nullptr);
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 9000, 1, 0, teststr, 22, nullptr);
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 14, 1, 0, teststr, 22, nullptr);
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 55556, 1, 0, teststr, 22, nullptr);
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 55556, 1, 0, teststr, 22, nullptr);
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 55556, 1, 0, teststr, 22, nullptr);
            rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 55556, 1, 0, test, sizeof(test), &wrote);
            while( wrote < sizeof( test ) ){
                rtmp_chunk_conn_send_message( server, RTMP_MSG_AUDIO, 55556, 1, 0, test + wrote, sizeof(test), &wrote);
                service( client, server );
            }
            ++state;
        }

        usleep(10000);
    }
    #if 0
    ors_data_t client_out = ors_data_create_file("data2", "rb");
    char nonce1[1528], nonce2[1528];
    unsigned int time1, time2;
    rtmp_chunk_stream_message_t cache[RTMP_STREAM_CACHE_MAX];
    memset( cache, 0, sizeof( cache ) );

    check( rtmp_chunk_read_shake_0( client_out ) );
    check( rtmp_chunk_read_shake_1( client_out, &time1, nonce1, 1528 ) );
    check( rtmp_chunk_read_shake_2( client_out, &time1, &time2, nonce2, 1528 ) );
    rtmp_chunk_stream_message_t *msg;
    while( ors_data_eof( client_out ) == 0 ){
        check_chunk( rtmp_chunk_read_hdr( client_out, &msg, cache ), &msg );
        if( !msg ){
            continue;
        }

        if( msg->message_type == RTMP_MSG_AMF0_CMD || msg->message_type == RTMP_MSG_AMF0_SO || msg->message_type == RTMP_MSG_AMF0_DAT ){
            amf0_print( client_out, msg->message_length, rtmp_default_printer );
        }
        else{
            byte buf[msg->message_length];

            ors_data_read( client_out, buf, msg->message_length);
            for( int i = 0; i < msg->message_length; ++i){
                //printf("%02X ", buf[i]);
            }
        }
        printf( "\n");
        //ors_data_seek( client_out, msg->message_length - count, ORS_SEEK_OFFSET );
    }
    #endif
    return 0;
}

