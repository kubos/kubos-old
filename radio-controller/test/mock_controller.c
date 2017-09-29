/* mock_controller.c
 *
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
 *
 * This is a mockup of the real radio controller. All it does is opens
 * a file and starts reading frames from it. A simple way to test it is to
 * use the socat command on linux:
 *
 *  $ mkfifo radio
 *  $ socat TCP4-LISTEN:2029 PIPE:radio
 *
 * and then running this test program. This should route frames from port
 * 2029 to the radio FIFO which gets read here. We currently assume there's
 * a fixed size frame. You can change the frame size with the -s option.
 */

#define _FRAME_FILEPATH "radio"
#define _FRAME_SIZE 120

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

void _sigint_handler( int sig );

char * frame_filepath   = _FRAME_FILEPATH;
int    frame_size       = _FRAME_SIZE;
int    frame_descriptor = -1;
int    verbose          = 0;
int    fin              = 0;
char * read_buffer      = NULL;

int main( int argc, char * argv [] ) {
  int opt, err = 0, bytes_read = 0, t = 0;
  
  while( -1 != ( opt = getopt( argc, argv, "f:s:v" ) ) ) {
    switch (opt) {
    case 'f':
      frame_filepath = optarg;
      break;

    case 's':
      frame_size = atoi( optarg );
      break;
      
    case 'v':
      verbose = 1;
      break;

    default:
      fprintf( stderr, "usage: %s [-f filename] [-s frame size] [-v]\n"
               "where:\n"
               " -f filename    name of the file to read frames from\n"
               " -s frame size  size of a frame\n"
               " -v             turns on verbose mode\n", argv[0] );
      exit( 1 );
    }
  }  

  if( 0 != verbose ) { fprintf( stderr, "mock: radio file: %s\nmock: frame size: %d\n", frame_filepath, frame_size ); }

  if( 0 != verbose ) { fprintf( stderr, "mock: setting signal handler\n" ); }
  
  signal( SIGINT, _sigint_handler );
  
  do {
    if( 0 != verbose ) { fprintf( stderr, "mock: allocating read buffer\n" ); }
    if( NULL == ( read_buffer = malloc( frame_size ) ) ) {
      err = 2;
      fprintf( stderr, "mock: error allocing buffer\n" );
      break;
    }
    
    if( 0 != verbose ) { fprintf( stderr, "mock: opening file\n" ); }
    
    if( 0 > ( frame_descriptor = open( frame_filepath, O_RDONLY ) ) ) {
      err = 3;
      fprintf( stderr, "mock: error opening file %s. (%d,%s)\n", frame_filepath, errno, strerror( errno ) );
      break;
    }

    while( ( 0 == err ) && ( 0 == fin ) ) {
      t = read( frame_descriptor, read_buffer, frame_size - bytes_read );
      
      if( t < 0 ) {
        err = 4;
        fprintf( stderr, "mock: error reading file %d %s\n", errno, strerror( errno ) );
        break;
      }
      
      bytes_read += t;
      
      if( frame_size == bytes_read ) {
        printf( "mock: read a whole packet %*.*s\n", frame_size, frame_size, read_buffer );
        bytes_read = 0;
      }

      sleep( 1 );
    }
    
  } while( 0 );

  if( frame_descriptor > -1 ) {
    close( frame_descriptor );
  }

  if( NULL != read_buffer ) {
    free( read_buffer );
    
  }
  return( err );
}

void _sigint_handler( int sig ) {
  fprintf( stderr, "mock: exiting program\n" );
  fin = 1;
}
