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
 * This file implements the API for the AOS frame parser defined in aos.h
 */

#include <string.h>
#include "radio-controller/aos.h"

tAOSErr AOS_Init( tAOSParser * parser ) {
  memset( parser, 0, sizeof( tAOSParser ) );
  return( AOS_ERR_NOERR );
}

tAOSErr AOS_Register( tAOSParser * parser, unsigned int channel, tAOSCallback frame, tAOSCallback exception ) {
  if( channel >= AOS_VC_COUNT ) {
    return( AOS_ERR_ICHANNEL );
  }

  parser->channels[ channel ].frame = frame;
  parser->channels[ channel ].exception = exception;
  
  return( AOS_ERR_NOERR );
}

tAOSErr AOS_Process( tAOSParser * parser, unsigned char * data, unsigned int length ) {
  return( AOS_ERR_NOERR );
}
