/*
 * os.h
 *
 *  Created on: 2023. 9. 5.
 *      Author: ShinSung Industrial Electric
 */

#ifndef PORTS_INCLUDE_OS_H_
#define PORTS_INCLUDE_OS_H_

#include <xdc/std.h>

#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/family/c28/Timer.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Clock.h>

#include <ti/sysbios/utils/Load.h>

/*
 *  SYSBIOS Wrapper
 */

/*
 *  ======== Debug ========
 */
#define DEBUG_ON 1
#if DEBUG_ON
#define DEBUG_Msg(format ) System_printf(format);\
    System_flush()
#define DEBUG_Printf(format, ...) System_printf(format, __VA_ARGS__);\
    System_flush()
#define DEBUG_Info()   System_printf("[%s %d]\n", __FUNCTION__, __LINE__);\
    System_flush()
#else
#define DEBUG_Msg(format)
#define DEBUG_Printf(format, ...)
#define DEBUG_Info()
#endif

/*Timeout*/
#define NO_WAIT                                     BIOS_NO_WAIT
#define WAIT_FOREVER                                BIOS_WAIT_FOREVER

// Semephore Module API
#define SEMAPHORE_POST(handle)                      Semaphore_post(handle)
#define SEMAPHORE_PEND(handle, timeout)             Semaphore_pend(handle, timeout)

// Mailbox Module   API
#define MBOX_POST(handle, msg, timeout)             Mailbox_post(handle, msg, timeout)
#define MBOX_PEND(handle, msg, timeout)             Mailbox_pend(handle, msg, timeout)

// Task Module      API
#define TASK_SLEEP(ticks)                           Task_sleep(ticks)
#define TASK_SET_PRI(handle, priority)              Task_setPri(handle, priority)

#define BIOS_EXIT(stat)                             BIOS_exit(stat)
#define SYSTEM_RESET()                              System_reset()

// Load Module      API
#define CPU_LOAD()                                  Load_getCPULoad()

// Timer Module     API
#define TIMER_START(handle)                         Timer_start(handle)
#define TIMER_STOP(handle)                          Timer_stop(handle)
#define TIMER_PERIOD_SET(handle, period)            Timer_setPeriod(handle, period)
#define TIMER_MS_SET(handle, microsecs)             Timer_setPeriodMicroSecs(handle, microsecs)
#define TIMER_FUNC_SET(handle, fxn, arg)            Timer_setFunc(handle, fxn, arg)

/*  Clock Module      API*/
#define CLOCK_GET_TICKS()                           Clock_getTicks()

/*Load Module*/
#define CPU_LOAD_GET()                              Load_getCPULoad()

#define HWI_LOAD_GET()                              Load_getGlobalHwiLoad()
#define SWI_LOAD_GET()                              Load_getGlobalSwiLoad()
#define TASK_LOAD_GET()                             Load_getTaskLoad()

// Mailbox Module   API
#define MAILBOX_POST(handle, msg, timeout)          Mailbox_post(handle, msg, timeout)
#define MAILBOX_PEND(handle, msg, timeout)          Mailbox_pend(handle, msg, timeout)


// Event Module     API
#define EVENT_POST(handle, eventid)                 Event_post(handle, eventid)
#define EVENT_PEND(handle, eventid_mask, \
                   eventid_or, timeout)  \
                                                    Event_pend(handle, eventid_mask,\
                                                               eventid_or, timeout)

#define EVENT_NONE                                  Event_Id_NONE
#define EVENT_ID0                                   Event_Id_00
#define EVENT_ID1                                   Event_Id_01
#define EVENT_ID2                                   Event_Id_02
#define EVENT_ID3                                   Event_Id_03
#define EVENT_ID4                                   Event_Id_04
#define EVENT_ID5                                   Event_Id_05
#define EVENT_ID6                                   Event_Id_06
#define EVENT_ID7                                   Event_Id_07
#define EVENT_ID8                                   Event_Id_08
#define EVENT_ID9                                   Event_Id_09
#define EVENT_ID10                                  Event_Id_10
#define EVENT_ID11                                  Event_Id_11
#define EVENT_ID12                                  Event_Id_12
#define EVENT_ID13                                  Event_Id_13
#define EVENT_ID14                                  Event_Id_14
#define EVENT_ID15                                  Event_Id_15
#define EVENT_ID16                                  Event_Id_16
#define EVENT_ID17                                  Event_Id_17
#define EVENT_ID18                                  Event_Id_18
#define EVENT_ID19                                  Event_Id_19
#define EVENT_ID20                                  Event_Id_20
#define EVENT_ID21                                  Event_Id_21
#define EVENT_ID22                                  Event_Id_22
#define EVENT_ID23                                  Event_Id_23
#define EVENT_ID24                                  Event_Id_24
#define EVENT_ID25                                  Event_Id_25
#define EVENT_ID26                                  Event_Id_26
#define EVENT_ID27                                  Event_Id_27
#define EVENT_ID28                                  Event_Id_28
#define EVENT_ID29                                  Event_Id_29
#define EVENT_ID30                                  Event_Id_30
#define EVENT_ID31                                  Event_Id_31


/*SYSBIOS Specific*/
#define HWI_LOAD()
#define SWI_LOAD()

#endif /* PORTS_INCLUDE_OS_H_ */
