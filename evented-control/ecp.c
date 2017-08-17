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

#include "ecp.h"

tECP_Error ECP_Init( tECP_Context * context ) {
  return( ECP_E_NOERR );
}

tECP_Error ECP_Listen( tECP_Context * context, uint16_t channel, tECP_Context (*callback)() ) {
  return( ECP_E_NOERR );
}

tECP_Error ECP_Broadcast( tECP_Context * context, uint16_t channel, tECP_Message * message ) {
  return( ECP_E_NOERR );
}

tECP_Error ECP_Loop( tECP_Context * context, unsigned int timeout ) {
  return( ECP_E_NOERR );
}

tECP_Error ECP_Destroy( tECP_Context * context ) {
  return( ECP_E_NOERR );
}
