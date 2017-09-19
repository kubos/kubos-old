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

/*
 * This file defines the interface into the AOS frame parser for the Kubos
 * Evented Radio Controller. The parser itself is relatively simple, providing
 * just enough functionality to evaluate an AOS frame (presumably from the
 * radio) then call a handler based on its virtual channel id. This functionality
 * is separate from the rest of the radio controller code to facilitate better
 * testing.
 */

#pragma once

/*
 * Let's define a few constants first. AOS frames are a fixed size; we use a
 * macro to define the AOS frame size to underscore it's FIXED nature. The frame
 * size includes octets for header and optional trailer.
 *
 * If AOS_FRAME_TRAILER_SIZE is 0, 2 or 6 depending on whether the macros
 * AOS_FRAME_OPERATIONAL_CONTROL_SIZE and AOS_FRAME_ERROR_CONTROL_SIZE are defined.
 *
 * The macro AOS_FRAME_DATA_FIELD_SIZE is equal to the number of 
 *
 * The frame size provided here is very likely completely incorrect.
 * TODO: Figure out what the actual AOS frame size is.
 */

#ifndef AOS_FRAME_SIZE
#define AOS_FRAME_SIZE 256
#endif

#ifdef AOS_FRAME_OPERATIONAL_CONTROL
#define AOS_FRAME_OPERATIONAL_CONTROL_SIZE 4
#else
#define AOS_FRAME_OPERATIONAL_CONTROL_SIZE 0
#endif

#ifdef AOS_FRAME_ERROR_CONTROL
#define AOS_FRAME_ERROR_CONTROL_SIZE 2
#else
#define AOS_FRAME_ERROR_CONTROL_SIZE 0
#endif

#define AOS_FRAME_TRAILER_SIZE ( AOS_FRAME_OPERATIONAL_CONTROL_SIZE + AOS_FRAME_OPERATIONAL_CONTROL_SIZE )

#define AOS_FRAME_HEADER_SIZE 6

#define AOS_FRAME_DATA_FIELD_SIZE ( AOS_FRAME_SIZE - AOS_FRAME_HEADER_SIZE - AOS_FRAME_TRAILER_SIZE )

/*
 * The system has a fixed number of virtual channel IDs:
 */

#define AOS_VC_RETRANSMIT 0 /* Channel used to request retransmission */
#define AOS_VC_CONTROL    1 /* Used to send commands */
#define AOS_VC_TELEMETRY  2 /* Used to request and report telemetry */
#define AOS_VC_COUNT      3 /* # of channels currently supported */

/* File Includes */
#include <stdint.h>

/* Typedefs, Structs, Unions, Enums */
typedef unsigned int tAOSErr;

typedef tAOSErr (*tAOSCallback)( unsigned char * data, unsigned int length );

typedef struct {
  uint32_t last_id;
  tAOSCallback callback;
} tAOSVirtualChannel;

typedef struct {
  tAOSVirtualChannel channels[ AOS_VC_COUNT ];
} tAOSParser;

/* Function Prototypes */
tAOSErr AOS_Init( tAOSParser * parser );
tAOSErr AOS_Register( tAOSParser * parser, unsigned int channel, tAOSCallback callback );
tAOSErr AOS_Process( tAOSParser * parser, unsigned char * data, unsigned int length );
