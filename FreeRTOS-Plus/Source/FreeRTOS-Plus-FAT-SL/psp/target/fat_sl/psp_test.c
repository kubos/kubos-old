/*
 * FreeRTOS+FAT SL V1.0.1 (C) 2014 HCC Embedded
 *
 * The FreeRTOS+FAT SL license terms are different to the FreeRTOS license
 * terms.
 *
 * FreeRTOS+FAT SL uses a dual license model that allows the software to be used
 * under a standard GPL open source license, or a commercial license.  The
 * standard GPL license (unlike the modified GPL license under which FreeRTOS
 * itself is distributed) requires that all software statically linked with
 * FreeRTOS+FAT SL is also distributed under the same GPL V2 license terms.
 * Details of both license options follow:
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

#include <stdio.h>
#include "psp_test.h"
#include "config_fat_sl.h"
#include "config_mdriver_ram.h"
#include "../../../api/fat_sl.h"
#include "../../../api/api_mdriver_ram.h"

#include "../../../version/ver_fat_sl.h"
#if VER_FAT_SL_MAJOR != 5
 #error Incompatible FAT_SL version number!
#endif
#include "../../../version/ver_psp_fat_sl.h"
#if VER_PSP_FAT_FAT_SL_MAJOR != 1 || VER_PSP_FAT_FAT_SL_MINOR != 2
 #error Incompatible PSP_FAT_FAT_SL version number!
#endif

#define PRINT_BUFFER_SIZE 64

extern void __printbuf ( char * buf, int len );

/* Use to display text (printf). */
void _f_dump ( char * s )
{
  printf( "%s\r\n", s );
}

/* Use to display test result (printf). */
uint8_t _f_result ( int linenum, uint32_t result )
{
  printf( "Error:%d/%ld\r\n", linenum, result );
  return 0;
}

/* Use to build file system (mount). */
uint8_t _f_poweron ( void )
{
  f_delvolume();
  return f_initvolume( ram_initfunc );
}
