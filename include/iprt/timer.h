/** @file
 * IPRT - Timer.
 */

/*
 * Copyright (C) 2006-2007 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 *
 * The contents of this file may alternatively be used under the terms
 * of the Common Development and Distribution License Version 1.0
 * (CDDL) only, as it comes in the "COPYING.CDDL" file of the
 * VirtualBox OSE distribution, in which case the provisions of the
 * CDDL are applicable instead of those of the GPL.
 *
 * You may elect to license modified versions of this file under the
 * terms and conditions of either the GPL or the CDDL or both.
 */

#ifndef ___iprt_timer_h
#define ___iprt_timer_h


#include <iprt/cdefs.h>
#include <iprt/types.h>


RT_C_DECLS_BEGIN

/** @defgroup grp_rt_timer      RTTimer - Timer
 *
 * The IPRT timer API provides a simple abstraction of recurring and one-shot callback timers.
 *
 * Because of the great variation in the native APIs and the quality of
 * the service delivered by those native APIs, the timers are operated
 * on at best effort basis.
 *
 * All the ring-3 implementations are naturally at the mercy of the scheduler,
 * which means that the callback rate might vary quite a bit and we might skip
 * ticks. Many systems have a restriction that a process can only have one
 * timer. IPRT currently makes no efforts at multiplexing timers in those kind
 * of situations and will simply fail if you try to create more than one timer.
 *
 * Things are generally better in ring-0. The implementations will use interrupt
 * time callbacks wherever available, and if not, resort to a high priority
 * kernel thread.
 *
 * @ingroup grp_rt
 * @{
 */


/** Timer handle. */
typedef struct RTTIMER   *PRTTIMER;

/**
 * Timer callback function.
 *
 * The context this call is made in varies with different platforms and
 * kernel / user mode IPRT.
 *
 * In kernel mode a timer callback should not waste time, it shouldn't
 * waste stack and it should be prepared that some APIs might not work
 * correctly because of weird OS restrictions in this context that we
 * haven't discovered and avoided yet. Please fix those APIs so they
 * at least avoid panics and weird behaviour.
 *
 * @param   pTimer      Timer handle.
 * @param   pvUser      User argument.
 * @param   iTick       The current timer tick. This is always 1 on the first
 *                      callback after the timer was started. For omni timers
 *                      this will be 1 when a cpu comes back online.
 */
typedef DECLCALLBACK(void) FNRTTIMER(PRTTIMER pTimer, void *pvUser, uint64_t iTick);
/** Pointer to FNRTTIMER() function. */
typedef FNRTTIMER *PFNRTTIMER;


/**
 * Create a recurring timer.
 *
 * @returns iprt status code.
 * @param   ppTimer             Where to store the timer handle.
 * @param   uMilliesInterval    Milliseconds between the timer ticks.
 *                              This is rounded up to the system granularity.
 * @param   pfnTimer            Callback function which shall be scheduled for execution
 *                              on every timer tick.
 * @param   pvUser              User argument for the callback.
 * @see     RTTimerDestroy, RTTimerStop
 */
RTDECL(int) RTTimerCreate(PRTTIMER *ppTimer, unsigned uMilliesInterval, PFNRTTIMER pfnTimer, void *pvUser);

/**
 * Create a suspended timer.
 *
 * @returns iprt status code.
 * @retval  VERR_NOT_SUPPORTED if an unsupported flag was specfied.
 *
 * @param   ppTimer             Where to store the timer handle.
 * @param   u64NanoInterval     The interval between timer ticks specified in nanoseconds if it's
 *                              a recurring timer. This is rounded to the fit the system timer granularity.
 *                              For one shot timers, pass 0.
 * @param   fFlags              Timer flags.
 * @param   pfnTimer            Callback function which shall be scheduled for execution
 *                              on every timer tick.
 * @param   pvUser              User argument for the callback.
 * @see     RTTimerStart, RTTimerStop, RTTimerDestroy, RTTimerGetSystemGranularity
 */
RTDECL(int) RTTimerCreateEx(PRTTIMER *ppTimer, uint64_t u64NanoInterval, unsigned fFlags, PFNRTTIMER pfnTimer, void *pvUser);

/** @name RTTimerCreateEx flags
 * @{ */
/** Any CPU is fine. (Must be 0.) */
#define RTTIMER_FLAGS_CPU_ANY        0
/** One specific CPU */
#define RTTIMER_FLAGS_CPU_SPECIFIC   RT_BIT(8)
/** Omni timer, run on all online CPUs.
 * @remarks The timer callback isn't necessarily running at the time same time on each CPU. */
#define RTTIMER_FLAGS_CPU_ALL        ( RTTIMER_FLAGS_CPU_MASK | RTTIMER_FLAGS_CPU_SPECIFIC )
/** CPU mask. */
#define RTTIMER_FLAGS_CPU_MASK       0xff
/** Convert a CPU number (0-based) to RTTimerCreateEx flags.
 * This will automatically OR in the RTTIMER_FLAG_CPU_SPECIFIC flag. */
#define RTTIMER_FLAGS_CPU(iCpu)      ( (iCpu) | RTTIMER_FLAG_CPU_SPECIFIC )
/** Macro that validates the flags. */
#define RTTIMER_FLAGS_ARE_VALID(fFlags) ( !((fFlags) & ~((fFlags) & RTTIMER_FLAGS_CPU_SPECIFIC ? 0x1ffU : 0x100U)) )
/** @} */

/**
 * Stops and destroys a running timer.
 *
 * @returns iprt status code.
 * @param   pTimer      Timer to stop and destroy. NULL is ok.
 */
RTDECL(int) RTTimerDestroy(PRTTIMER pTimer);

/**
 * Stops an active timer.
 *
 * @returns IPRT status code.
 * @retval  VERR_INVALID_HANDLE if pTimer isn't valid.
 * @retval  VERR_TIMER_ACTIVE if the timer isn't suspended.
 *
 * @param   pTimer      The timer to activate.
 * @param   u64First    The RTTimeSystemNanoTS() for when the timer should start
 *                      firing (relative). If 0 is specified, the timer will fire ASAP.
 * @see     RTTimerStop
 */
RTDECL(int) RTTimerStart(PRTTIMER pTimer, uint64_t u64First);

/**
 * Stops an active timer.
 *
 * @returns IPRT status code.
 * @retval  VERR_INVALID_HANDLE if pTimer isn't valid.
 * @retval  VERR_TIMER_SUSPENDED if the timer isn't active.
 * @retval  VERR_NOT_SUPPORTED if the IPRT implementation doesn't support stopping a timer.
 *
 * @param   pTimer  The timer to suspend.
 * @see     RTTimerStart
 */
RTDECL(int) RTTimerStop(PRTTIMER pTimer);


/**
 * Gets the (current) timer granularity of the system.
 *
 * @returns The timer granularity of the system in nanoseconds.
 * @see     RTTimerRequestSystemGranularity
 */
RTDECL(uint32_t) RTTimerGetSystemGranularity(void);

/**
 * Requests a specific system timer granularity.
 *
 * Successfull calls to this API must be coupled with the exact same number of
 * calls to RTTimerReleaseSystemGranularity() in order to undo any changes made.
 *
 *
 * @returns IPRT status code.
 * @retval  VERR_NOT_SUPPORTED if the requested value isn't supported by the host platform
 *          or if the host platform doesn't support modifying the system timer granularity.
 * @retval  VERR_PERMISSION_DENIED if the caller doesn't have the necessary privilege to
 *          modify the system timer granularity.
 *
 * @param   u32Request      The requested system timer granularity in nanoseconds.
 * @param   pu32Granted     Where to store the granted system granularity. This is the value
 *                          that should be passed to  RTTimerReleaseSystemGranularity(). It
 *                          is what RTTimerGetSystemGranularity() would return immediately
 *                          after the change was made.
 *
 *                          The value differ from the request in two ways; rounding and
 *                          scale. Meaning if your request is for 10.000.000 you might
 *                          be granted 10.000.055 or 1.000.000.
 * @see     RTTimerReleaseSystemGranularity, RTTimerGetSystemGranularity
 */
RTDECL(int) RTTimerRequestSystemGranularity(uint32_t u32Request, uint32_t *pu32Granted);

/**
 * Releases a system timer granularity grant acquired by RTTimerRequestSystemGranularity().
 *
 * @returns IPRT status code.
 * @retval  VERR_NOT_SUPPORTED if the host platform doesn't have any way of modifying
 *          the system timer granularity.
 * @retval  VERR_WRONG_ORDER if nobody call RTTimerRequestSystemGranularity() with the
 *          given grant value.
 * @param   u32Granted      The granted system granularity.
 * @see     RTTimerRequestSystemGranularity
 */
RTDECL(int) RTTimerReleaseSystemGranularity(uint32_t u32Granted);



/**
 * Timer callback function for low res timers.
 *
 * This is identfical to FNRTTIMER except for the first parameter, so
 * see FNRTTIMER for details.
 *
 * @param   hTimerLR    The low resolution timer handle.
 * @param   pvUser      User argument.
 * @param   iTick       The current timer tick. This is always 1 on the first
 *                      callback after the timer was started. Will jump if we've
 *                      skipped ticks when lagging behind.
 */
typedef DECLCALLBACK(void) FNRTTIMERLR(RTTIMERLR hTimerLR, void *pvUser, uint64_t iTick);
/** Pointer to FNRTTIMER() function. */
typedef FNRTTIMERLR *PFNRTTIMERLR;


/**
 * Create a recurring low resolution timer.
 *
 * @returns iprt status code.
 * @param   phTimerLR           Where to store the timer handle.
 * @param   uMilliesInterval    Milliseconds between the timer ticks, at least 100 ms.
 *                              If higher resolution is required use the other API.
 * @param   pfnTimer            Callback function which shall be scheduled for execution
 *                              on every timer tick.
 * @param   pvUser              User argument for the callback.
 * @see     RTTimerLRCreateEx, RTTimerLRDestroy, RTTimerLRStop
 */
RTDECL(int) RTTimerLRCreate(PRTTIMERLR phTimerLR, uint32_t uMilliesInterval, PFNRTTIMERLR pfnTimer, void *pvUser);

/**
 * Create a suspended low resolution timer.
 *
 * @returns iprt status code.
 * @retval  VERR_NOT_SUPPORTED if an unsupported flag was specfied.
 *
 * @param   phTimerLR           Where to store the timer handle.
 * @param   u64NanoInterval     The interval between timer ticks specified in nanoseconds if it's
 *                              a recurring timer, the minimum for is 100000000 ns.
 *                              For one shot timers, pass 0.
 * @param   fFlags              Timer flags. Same as RTTimerCreateEx.
 * @param   pfnTimer            Callback function which shall be scheduled for execution
 *                              on every timer tick.
 * @param   pvUser              User argument for the callback.
 * @see     RTTimerLRStart, RTTimerLRStop, RTTimerLRDestroy
 */
RTDECL(int) RTTimerLRCreateEx(PRTTIMERLR phTimerLR, uint64_t u64NanoInterval, uint32_t fFlags, PFNRTTIMERLR pfnTimer, void *pvUser);

/**
 * Stops and destroys a running low resolution timer.
 *
 * @returns iprt status code.
 * @param   hTimerLR            The low resolution timer to stop and destroy.
 *                              NIL_RTTIMERLR is accepted.
 */
RTDECL(int) RTTimerLRDestroy(RTTIMERLR hTimerLR);

/**
 * Starts a low resolution timer.
 *
 * @returns IPRT status code.
 * @retval  VERR_INVALID_HANDLE if pTimer isn't valid.
 * @retval  VERR_TIMER_ACTIVE if the timer isn't suspended.
 *
 * @param   hTimerLR            The low resolution timer to activate.
 * @param   u64First            The RTTimeSystemNanoTS() for when the timer should start
 *                              firing (relative), the minimum is 100000000 ns.
 *                              If 0 is specified, the timer will fire ASAP.
 *
 * @see     RTTimerLRStop
 */
RTDECL(int) RTTimerLRStart(RTTIMERLR hTimerLR, uint64_t u64First);

/**
 * Stops an active low resolution timer.
 *
 * @returns IPRT status code.
 * @retval  VERR_INVALID_HANDLE if pTimer isn't valid.
 * @retval  VERR_TIMER_SUSPENDED if the timer isn't active.
 * @retval  VERR_NOT_SUPPORTED if the IPRT implementation doesn't support stopping a timer.
 *
 * @param   hTimerLR            The low resolution timer to suspend.
 *
 * @see     RTTimerLRStart
 */
RTDECL(int) RTTimerLRStop(RTTIMERLR hTimerLR);

/** @} */

RT_C_DECLS_END

#endif
