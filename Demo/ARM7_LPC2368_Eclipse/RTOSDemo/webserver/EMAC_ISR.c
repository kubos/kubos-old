#include "FreeRTOS.h"
#include "Semphr.h"
#include "Task.h"

void vEMAC_ISR( void ) __attribute__((naked));

extern xSemaphoreHandle xEMACSemaphore;

void vEMAC_ISR( void )
{
    portENTER_SWITCHING_ISR();

    static portBASE_TYPE xSwitchRequired;

	xSwitchRequired = pdFALSE;

    /* Clear the interrupt. */
    MAC_INTCLEAR = 0xffff;
    VICVectAddr = 0;

    /* Ensure the uIP task is not blocked as data has arrived. */
    if( xSemaphoreGiveFromISR( xEMACSemaphore, pdFALSE ) )
    {
        xSwitchRequired = pdTRUE;
    }

    /* Switch to the uIP task. */
    portEXIT_SWITCHING_ISR( xSwitchRequired );
}


