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

#pragma once

/* Macro Definitions */
#define ECP_E_NOERR 0

#define ECP_R_SUCCESS          0x00000000
#define ECP_R_ERROR            0x80000000

#define ECP_M_RIO              0x0002
#define ECP_M_RIO_INFO_REQ     0x00020000
#define ECP_M_RIO_INFO_RES     0x00020001
#define ECP_M_RIO_ON           0x00020002
#define ECP_M_RIO_ON_ACK       0x00020003
#define ECP_M_RIO_OFF          0x00020004
#define ECP_M_RIO_OFF_ACK      0x00020005
#define ECP_M_RIO_XMIT         0x00020006
#define ECP_M_RIO_XMIT_ACK     0x00020007
#define ECP_M_RIO_RECV         0x00020009
#define ECP_M_RIO_TEMP_SET     0x0002000A
#define ECP_M_RIO_TEMP_SET_ACK 0x0002000B
#define ECP_M_RIO_TEMP         0x0002000D

#define ECP_M_EPS              0x0003
#define ECP_M_EPS_INFO_REQ     0x00030000
#define ECP_M_EPS_INFO_RES     0x00030001
#define ECP_M_EPS_STAT_REQ     0x00030002
#define ECP_M_EPS_STAT_RES     0x00030003
#define ECP_M_EPS_ALL_ON       0x00030004
#define ECP_M_EPS_ALL_ON_ACK   0x00030005
#define ECP_M_EPS_ALL_OFF      0x00030006
#define ECP_M_EPS_ALL_OFF_ACK  0x00030007
#define ECP_M_EPS_ON           0x00030008
#define ECP_M_EPS_ON_ACK       0x00030009
#define ECP_M_EPS_OFF          0x0003000A
#define ECP_M_EPS_OFF_ACK      0x0003000B
#define ECP_M_EPS_BAT_STAT_REQ 0x0003000C
#define ECP_M_EPS_BAT_STAT_RES 0x0003000D
#define ECP_M_EPS_BAT_LVL_REQ  0x0003000E
#define ECP_M_EPS_BAT_LVL_REQ  0x0003000F
#define ECP_M_EPS_BAT_LVL_ALRM 0x00030011

/* Typedefs, Structs, etc. */
typedef int tECP_Error;

typedef struct {
} tECP_Context;

typedef struct {
} tECP_Message_Null;

typedef struct {
  uint32 response;
} tECP_Message_Ack;

typedef struct {
  uint32 major;
  uint32 minor;
  uint32 patch;
} tECP_Message_Info;

typedef struct {
  uint32 temp;
} tECP_Message_Temp;

typedef struct {
  int32 voltage[8];
  int32 current[8];
} tECP_Message_EPS;

typedef struct {
  int32 line;
} tECP_Message_EPS_Line;

typedef struct {
  unsigned char path[ 128 ];
} tECP_Message_Filespec;

typedef struct {
  uint32 id;
  union content {
    tECP_Message_Null null;
    tECP_Message_Ack ack;
    tECP_Message_Info info;
    tECP_Message_Temp temp;
    tECP_Message_EPS eps;
    tECP_Message_Filespec filename;
    tECP_Message_Line line;
  };
} tECP_Message;

/* Function Prototypes */

tECP_Error ECP_Init( tECP_Context * context );
tECP_Error ECP_Listen( tECP_Context * context, uint16 channel, tECP_Context (*callback)() );
tECP_Error ECP_Broadcast( tECP_Context * context, uint16 channel, tECP_Message * message );
tECP_Error ECP_Loop( tECP_Context * context, unsigned int timeout );
tECP_Error ECP_Destroy( tECP_Context * context );
