/*******************************************************************************
 * FreeRTOS+Trace v2.2.3 Recorder Library
 * Percepio AB, www.percepio.se
 *
 * trcKernel.c
 *
 * Functions for integration of the trace recorder library in the FreeRTOS 
 * kernel (requires FreeRTOS v7.1.0 or later).
 * 
 * Terms of Use
 * This software is copyright Percepio AB. The recorder library is free for
 * use together with Percepio products. You may distribute the recorder library
 * in its original form, including modifications in trcPort.c and trcPort.h
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
 * FreeRTOS+Trace is available as Free Edition and in two premium editions.
 * You may use the premium features during 30 days for evaluation.
 * Download FreeRTOS+Trace at http://www.percepio.se/index.php?page=downloads
 *
 * Copyright Percepio AB, 2012.
 * www.percepio.se
 ******************************************************************************/

#include "FreeRTOS.h"
#include "task.h"
#include "trcKernel.h"

#if (configUSE_TRACE_FACILITY == 1)

/******************************************************************************
 * TraceObjectClassTable
 * Translates a FreeRTOS QueueType into trace objects classes (TRACE_CLASS_).
 * This was added since we want to map both types of Mutex and both types of 
 * Semaphores on common classes for all Mutexes and all Semaphores respectively. 
 * 
 * FreeRTOS Queue types
 * #define queueQUEUE_TYPE_BASE                  ( 0U ) => TRACE_CLASS_QUEUE
 * #define queueQUEUE_TYPE_MUTEX                 ( 1U ) => TRACE_CLASS_MUTEX
 * #define queueQUEUE_TYPE_COUNTING_SEMAPHORE    ( 2U ) => TRACE_CLASS_SEMAPHORE
 * #define queueQUEUE_TYPE_BINARY_SEMAPHORE      ( 3U ) => TRACE_CLASS_SEMAPHORE
 * #define queueQUEUE_TYPE_RECURSIVE_MUTEX       ( 4U ) => TRACE_CLASS_MUTEX 
 ******************************************************************************/
traceObjectClass TraceObjectClassTable[5]        =  {TRACE_CLASS_QUEUE,     
                                                     TRACE_CLASS_MUTEX,      
                                                     TRACE_CLASS_SEMAPHORE,  
                                                     TRACE_CLASS_SEMAPHORE,
                                                     TRACE_CLASS_MUTEX };

/* This is defined in FreeRTOS! */
extern volatile void * volatile pxCurrentTCB; 

/* Internal variables */
uint8_t nISRactive = 0;
objectHandleType handle_of_last_logged_task = 0;
uint8_t inExcludedTask = 0;

/*******************************************************************************
 * vTraceStoreKernelCall
 *
 * This is the main integration point for storing FreeRTOS kernel calls, and
 * is called by the hooks in FreeRTOS.h (see trcKernel.h for event codes).
 ******************************************************************************/
void vTraceStoreKernelCall(uint32_t ecode, uint32_t objectNumber)
{
    KernelCall * kse;
    uint16_t dts1;

    if (handle_of_last_logged_task == 0)
    {
        return;
    }

    /* This checks if this is the first kernel call after a call to
    vTraceTaskInstanceIsFinished. In that case, calls to this kernel service 
    with this specific kernel object become the "instance finish event"
    (IFE) of the calling task.*/
    if (GET_TASK_FLAG_MARKIFE(handle_of_last_logged_task))
    {
        /* Reset the flag - this has been handled now */
        CLEAR_TASK_FLAG_MARKIFE(handle_of_last_logged_task);

        /* Store the kernel service tagged as instance finished event */
        PROPERTY_TASK_IFE_SERVICECODE(handle_of_last_logged_task) = 
          (uint8_t)ecode;                

        /* Store the handle of the specific kernel object */
        PROPERTY_TASK_IFE_OBJHANDLE(handle_of_last_logged_task) =
          (objectHandleType)objectNumber;    
    }
    
    if (RecorderDataPtr->recorderActive && (!inExcludedTask || nISRactive))
    {
        dts1 = (uint16_t)prvTraceGetDTS(0xFFFF);

        if (RecorderDataPtr->recorderActive) /* Need to repeat this check! */
        {           
            kse = (KernelCall*) xTraceNextFreeEventBufferSlot();
            if (kse != NULL)
            {
                kse->dts = dts1;
                kse->type = (uint8_t)ecode;
                kse->objHandle = (uint8_t)objectNumber;
                prvTraceUpdateCounters();
            }
        }
    }
}

/*******************************************************************************
 * vTraceStoreKernelCallWithParam
 *
 * Used for storing kernel calls with a handle and a numeric parameter. This is
 * only used for traceTASK_PRIORITY_SET at the moment.
 ******************************************************************************/
void vTraceStoreKernelCallWithParam(uint32_t evtcode, 
                                    uint32_t objectNumber, 
                                    uint8_t param)
{
    KernelCallWithParamAndHandle * kse;
    uint8_t dts2;

    if (RecorderDataPtr->recorderActive && handle_of_last_logged_task && 
        (! inExcludedTask || nISRactive))
    {
        dts2 = (uint8_t)prvTraceGetDTS(0xFF);

        if (RecorderDataPtr->recorderActive) /* Need to repeat this check! */
        {                
            kse = (KernelCallWithParamAndHandle*) xTraceNextFreeEventBufferSlot();
            if (kse != NULL)
            {
                kse->dts = dts2;
                kse->type = (uint8_t)evtcode;
                kse->objHandle = (uint8_t)objectNumber;
                kse->param = param;
                prvTraceUpdateCounters();    
            }
        }
    }
}


/*******************************************************************************
 * vTraceStoreKernelCallWithNumericParamOnly
 *
 * Used for storing kernel calls with numeric parameters only. This is
 * only used for traceTASK_DELAY and traceDELAY_UNTIL at the moment.
 ******************************************************************************/
void vTraceStoreKernelCallWithNumericParamOnly(uint32_t evtcode, uint16_t param)
{
    KernelCallWithParam16 * kse;
    uint8_t dts6;

    if (RecorderDataPtr->recorderActive && handle_of_last_logged_task 
        && (! inExcludedTask || nISRactive))
    {        
        dts6 = (uint8_t)prvTraceGetDTS(0xFF);

        if (RecorderDataPtr->recorderActive) /* Need to repeat this check! */
        {                
            kse = (KernelCallWithParam16*) xTraceNextFreeEventBufferSlot();
            if (kse != NULL)
            {
                kse->dts = dts6;
                kse->type = (uint8_t)evtcode;
                kse->param = param;
                prvTraceUpdateCounters();    
            }
        }
    }
}

/*******************************************************************************
 * vTraceStoreTaskswitch
 * Called by the scheduler, from the SWITCHED_OUT hook.
 * At this point interrupts are disabled, so no need to disable interrupts.
 ******************************************************************************/
void vTraceStoreTaskswitch(void)
{
    uint16_t dts3;
    TSEvent* ts;
    static objectHandleType handle_of_running_task = 0;
    int8_t skipEvent = 0;
    
    if (xTaskGetSchedulerState() == 0)
    {
        /* This occurs on the very first taskswitch event, generated by 
        vTraceStart and uiTraceStart if the scheduler is not yet started.
        This creates a dummy "(startup)" task entry internally in the
        recorder */

        handle_of_running_task = xTraceGetObjectHandle(TRACE_CLASS_TASK);
        vTraceSetObjectName(TRACE_CLASS_TASK, 
            handle_of_running_task,
            "(startup)");
        vTraceSetPriorityProperty(TRACE_CLASS_TASK,
            handle_of_running_task,
            0);
    }
    else
    {    
        handle_of_running_task = 
        (objectHandleType)uxTaskGetTaskNumber(xTaskGetCurrentTaskHandle());
    }

    /* Skip the event if the task has been excluded, using vTraceExcludeTask */
    if (GET_TASK_FLAG_ISEXCLUDED(handle_of_running_task))
    {    
        skipEvent = 1;
        inExcludedTask = 1;            
    }        

    /* Skip the event if the same task is scheduled */
    if (handle_of_running_task == handle_of_last_logged_task)
    {
        skipEvent = 1;
    }
    
    
    if (! RecorderDataPtr->recorderActive)
    {
        skipEvent = 1;
    }

    /* If this event should be logged, log it! */
    if (skipEvent == 0)    
    {
        dts3 = (uint16_t)prvTraceGetDTS(0xFFFF);
        
        if (RecorderDataPtr->recorderActive) /* Need to repeat this check! */
        {        
            inExcludedTask = 0;

            handle_of_last_logged_task = handle_of_running_task;
            
            ts = (TSEvent*)xTraceNextFreeEventBufferSlot();

            if (ts != NULL)
            {
                if (uiTraceGetObjectState(TRACE_CLASS_TASK,
                    handle_of_last_logged_task) == TASK_STATE_INSTANCE_ACTIVE)
                {
                    ts->type = TS_TASK_RESUME;
                }
                else
                {
                    ts->type = TS_TASK_BEGIN;
                }

                ts->dts = dts3;
                ts->objHandle = handle_of_last_logged_task;

                vTraceSetObjectState(TRACE_CLASS_TASK, 
                                     handle_of_last_logged_task, 
                                     TASK_STATE_INSTANCE_ACTIVE);

                prvTraceUpdateCounters();    
            }
        }
    }
    
}

/*******************************************************************************
 * vTraceStoreNameCloseEvent
 *
 * Updates the symbol table with the name of this object from the dynamic
 * objects table and stores a "close" event, holding the mapping between handle
 * and name (a symbol table handle). The stored name-handle mapping is thus the
 * "old" one, valid up until this point.
 ******************************************************************************/
#if (INCLUDE_OBJECT_DELETE == 1)
void vTraceStoreObjectNameOnCloseEvent(objectHandleType handle, 
                                       traceObjectClass objectclass)
{    
    ObjCloseNameEvent * ce;
    const char * name;
    traceLabel idx;

    name = PROPERTY_NAME_GET(objectclass, handle);

    idx = prvTraceOpenSymbol(name, 0);

    ce = (ObjCloseNameEvent*) xTraceNextFreeEventBufferSlot();
    if (ce != NULL)
    {
        ce->type = EVENTGROUP_OBJCLOSE_NAME + objectclass;
        ce->objHandle = handle;
        ce->symbolIndex = idx;
        prvTraceUpdateCounters();
    }
}

void vTraceStoreObjectPropertiesOnCloseEvent(objectHandleType handle, 
                                             traceObjectClass objectclass)
{
    ObjClosePropEvent * pe;

    if (objectclass == TRACE_CLASS_ISR)
    {        
        /* ISR handles should not be closed - never called for ISR */
        return;
    }

    pe = (ObjClosePropEvent*) xTraceNextFreeEventBufferSlot();
    if (pe != NULL)
    {
        if (objectclass == TRACE_CLASS_TASK)
        {
            pe->arg1 = PROPERTY_ACTOR_PRIORITY(objectclass, handle);
            pe->arg2 = PROPERTY_TASK_IFE_SERVICECODE(handle);
            pe->arg3 = PROPERTY_TASK_IFE_OBJHANDLE(handle);
            PROPERTY_TASK_IFE_SERVICECODE(handle) = 0;
            PROPERTY_TASK_IFE_OBJHANDLE(handle) = 0;
        }else{
            pe->arg1 = PROPERTY_OBJECT_STATE(objectclass, handle);
        }
        pe->type = EVENTGROUP_OBJCLOSE_PROP + objectclass;    
        prvTraceUpdateCounters();
    }
}
#endif

void vTraceSetPriorityProperty(uint8_t objectclass, uint8_t id, uint8_t value)
{
    PROPERTY_ACTOR_PRIORITY(objectclass, id) = value;
}

uint8_t uiTraceGetPriorityProperty(uint8_t objectclass, uint8_t id)
{
    return PROPERTY_ACTOR_PRIORITY(objectclass, id);
}

void vTraceSetObjectState(uint8_t objectclass, uint8_t id, uint8_t value)
{
    PROPERTY_OBJECT_STATE(objectclass, id) = value;
}

void vTraceSetTaskInstanceFinished(objectHandleType handle)
{
#if (USE_IMPLICIT_IFE_RULES == 1)
    if (PROPERTY_TASK_IFE_SERVICECODE(handle) == 0)
    {
        PROPERTY_OBJECT_STATE(TRACE_CLASS_TASK, handle) = 0;
    }
#endif
}

uint8_t uiTraceGetObjectState(uint8_t objectclass, uint8_t id)
{
    return PROPERTY_OBJECT_STATE(objectclass, id);
}


#endif