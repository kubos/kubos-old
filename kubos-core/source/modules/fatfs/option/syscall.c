/*------------------------------------------------------------------------*/
/* Sample code of OS dependent controls for FatFs                         */
/* (C)ChaN, 2014                                                          */
/*------------------------------------------------------------------------*/

#include "kubos-core/modules/fatfs/ff.h"

#if _FS_REENTRANT
/*------------------------------------------------------------------------*/
/* Create a Synchronization Object
/*------------------------------------------------------------------------*/
/* This function is called in f_mount() function to create a new
/  synchronization object, such as semaphore and mutex. When a 0 is returned,
/  the f_mount() function fails with FR_INT_ERR.
*/

int ff_cre_syncobj (    /* 1:Function succeeded, 0:Could not create the sync object */
    BYTE vol,           /* Corresponding volume (logical drive number) */
    _SYNC_t *sobj       /* Pointer to return the created sync object */
)
{
    int ret;

    *sobj = xSemaphoreCreateMutex();    /* FreeRTOS */
    ret = (int)(*sobj != NULL);

    return ret;
}



/*------------------------------------------------------------------------*/
/* Delete a Synchronization Object                                        */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount() function to delete a synchronization
/  object that created with ff_cre_syncobj() function. When a 0 is returned,
/  the f_mount() function fails with FR_INT_ERR.
*/

int ff_del_syncobj (    /* 1:Function succeeded, 0:Could not delete due to any error */
    _SYNC_t sobj        /* Sync object tied to the logical drive to be deleted */
)
{
    int ret;

    vSemaphoreDelete(sobj);     /* FreeRTOS */
    return 1;
}



/*------------------------------------------------------------------------*/
/* Request Grant to Access the Volume                                     */
/*------------------------------------------------------------------------*/
/* This function is called on entering file functions to lock the volume.
/  When a 0 is returned, the file function fails with FR_TIMEOUT.
*/

int ff_req_grant (  /* 1:Got a grant to access the volume, 0:Could not get a grant */
    _SYNC_t sobj    /* Sync object to wait */
)
{
    return (int)(xSemaphoreTake(sobj, _FS_TIMEOUT) == pdTRUE);	/* FreeRTOS */
}



/*------------------------------------------------------------------------*/
/* Release Grant to Access the Volume                                     */
/*------------------------------------------------------------------------*/
/* This function is called on leaving file functions to unlock the volume.
*/

void ff_rel_grant (
    _SYNC_t sobj    /* Sync object to be signaled */
)
{
    xSemaphoreGive(sobj);   /* FreeRTOS */
}

#endif
