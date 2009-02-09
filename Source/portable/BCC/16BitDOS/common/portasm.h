/*
	FreeRTOS.org V5.1.2 - Copyright (C) 2003-2009 Richard Barry.

	This file is part of the FreeRTOS.org distribution.

	FreeRTOS.org is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	FreeRTOS.org is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FreeRTOS.org; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	A special exception to the GPL can be applied should you wish to distribute
	a combined work that includes FreeRTOS.org, without being obliged to provide
	the source code for any proprietary components.  See the licensing section 
	of http://www.FreeRTOS.org for full details of how and when the exception
	can be applied.

    ***************************************************************************
    ***************************************************************************
    *                                                                         *
    * Get the FreeRTOS eBook!  See http://www.FreeRTOS.org/Documentation      *
	*                                                                         *
	* This is a concise, step by step, 'hands on' guide that describes both   *
	* general multitasking concepts and FreeRTOS specifics. It presents and   *
	* explains numerous examples that are written using the FreeRTOS API.     *
	* Full source code for all the examples is provided in an accompanying    *
	* .zip file.                                                              *
    *                                                                         *
    ***************************************************************************
    ***************************************************************************

	Please ensure to read the configuration and relevant port sections of the
	online documentation.

	http://www.FreeRTOS.org - Documentation, latest information, license and 
	contact details.

	http://www.SafeRTOS.com - A version that is certified for use in safety 
	critical systems.

	http://www.OpenRTOS.com - Commercial support, development, porting, 
	licensing and training services.
*/

#ifndef PORT_ASM_H
#define PORT_ASM_H

typedef void tskTCB;
extern volatile tskTCB * volatile pxCurrentTCB;
extern void vTaskSwitchContext( void );

/*
 * Saves the stack pointer for one task into its TCB, calls
 * vTaskSwitchContext() to update the TCB being used, then restores the stack
 * from the new TCB read to run the task.
 */
void portSWITCH_CONTEXT( void );

/*
 * Load the stack pointer from the TCB of the task which is going to be first
 * to execute.  Then force an IRET so the registers and IP are popped off the
 * stack.
 */
void portFIRST_CONTEXT( void );

/* There are slightly different versions depending on whether you are building
to include debugger information.  If debugger information is used then there
are a couple of extra bytes left of the ISR stack (presumably for use by the
debugger).  The true stack pointer is then stored in the bp register.  We add
2 to the stack pointer to remove the extra bytes before we restore our context. */

#define portSWITCH_CONTEXT()											\
							asm { mov	ax, seg pxCurrentTCB		}	\
							asm { mov	ds, ax						}	\
							asm { les	bx, pxCurrentTCB			}	/* Save the stack pointer into the TCB. */		\
							asm { mov	es:0x2[ bx ], ss			}	\
							asm { mov	es:[ bx ], sp				}	\
							asm { call  far ptr vTaskSwitchContext	}	/* Perform the switch. */						\
							asm { mov	ax, seg pxCurrentTCB		}	/* Restore the stack pointer from the TCB. */	\
							asm { mov	ds, ax						}	\
							asm { les	bx, dword ptr pxCurrentTCB	}	\
							asm { mov	ss, es:[ bx + 2 ]			}	\
							asm { mov	sp, es:[ bx ]				}

#define portFIRST_CONTEXT()												\
							__asm { mov	ax, seg pxCurrentTCB		}	\
							__asm { mov	ds, ax						}	\
							__asm { les	bx, dword ptr pxCurrentTCB	}	\
							__asm { mov	ss, es:[ bx + 2 ]			}	\
							__asm { mov	sp, es:[ bx ]				}	\
							__asm { pop	bp							}	\
							__asm { pop	di							}	\
							__asm { pop	si							}	\
							__asm { pop	ds							}	\
							__asm { pop	es							}	\
							__asm { pop	dx							}	\
							__asm { pop	cx							}	\
							__asm { pop	bx							}	\
							__asm { pop	ax							}	\
							__asm { iret							}


#endif

