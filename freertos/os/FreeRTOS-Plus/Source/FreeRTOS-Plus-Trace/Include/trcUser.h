/*******************************************************************************
 * Tracealyzer v3.0.2 Recorder Library
 * Percepio AB, www.percepio.com
 *
 * trcUser.h
 * The public API of the trace recorder library.
 *
 * Terms of Use
 * This software is copyright Percepio AB. The recorder library is free for
 * use together with Percepio products. You may distribute the recorder library
 * in its original form, including modifications in trcHardwarePort.c/.h
 * given that these modification are clearly marked as your own modifications
 * and documented in the initial comment section of these source files.
 * This software is the intellectual property of Percepio AB and may not be
 * sold or in other ways commercially redistributed without explicit written
 * permission by Percepio AB.
 *
 * Disclaimer
 * The trace tool and recorder library is being delivered to you AS IS and
 * Percepio AB makes no warranty as to its use or performance. Percepio AB does
 * not and cannot warrant the performance or results you may obtain by using the
 * software or documentation. Percepio AB make no warranties, express or
 * implied, as to noninfringement of third party rights, merchantability, or
 * fitness for any particular purpose. In no event will Percepio AB, its
 * technology partners, or distributors be liable to you for any consequential,
 * incidental or special damages, including any lost profits or lost savings,
 * even if a representative of Percepio AB has been advised of the possibility
 * of such damages, or for any claim by any third party. Some jurisdictions do
 * not allow the exclusion or limitation of incidental, consequential or special
 * damages, or the exclusion of implied warranties or limitations on how long an
 * implied warranty may last, so the above limitations may not apply to you.
 *
 * Tabs are used for indent in this file (1 tab = 4 spaces)
 *
 * Copyright Percepio AB, 2014.
 * www.percepio.com
 ******************************************************************************/

#ifndef TRCUSER_H
#define TRCUSER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "trcKernelPort.h"

#if (USE_TRACEALYZER_RECORDER == 1)

#ifndef USE_SEPARATE_USER_EVENT_BUFFER
#define USE_SEPARATE_USER_EVENT_BUFFER 0
#endif

/*******************************************************************************
 * TRACE_STOP_HOOK - Hook Pointer Data Type
 *
 * Declares a data type for a call back function that will be invoked whenever
 * the recorder is stopped.
 ******************************************************************************/
typedef void (*TRACE_STOP_HOOK)(void);

/*******************************************************************************
 * vTraceStopHookPtr
 *
 * Points to a call back function that is called from vTraceStop().
 ******************************************************************************/
extern TRACE_STOP_HOOK vTraceStopHookPtr;

/*******************************************************************************
 * vTraceInitTraceData
 *
 * Allocates, if necessary, and initializes the recorder data structure, based
 * on the constants in trcConfig.h.
 ******************************************************************************/
void vTraceInitTraceData(void);

/*******************************************************************************
 * vTraceSetRecorderData
 *
 * If custom allocation is used, this function must be called so the recorder
 * library knows where to save the trace data.
 ******************************************************************************/
#if (TRACE_DATA_ALLOCATION == TRACE_DATA_ALLOCATION_CUSTOM)
void vTraceSetRecorderData(void* pRecorderData);
#endif

/*******************************************************************************
 * vTraceSetStopHook
 *
 * Sets a function to be called when the recorder is stopped.
 ******************************************************************************/
void vTraceSetStopHook(TRACE_STOP_HOOK stopHookFunction);

/*******************************************************************************
 * uiTraceStart
 *
 * Starts the recorder. The recorder will not be started if an error has been
 * indicated using vTraceError, e.g. if any of the Nx constants in trcConfig.h
 * has a too small value (NTASK, NQUEUE, etc).
 *
 * Returns 1 if the recorder was started successfully.
 * Returns 0 if the recorder start was prevented due to a previous internal
 * error. In that case, check vTraceGetLastError to get the error message.
 * Any error message is also presented when opening a trace file.
 *
 ******************************************************************************/
uint32_t uiTraceStart(void);

/*******************************************************************************
 * vTraceStart
 *
 * Starts the recorder. The recorder will not be started if an error has been
 * indicated using vTraceError, e.g. if any of the Nx constants in trcConfig.h
 * has a too small value (NTASK, NQUEUE, etc).
 *
 * This function is obsolete, but has been saved for backwards compatibility.
 * We recommend using uiTraceStart instead.
 ******************************************************************************/
void vTraceStart(void);

/*******************************************************************************
 * vTraceStop
 *
 * Stops the recorder. The recording can be resumed by calling vTraceStart.
 * This does not reset the recorder. Use vTraceClear is that is desired.
 ******************************************************************************/
void vTraceStop(void);

/*******************************************************************************
 * xTraceGetLastError
 *
 * Gives the last error message, if any. NULL if no error message is stored.
 * Any error message is also presented when opening a trace file.
 ******************************************************************************/
char* xTraceGetLastError(void);

/*******************************************************************************
 * vTraceClear
 *
 * Resets the recorder. Only necessary if a restart is desired - this is not
 * needed in the startup initialization.
 ******************************************************************************/
void vTraceClear(void);

/*******************************************************************************
* vTraceClearError
*
* Removes any previous error message generated by recorder calling vTraceError.
* By calling this function, it may be possible to start/restart the trace
* despite errors in the recorder, but there is no guarantee that the trace
* recorder will work correctly in that case, depending on the type of error.
******************************************************************************/
void vTraceClearError(int resetErrorMessage);

#if (INCLUDE_ISR_TRACING == 1)

/*******************************************************************************
 * vTraceSetISRProperties
 *
 * Registers an Interrupt Service Routine in the recorder library, This must be
 * called before using vTraceStoreISRBegin to store ISR events. This is
 * typically called in the startup of the system, before the scheduler is
 * started.
 *
 * Example:
 *	 #define ID_ISR_TIMER1 1		// lowest valid ID is 1
 *	 #define PRIO_OF_ISR_TIMER1 3 // the hardware priority of the interrupt
 *	 ...
 *	 vTraceSetISRProperties(ID_ISR_TIMER1, "ISRTimer1", PRIO_OF_ISR_TIMER1);
 *	 ...
 *	 void ISR_handler()
 *	 {
 *		 vTraceStoreISRBegin(ID_OF_ISR_TIMER1);
 *		 ...
 *		 vTraceStoreISREnd(0);
 *	 }
 ******************************************************************************/
void vTraceSetISRProperties(objectHandleType handle, const char* name, char priority);

/*******************************************************************************
 * vTraceStoreISRBegin
 *
 * Registers the beginning of an Interrupt Service Routine.
 * If allowing nested ISRs, this must be called with interrupts disabled.
 *
 * Example:
 *	 #define ID_ISR_TIMER1 1		// lowest valid ID is 1
 *	 #define PRIO_OF_ISR_TIMER1 3 // the hardware priority of the interrupt
 *	 ...
 *	 vTraceSetISRProperties(ID_ISR_TIMER1, "ISRTimer1", PRIO_OF_ISR_TIMER1);
 *	 ...
 *	 void ISR_handler()
 *	 {
 *		 vTraceStoreISRBegin(ID_OF_ISR_TIMER1);
 *		 ...
 *		 vTraceStoreISREnd(0);
 *	 }
 *
 ******************************************************************************/
void vTraceStoreISRBegin(objectHandleType id);

/*******************************************************************************
 * vTraceStoreISREnd
 *
 * Registers the end of an Interrupt Service Routine.
 *
 * The parameter pendingISR indicates if the interrupt has requested a
 * task-switch (= 1) or if the interrupt returns to the earlier context (= 0)
 *
 * Example:
 *	 #define ID_ISR_TIMER1 1	  // lowest valid ID is 1
 *	 #define PRIO_OF_ISR_TIMER1 3 // the hardware priority of the interrupt
 *	 ...
 *	 vTraceSetISRProperties(ID_ISR_TIMER1, "ISRTimer1", PRIO_OF_ISR_TIMER1);
 *	 ...
 *	 void ISR_handler()
 *	 {
 *		 vTraceStoreISRBegin(ID_OF_ISR_TIMER1);
 *		 ...
 *		 vTraceStoreISREnd(0);
 *	 }
 *
 ******************************************************************************/
void vTraceStoreISREnd(int pendingISR);

#else
	/* If not including the ISR recording */

void vTraceIncreaseISRActive(void);

void vTraceDecreaseISRActive(void);

#define vTraceSetISRProperties(handle, name, priority)
#define vTraceStoreISRBegin(id) vTraceIncreaseISRActive()
#define vTraceStoreISREnd() vTraceDecreaseISRActive()

#endif


/******************************************************************************
 * vTraceTaskInstanceFinish(void)
 *
 * Marks the current task instance as finished on the next kernel call.
 *
 * If that kernel call is blocking, the instance ends after the blocking event
 * and the corresponding return event is then the start of the next instance.
 * If the kernel call is not blocking, the viewer instead splits the current
 * fragment right before the kernel call, which makes this call the first event
 * of the next instance.
 *
 * See also USE_IMPLICIT_IFE_RULES in trcConfig.h
 *
 * Example:
 *
 *		while(1)
 *		{
 *			xQueueReceive(CommandQueue, &command, timeoutDuration);
 *			processCommand(command);
 *          vTraceInstanceFinish();
 *		}
 *
 *****************************************************************************/
void vTraceTaskInstanceFinish(void);

/******************************************************************************
 * vTraceTaskInstanceFinishDirect(void)
 *
 * Marks the current task instance as finished at this very instant.
 * This makes the viewer to splits the current fragment at this point and begin
 * a new actor instance.
 *
 * See also USE_IMPLICIT_IFE_RULES in trcConfig.h
 *
 * Example:
 *
 *		This example will generate two instances for each loop iteration.
 *		The first instance ends at vTraceInstanceFinishDirect(), while the second
 *      instance ends at the next xQueueReceive call.
 *
 *		while (1)
 *		{
 *          xQueueReceive(CommandQueue, &command, timeoutDuration);
 *			ProcessCommand(command);
 *			vTraceInstanceFinishDirect();
 *			DoSometingElse();
 *          vTraceInstanceFinish();
 *      }
 *
 *
 *****************************************************************************/
void vTraceTaskInstanceFinishDirect(void);

/*******************************************************************************
 * vTraceGetTraceBuffer
 *
 * Returns a pointer to the recorder data structure. Use this together with
 * uiTraceGetTraceBufferSize if you wish to implement an own store/upload
 * solution, e.g., in case a debugger connection is not available for uploading
 * the data.
 ******************************************************************************/
void* vTraceGetTraceBuffer(void);

/*******************************************************************************
 * uiTraceGetTraceBufferSize
 *
 * Gets the size of the recorder data structure. For use together with
 * vTraceGetTraceBuffer if you wish to implement an own store/upload solution,
 * e.g., in case a debugger connection is not available for uploading the data.
 ******************************************************************************/
uint32_t uiTraceGetTraceBufferSize(void);

#if (INCLUDE_USER_EVENTS == 1)

/*******************************************************************************
 * xTraceOpenLabel
 *
 * Creates user event labels for user event channels or for individual events.
 * User events can be used to log application events and data for display in
 * the visualization tool. A user event is identified by a label, i.e., a string,
 * which is stored in the recorder's symbol table.
 * When logging a user event, a numeric handle (reference) to this string is
 * used to identify the event. This is obtained by calling
 *
 *	 xTraceOpenLabel()
 *
 * whihc adds the string to the symbol table (if not already present)
 * and returns the corresponding handle.
 *
 * This can be used in two ways:
 *
 * 1. The handle is looked up every time, when storing the user event.
 *
 * Example:
 *	 vTraceUserEvent(xTraceOpenLabel("MyUserEvent"));
 *
 * 2. The label is registered just once, with the handle stored in an
 * application variable - much like using a file handle.
 *
 * Example:
 *	 myEventHandle = xTraceOpenLabel("MyUserEvent");
 *	 ...
 *	 vTraceUserEvent(myEventHandle);
 *
 * The second option is faster since no lookup is required on each event, and
 * therefore recommended for user events that are frequently
 * executed and/or located in time-critical code. The lookup operation is
 * however fairly fast due to the design of the symbol table.
 ******************************************************************************/
traceLabel xTraceOpenLabel(const char* label);

 /******************************************************************************
 * vTraceUserEvent
 *
 * Basic user event (Standard and Professional Edition only)
 *
 * Generates a User Event with a text label. The label is created/looked up
 * in the symbol table using xTraceOpenLabel.
 ******************************************************************************/
void vTraceUserEvent(traceLabel eventLabel);

 /******************************************************************************
 * vTracePrintF
 *
 * Advanced user events (Professional Edition only)
 *
 * Generates User Event with formatted text and data, similar to a "printf".
 * It is very fast compared to a normal "printf" since this function only
 * stores the arguments. The actual formatting is done
 * on the host PC when the trace is displayed in the viewer tool.
 *
 * User Event labels are created using xTraceOpenLabel.
 * Example:
 *
 *	 traceLabel adc_uechannel = xTraceOpenLabel("ADC User Events");
 *	 ...
 *	 vTracePrint(adc_uechannel,
 *				 "ADC channel %d: %lf volts",
 *				 ch, (double)adc_reading/(double)scale);
 *
 * This can be combined into one line, if desired, but this is slower:
 *
 *	 vTracePrint(xTraceOpenLabel("ADC User Events"),
 *				 "ADC channel %d: %lf volts",
 *				 ch, (double)adc_reading/(double)scale);
 *
 * Calling xTraceOpenLabel multiple times will not create duplicate entries, but
 * it is of course faster to just do it once, and then keep the handle for later
 * use. If you don�t have any data arguments, only a text label/string, it is
 * better to use vTraceUserEvent - it is faster.
 *
 * Format specifiers supported:
 * %d - 32 bit signed integer
 * %u - 32 bit unsigned integer
 * %f - 32 bit float
 * %s - string (is copied to the recorder symbol table)
 * %hd - 16 bit signed integer
 * %hu - 16 bit unsigned integer
 * %bd - 8 bit signed integer
 * %bu - 8 bit unsigned integer
 * %lf - double-precision float (Note! See below...)
 *
 * Up to 15 data arguments are allowed, with a total size of maximum 32 byte.
 * In case this is exceeded, the user event is changed into an error message.
 *
 * The data is stored in trace buffer, and is packed to allow storing multiple
 * smaller data entries in the same 4-byte record, e.g., four 8-bit values.
 * A string requires two bytes, as the symbol table is limited to 64K. Storing
 * a double (%lf) uses two records, so this is quite costly. Use float (%f)
 * unless the higher precision is really necessary.
 *
 * Note that the double-precision float (%lf) assumes a 64 bit double
 * representation. This does not seem to be the case on e.g. PIC24 and PIC32.
 * Before using a %lf argument on a 16-bit MCU, please verify that
 * "sizeof(double)" actually gives 8 as expected. If not, use %f instead.
 ******************************************************************************/
void vTracePrintF(traceLabel eventLabel, const char* formatStr, ...);

#if (USE_SEPARATE_USER_EVENT_BUFFER == 1)
UserEventChannel xTraceRegisterChannelFormat(traceLabel channel, traceLabel formatStr);
void vTraceChannelPrintF(UserEventChannel channel, ...);
void vTraceChannelUserEvent(UserEventChannel channel);
#endif

#else

#define vTracePrintF(eventLabel, formatStr, ...);
#define xTraceOpenLabel(label) 0
#define vTraceUserEvent(eventLabel)

#endif

#else

/* Empty defines for user functions to avoid compiler errors if trace is not to be used */

#define vTraceInitTraceData()
#define uiTraceStart() (1) // Fake "success", if used when recorder is excluded from build
#define vTraceStart()
#define vTraceStop()
#define vTraceClear()
#define vTraceStartStatusMonitor()
#define vTraceGetTraceBuffer() ((void*)0)
#define uiTraceGetTraceBufferSize() 0
#define xTraceOpenLabel(label) 0
#define vTraceUserEvent(eventLabel)
#define vTracePrintF(eventLabel,formatStr,...)
#define vTraceExcludeTaskFromSchedulingTrace(name)

#define vTraceSetISRProperties(handle, name, priority)
#define vTraceStoreISRBegin(id)
#define vTraceStoreISREnd(flag)
#define vTraceExcludeTaskFromTrace(handle)
#define vTraceSetQueueName(a, b)
#define vTraceSetMutexName(a, b)
#define vTraceSetSemaphoreName(a, b)
#define vTraceSetEventGroupName(a, b)

#define vTraceSetStopHook(a)
#endif

#ifdef __cplusplus
}
#endif

#endif
