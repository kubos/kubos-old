/*
 * FreeRTOS+FAT FS V1.0.0 (C) 2013 HCC Embedded
 *
 * The FreeRTOS+FAT SL license terms are different to the FreeRTOS license 
 * terms.
 * 
 * FreeRTOS+FAT SL uses a dual license model that allows the software to be used
 * under a pure GPL open source license (as opposed to the modified GPL licence
 * under which FreeRTOS is distributed) or a commercial license.  Details of 
 * both license options follow:
 * 
 * - Open source licensing -
 * FreeRTOS+FAT SL is a free download and may be used, modified, evaluated and
 * distributed without charge provided the user adheres to version two of the 
 * GNU General Public License (GPL) and does not remove the copyright notice or 
 * this text.  The GPL V2 text is available on the gnu.org web site, and on the
 * following URL: http://www.FreeRTOS.org/gpl-2.0.txt.
 * 
 * - Commercial licensing -
 * Businesses and individuals who for commercial or other reasons cannot comply
 * with the terms of the GPL V2 license must obtain a commercial license before 
 * incorporating FreeRTOS+FAT SL into proprietary software for distribution in 
 * any form.  Commercial licenses can be purchased from 
 * http://shop.freertos.org/fat_sl and do not require any source files to be 
 * changed.
 *
 * FreeRTOS+FAT SL is distributed in the hope that it will be useful.  You
 * cannot use FreeRTOS+FAT SL unless you agree that you use the software 'as
 * is'.  FreeRTOS+FAT SL is provided WITHOUT ANY WARRANTY; without even the
 * implied warranties of NON-INFRINGEMENT, MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE. Real Time Engineers Ltd. and HCC Embedded disclaims all
 * conditions and terms, be they implied, expressed, or statutory.
 *
 * http://www.FreeRTOS.org
 * http://www.FreeRTOS.org/FreeRTOS-Plus
 *
 */

#ifndef __DIR_H
#define __DIR_H

#include "../../version/ver_fat_sl.h"
#if VER_FAT_SL_MAJOR != 3 || VER_FAT_SL_MINOR != 2
 #error Incompatible FAT_SL version number!
#endif

#ifdef __cplusplus
extern "C" {
#endif




#define NTRES_LOW           0x08 /*lower case name*/


typedef struct
{
  unsigned char  name[F_MAXNAME];   /* 8+3 */
  unsigned char  ext[F_MAXEXT];
  unsigned char  attr;                  /* 00ADVSHR */

  unsigned char  ntres;             /* FAT32 only  */
  unsigned char  crttimetenth;      /* FAT32 only  */
  unsigned char  crttime[2];        /* FAT32 only  */
  unsigned char  crtdate[2];        /* FAT32 only */
  unsigned char  lastaccessdate[2]; /* FAT32 only */

  unsigned char  clusterhi[2]; /* FAT32 only */
  unsigned char  ctime[2];
  unsigned char  cdate[2];
  unsigned char  clusterlo[2]; /* fat12,fat16,fat32 */
  unsigned char  filesize[4];
} F_DIRENTRY;


unsigned char _f_getdirsector ( unsigned long );
unsigned char _f_findfilewc ( char *, char *, F_POS *, F_DIRENTRY * *, unsigned char );
unsigned char _f_findpath ( F_NAME *, F_POS * );
unsigned long _f_getdecluster ( F_DIRENTRY * );

unsigned char _f_writedirsector ( void );
void _f_setdecluster ( F_DIRENTRY *, unsigned long );
unsigned char _f_addentry ( F_NAME *, F_POS *, F_DIRENTRY * * );

#ifdef __cplusplus
}
#endif

#endif /* ifndef __DIR_H */
