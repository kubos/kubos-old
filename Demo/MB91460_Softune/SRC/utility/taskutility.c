/* THIS SAMPLE CODE IS PROVIDED AS IS AND IS SUBJECT TO ALTERATIONS. FUJITSU */
/* MICROELECTRONICS ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR */
/* ELIGIBILITY FOR ANY PURPOSES.                                             */
/*                 (C) Fujitsu Microelectronics Europe GmbH                  */
/*------------------------------------------------------------------------
  taskutility.C
  - 
-------------------------------------------------------------------------*/

 
/*************************@INCLUDE_START************************/
#include "mb91467d.h"
#include "vectors.h"
#include "FreeRTOS.h"
#include "task.h"


static void vUART5Task( void *pvParameters );

/**************************@INCLUDE_END*************************/
/*********************@GLOBAL_VARIABLES_START*******************/
const char ASCII[] = "0123456789ABCDEF";

xTaskHandle UART_TaskHandle;

void InitUart5(void)
{
	//Initialize UART asynchronous mode
	BGR05 = 1666; //  9600 Baud @ 16MHz
	
	SCR05 = 0x17;    // 7N2
	SMR05 = 0x0d;    // enable SOT3, Reset, normal mode
	SSR05 = 0x00;    // LSB first
	
	PFR19_D4 = 1;    // enable UART
	PFR19_D5 = 1;    // enable UART

	//EPFR19 = 0x00;   // enable UART
	
	SSR05_RIE = 1;
}

void Putch5(char ch)         /* sends a char */
{
  while (SSR05_TDRE == 0);    /* wait for transmit buffer empty 	*/
  TDR05 = ch;                 /* put ch into buffer			*/
}

char Getch5(void)            /* waits for and returns incomming char 	*/
{
  volatile unsigned ch;
  
  while(SSR05_RDRF == 0);	 /* wait for data received  	*/
  if (SSR05_ORE)              /* overrun error 		*/
  {
    ch = RDR05;              /* reset error flags 		*/
    return (char)(-1);
  }
  else
  return (RDR05);            /* return char 			*/
}

void Puts5(const char *Name5)  /* Puts a String to UART */
{
  volatile portSHORT i,len;
  len = strlen(Name5);
	
  for (i=0; i<strlen(Name5); i++)   /* go through string                     */
  {
    if (Name5[i] == 10)
      Putch5(13);
    Putch5(Name5[i]);              /* send it out                           */
  }
}

void Puthex5(unsigned long n, unsigned char digits)
{
   unsigned portCHAR digit=0,div=0,i;

   div=(4*(digits-1));	/* init shift divisor */
   for (i=0;i<digits;i++)
   {
     digit = ((n >> div)&0xF); /* get hex-digit value */
	 Putch5(digit + ((digit < 0xA) ? '0' : 'A' - 0xA));
     div-=4;     		/* next digit shift */
   }
}

void Putdec5(unsigned long x, int digits)
{
	portSHORT i;
	portCHAR buf[10],sign=1;
	
	if (digits < 0) {     /* should be print of zero? */
	  digits *= (-1);
	  sign =1;
	}  
	buf[digits]='\0';			/* end sign of string */
	
	for (i=digits; i>0; i--) {
		buf[i-1] = ASCII[x % 10];
		x = x/10;
	}

    if ( sign )
    {
	  for (i=0; buf[i]=='0'; i++) { /* no print of zero */
		if ( i<digits-1)
			buf[i] = ' ';
	  }		
    }
    
	Puts5(buf);					/* send string */
}

void vTraceListTasks( unsigned portBASE_TYPE uxPriority )
{
	portENTER_CRITICAL();
	InitUart5();
	portENTER_CRITICAL();
	xTaskCreate( vUART5Task , ( signed portCHAR * ) "UART4",  ( unsigned portSHORT ) 2048, ( void * ) NULL, uxPriority, &UART_TaskHandle );
}

static void vUART5Task( void *pvParameters )
{
	portCHAR tasklist_buff[512], trace_buff[512];
	unsigned portLONG trace_len, j;
	
	unsigned portCHAR ch;	
	
	Puts5("\n -------------MB91467D FreeRTOS DEMO Task List and Trace Utility----------- \n");

	for(;;)
	{
		Puts5("\n\rPress any of the following keys for the corresponding functionality: ");

		Puts5("\n\r1: To call vTaskList() and display current task status ");

		Puts5("\n\r2: To call vTaskStartTrace() and to display trace results once the trace ends");

		SSR05_RIE=1;

		vTaskSuspend(NULL);

	 	ch=Getch5();

		switch ( ch ) 
		{
			case '1':
				vTaskList( ( signed char * ) tasklist_buff );
				Puts5("\n\rThe current task list is as follows....");
				Puts5("\n\r----------------------------------------------");
				Puts5("\n\rName          State  Priority  Stack   Number");
				Puts5("\n\r----------------------------------------------");
				Puts5(tasklist_buff);
				Puts5("\r----------------------------------------------");
				break;

			case '2':
				vTaskStartTrace(( signed char * ) trace_buff, 512);
				Puts5("\n\rThe trace started!!");
				vTaskDelay( ( portTickType ) 450);
				trace_len = ulTaskEndTrace();
				Puts5("\n\rThe trace ended!!");
				Puts5("\n\rThe trace is as follows....");
				Puts5("\n\r--------------------------------------------------------");
				Puts5("\n\r  Tick     | Task Number  |     Tick     | Task Number  |");
				Puts5("\n\r--------------------------------------------------------\n\r");
				for( j = 0 ; j < trace_len ; j++ )
				{
					Puthex5(trace_buff[j],2);
					if(j%4==3) Puts5("   |   ");
					if(j%16==15) Puts5("\n");
				}
				Puts5("\r--------------------------------------------------------");
				break;
				
			default:
				break;
		}
		Puts5("\n");
	}
}

__interrupt void UART5_RxISR ( void )
{
	SSR05_RIE=0;
	vTaskResume( UART_TaskHandle );
}
