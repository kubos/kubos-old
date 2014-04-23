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

#include "../../api/fat_sl.h"
#include "../../psp/include/psp_string.h"

#include "drv.h"
#include "util.h"
#include "volume.h"

#include "../../version/ver_fat_sl.h"
#if VER_FAT_SL_MAJOR != 5 || VER_FAT_SL_MINOR != 2
 #error Incompatible FAT_SL version number!
#endif

F_DRIVER * mdrv = NULL;  /* driver structure */


/****************************************************************************
 *
 * _f_checkstatus
 *
 * checking a volume driver status, if media is removed or has been changed
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
unsigned char _f_checkstatus ( void )
{
  if ( mdrv->getstatus != NULL )
  {
    if ( mdrv->getstatus( mdrv ) & ( F_ST_MISSING | F_ST_CHANGED ) )
    {
      gl_volume.state = F_STATE_NEEDMOUNT; /*card has been removed;*/
      return F_ERR_CARDREMOVED;
    }
  }

  return F_NO_ERROR;
}


/****************************************************************************
 *
 * _f_writesector
 *
 * write sector data on a volume, it calls low level driver function, it
 * writes a complete sector
 *
 * INPUTS
 * sector - which physical sector
 *
 * RETURNS
 * error code or zero if successful
 *
 ***************************************************************************/
unsigned char _f_writeglsector ( unsigned long sector )
{
  unsigned char  retry;

  if ( mdrv->writesector == NULL )
  {
    gl_volume.state = F_STATE_NEEDMOUNT; /*no write function*/
    return F_ERR_ACCESSDENIED;
  }

  if ( sector == (unsigned long)-1 )
  {
    if ( gl_file.modified )
    {
      sector = gl_file.pos.sector;
    }
    else
    {
      sector = gl_volume.actsector;
    }
  }

  if ( sector != (unsigned long)-1 )
  {
    if ( mdrv->getstatus != NULL )
    {
      unsigned int  status;

      status = mdrv->getstatus( mdrv );

      if ( status & ( F_ST_MISSING | F_ST_CHANGED ) )
      {
        gl_volume.state = F_STATE_NEEDMOUNT; /*card has been removed;*/
        return F_ERR_CARDREMOVED;
      }

      if ( status & ( F_ST_WRPROTECT ) )
      {
        gl_volume.state = F_STATE_NEEDMOUNT;  /*card has been removed;*/
        return F_ERR_WRITEPROTECT;
      }
    }

    gl_volume.modified = 0;
    gl_file.modified = 0;
    gl_volume.actsector = sector;
    for ( retry = 3 ; retry ; retry-- )
    {
      int mdrv_ret;
      mdrv_ret = mdrv->writesector( mdrv, (unsigned char *)gl_sector, sector );
      if ( !mdrv_ret )
      {
        return F_NO_ERROR;
      }

      if ( mdrv_ret == -1 )
      {
        gl_volume.state = F_STATE_NEEDMOUNT; /*card has been removed;*/
        return F_ERR_CARDREMOVED;
      }
    }
  }


  return F_ERR_ONDRIVE;
} /* _f_writeglsector */


/****************************************************************************
 *
 * _f_readsector
 *
 * read sector data from a volume, it calls low level driver function, it
 * reads a complete sector
 *
 * INPUTS
 * sector - which physical sector is read
 *
 * RETURNS
 * error code or zero if successful
 *
 ***************************************************************************/
unsigned char _f_readglsector ( unsigned long sector )
{
  unsigned char  retry;
  unsigned char  ret;

  if ( sector == gl_volume.actsector )
  {
    return F_NO_ERROR;
  }

  if ( gl_volume.modified || gl_file.modified )
  {
    ret = _f_writeglsector( (unsigned long)-1 );
    if ( ret )
    {
      return ret;
    }
  }


  for ( retry = 3 ; retry ; retry-- )
  {
    int mdrv_ret;
    mdrv_ret = mdrv->readsector( mdrv, (unsigned char *)gl_sector, sector );
    if ( !mdrv_ret )
    {
      gl_volume.actsector = sector;
      return F_NO_ERROR;
    }

    if ( mdrv_ret == -1 )
    {
      gl_volume.state = F_STATE_NEEDMOUNT; /*card has been removed;*/
      return F_ERR_CARDREMOVED;
    }
  }

  gl_volume.actsector = (unsigned long)-1;
  return F_ERR_ONDRIVE;
} /* _f_readglsector */

