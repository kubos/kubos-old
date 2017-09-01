/*
 * Copyright (C) 2017 Kubos Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>
#include <stdlib.h>
#include <error.h>
#include <nanomsg/src/nn.h>
#include <nanomsg/src/pubsub.h>
#include <nanomsg/src/bus.h>
#include "evented-control/ecp.h"

/* This is the endpoint used by nanomsg for pub/sub broadcast messages.
** Eventually we'll want to read this from a configuration file, but
** hardcoding it is fine for the near term.
*/
char * broadcast_endpoint = "tcp://127.0.0.1:25901";

/**
 * Initialize an ECP Context
 *
*/

tECP_Error ECP_Init( tECP_Context * context ) {
  tECP_Error err = ECP_E_NOERR;
  int i = 0;

  /* Initialize context to known state */
  context->talk = -1;
  context->listen = -1;
  context->talk_id = -1;
  context->listen_id = -1;
  context->callbacks = NULL;

  do {
    sleep(1);
    err = ECP_E_NOERR;
    /* Create talker and listener sockets. */
    if( 0 > ( context->talk = nn_socket( AF_SP, NN_PUB ) ) ) {
      printf("1\n");
      err = ECP_E_GENERIC;
      continue;
    //   break;
    }

    if( 0 > ( context->listen = nn_socket( AF_SP, NN_SUB ) ) ) {
      printf("2\n");
      err = ECP_E_GENERIC;
      continue;
    //   break;
    }

    /* Associate the broadcast endpoint with the sockets */
    if( 0 > ( context->talk_id = nn_bind( context->talk, broadcast_endpoint ) ) ) {
      printf("bind failed %d %s\n", errno, nn_strerror(errno));
      err = ECP_E_GENERIC;
      continue;
    //   break;
    }

    if( 0 > ( context->listen_id = nn_connect( context->listen, broadcast_endpoint ) ) ) {
      printf("4\n");
      err = ECP_E_GENERIC;
      continue;
    //   break;
    }

    break;

  } while( i++ < 10 );

  return( err );
}

/**
 ** Register a callback for messages. Note: we don't start listening for messages until the
 ** first call to ECP_Loop().
 */


tECP_Error ECP_Listen( tECP_Context * context, uint16_t channel, tECP_Callback callback ) {
  tECP_ChannelAction * current = context->callbacks;
  tECP_ChannelAction * new_ca = malloc( sizeof( tECP_ChannelAction ) );

  /* todo: check for null */

  new_ca->channel = channel;
  new_ca->callback = callback;
  new_ca->next = NULL;

  if( NULL == current ) {
    context->callbacks = new_ca;
  } else {
    while( NULL != current-> next ) {
      current = current->next;
    }
    current->next = new_ca;
  }

  return( ECP_E_NOERR );
}

/**
 ** Send a broadcast message on a pub-sub channel. Note: messages are 
 ** broadcast immediately and don't wait for a call to ECP_Loop().
 */

#define BUFFER_SIZE ( sizeof( uint16_t ) + sizeof( tECP_Message ) )

tECP_Error ECP_Broadcast( tECP_Context * context, uint16_t channel, tECP_Message * message ) {
  tECP_Error err = ECP_E_NOERR;
  char * buffer[ BUFFER_SIZE ];

  /* This is a horrible way to construct messages */
  memcpy( buffer, & channel, sizeof( uint16_t ) );
  memcpy( & ( buffer[ sizeof( uint16_t ) ] ), & message, sizeof( tECP_Message ) );

  /* TODO: this should probably be NN_DONTWAIT */
  if( 0 > nn_send( context->talk, buffer, BUFFER_SIZE, 0 ) ) {
    err = ECP_E_GENERIC;
  }
  
  return( err );
}

/**
 ** Block until a message is received or the timeout (in microseconds) expires
 */
tECP_Error ECP_Loop( tECP_Context * context, unsigned int timeout ) {
  tECP_Error err = ECP_E_NOERR;
  int rcvtimeo = timeout / 1000; /* need this in msec for nn_setsockopt */
  char buffer[ BUFFER_SIZE ];
  uint16_t channelid = -1;
  tECP_ChannelAction *current = context->callbacks;
  int recverr;
  
  do {
    if( 0 > nn_setsockopt( context->listen, NN_SOL_SOCKET, NN_RCVTIMEO, & rcvtimeo, sizeof( int ) ) ) {
      printf("fail socketopt\n");
        err = ECP_E_GENERIC;
      break;
    }

    if( 0 > ( recverr = nn_recv( context->listen, buffer, BUFFER_SIZE, 0 ) ) ) {
      printf("fail recv %d %s\n", errno, nn_strerror(errno));
      if( ETIMEDOUT != errno ) {
        err = ECP_E_GENERIC;
      }
      break;
    }
    
    channelid = * ( (uint16_t *) & buffer );

    while( NULL != current ) {
      if( current->channel == channelid ) {
        break;
      }
      current = current->next;
    }

    if( NULL != current ) {
     err = current->callback( context, & buffer );
     break;
    }
    
  } while( 0 );
  
  return( err );
}

/**
 ** Release resources allocated by ECP_Init()
 */
tECP_Error ECP_Destroy( tECP_Context * context ) {
  tECP_Error err = ECP_E_NOERR;

  do {
    /* Call shutdown on endpoint ids if they're not -1 */
    if( ( -1 != context->listen_id ) &&
        ( -1 != context->listen ) &&
        ( 0 > nn_shutdown( context->listen, context->listen_id ) ) ) {
      err = ECP_E_GENERIC;
    }

    if( ( -1 != context->talk_id ) &&
        ( -1 != context->talk ) &&
        ( 0 > nn_shutdown( context->talk, context->talk_id ) ) ) {
      err = ECP_E_GENERIC;
    }

    /* Call close on sockets if they're not -1 */
    if( ( -1 != context->listen ) &&
        ( 0 > nn_close( context->listen ) ) ) {
      err = ECP_E_GENERIC;
    }

    if( ( -1 != context->talk ) &&
        ( 0 > nn_close( context->talk ) ) ) {
      err = ECP_E_GENERIC;
    }

  } while( 0 );

  return( err );
}
