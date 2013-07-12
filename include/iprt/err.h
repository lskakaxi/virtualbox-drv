/** @file
 * IPRT - Status Codes.
 */

/*
 * Copyright (C) 2006-2009 Oracle Corporation
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

#ifndef ___iprt_err_h
#define ___iprt_err_h

#include <iprt/cdefs.h>
#include <iprt/types.h>

RT_C_DECLS_BEGIN

/** @defgroup grp_rt_err            RTErr - Status Codes
 * @ingroup grp_rt
 * @{
 */

/** @defgroup grp_rt_err_hlp        Status Code Helpers
 * @ingroup grp_rt_err
 * @{
 */

#ifdef __cplusplus
/**
 * Strict type validation class.
 *
 * This is only really useful for type checking the arguments to RT_SUCCESS,
 * RT_SUCCESS_NP, RT_FAILURE and RT_FAILURE_NP.  The RTErrStrictType2
 * constructor is for integration with external status code strictness regimes.
 */
class RTErrStrictType
{
protected:
    int32_t m_rc;

public:
    /**
     * Constructor for interaction with external status code strictness regimes.
     *
     * This is a special constructor for helping external return code validator
     * classes interact cleanly with RT_SUCCESS, RT_SUCCESS_NP, RT_FAILURE and
     * RT_FAILURE_NP while barring automatic cast to integer.
     *
     * @param   rcObj       IPRT status code object from an automatic cast.
     */
    RTErrStrictType(RTErrStrictType2 const rcObj)
        : m_rc(rcObj.getValue())
    {
    }

    /**
     * Integer constructor used by RT_SUCCESS_NP.
     *
     * @param   rc          IPRT style status code.
     */
    RTErrStrictType(int32_t rc)
        : m_rc(rc)
    {
    }

#if 0 /** @todo figure where int32_t is long instead of int. */
    /**
     * Integer constructor used by RT_SUCCESS_NP.
     *
     * @param   rc          IPRT style status code.
     */
    RTErrStrictType(signed int rc)
        : m_rc(rc)
    {
    }
#endif

    /**
     * Test for success.
     */
    bool success() const
    {
        return m_rc >= 0;
    }

private:
    /** @name Try ban a number of wrong types.
     * @{ */
    RTErrStrictType(uint8_t rc)         : m_rc(-999) { NOREF(rc); }
    RTErrStrictType(uint16_t rc)        : m_rc(-999) { NOREF(rc); }
    RTErrStrictType(uint32_t rc)        : m_rc(-999) { NOREF(rc); }
    RTErrStrictType(uint64_t rc)        : m_rc(-999) { NOREF(rc); }
    RTErrStrictType(int8_t rc)          : m_rc(-999) { NOREF(rc); }
    RTErrStrictType(int16_t rc)         : m_rc(-999) { NOREF(rc); }
    RTErrStrictType(int64_t rc)         : m_rc(-999) { NOREF(rc); }
    /** @todo fight long here - clashes with int32_t/int64_t on some platforms. */
    /** @} */
};
#endif /* __cplusplus */


/** @def RTERR_STRICT_RC
 * Indicates that RT_SUCCESS_NP, RT_SUCCESS, RT_FAILURE_NP and RT_FAILURE should
 * make type enforcing at compile time.
 *
 * @remarks     Only define this for C++ code.
 */
#if defined(__cplusplus) \
 && !defined(RTERR_STRICT_RC) \
 && (   defined(DOXYGEN_RUNNING) \
     || defined(DEBUG) \
     || defined(RT_STRICT) )
# define RTERR_STRICT_RC        1
#endif


/** @def RT_SUCCESS
 * Check for success. We expect success in normal cases, that is the code path depending on
 * this check is normally taken. To prevent any prediction use RT_SUCCESS_NP instead.
 *
 * @returns true if rc indicates success.
 * @returns false if rc indicates failure.
 *
 * @param   rc  The iprt status code to test.
 */
#define RT_SUCCESS(rc)      ( RT_LIKELY(RT_SUCCESS_NP(rc)) )

/** @def RT_SUCCESS_NP
 * Check for success. Don't predict the result.
 *
 * @returns true if rc indicates success.
 * @returns false if rc indicates failure.
 *
 * @param   rc  The iprt status code to test.
 */
#ifdef RTERR_STRICT_RC
# define RT_SUCCESS_NP(rc)   ( RTErrStrictType(rc).success() )
#else
# define RT_SUCCESS_NP(rc)   ( (int)(rc) >= VINF_SUCCESS )
#endif

/** @def RT_FAILURE
 * Check for failure. We don't expect in normal cases, that is the code path depending on
 * this check is normally NOT taken. To prevent any prediction use RT_FAILURE_NP instead.
 *
 * @returns true if rc indicates failure.
 * @returns false if rc indicates success.
 *
 * @param   rc  The iprt status code to test.
 */
#define RT_FAILURE(rc)      ( RT_UNLIKELY(!RT_SUCCESS_NP(rc)) )

/** @def RT_FAILURE_NP
 * Check for failure. Don't predict the result.
 *
 * @returns true if rc indicates failure.
 * @returns false if rc indicates success.
 *
 * @param   rc  The iprt status code to test.
 */
#define RT_FAILURE_NP(rc)   ( !RT_SUCCESS_NP(rc) )

/**
 * Converts a Darwin HRESULT error to an iprt status code.
 *
 * @returns iprt status code.
 * @param   iNativeCode    HRESULT error code.
 * @remark  Darwin ring-3 only.
 */
RTDECL(int)  RTErrConvertFromDarwinCOM(int32_t iNativeCode);

/**
 * Converts a Darwin IOReturn error to an iprt status code.
 *
 * @returns iprt status code.
 * @param   iNativeCode    IOReturn error code.
 * @remark  Darwin only.
 */
RTDECL(int)  RTErrConvertFromDarwinIO(int iNativeCode);

/**
 * Converts a Darwin kern_return_t error to an iprt status code.
 *
 * @returns iprt status code.
 * @param   iNativeCode    kern_return_t error code.
 * @remark  Darwin only.
 */
RTDECL(int)  RTErrConvertFromDarwinKern(int iNativeCode);

/**
 * Converts a Darwin error to an iprt status code.
 *
 * This will consult RTErrConvertFromDarwinKern, RTErrConvertFromDarwinIO
 * and RTErrConvertFromDarwinCOM in this order. The latter is ring-3 only as it
 * doesn't apply elsewhere.
 *
 * @returns iprt status code.
 * @param   iNativeCode    Darwin error code.
 * @remarks Darwin only.
 * @remarks This is recommended over RTErrConvertFromDarwinKern and RTErrConvertFromDarwinIO
 *          since these are really just subsets of the same error space.
 */
RTDECL(int)  RTErrConvertFromDarwin(int iNativeCode);

/**
 * Converts errno to iprt status code.
 *
 * @returns iprt status code.
 * @param   uNativeCode    errno code.
 */
RTDECL(int)  RTErrConvertFromErrno(unsigned uNativeCode);

/**
 * Converts a L4 errno to a iprt status code.
 *
 * @returns iprt status code.
 * @param   uNativeCode l4 errno.
 * @remark  L4 only.
 */
RTDECL(int)  RTErrConvertFromL4Errno(unsigned uNativeCode);

/**
 * Converts NT status code to iprt status code.
 *
 * Needless to say, this is only available on NT and winXX targets.
 *
 * @returns iprt status code.
 * @param   lNativeCode    NT status code.
 * @remark  Windows only.
 */
RTDECL(int)  RTErrConvertFromNtStatus(long lNativeCode);

/**
 * Converts OS/2 error code to iprt status code.
 *
 * @returns iprt status code.
 * @param   uNativeCode    OS/2 error code.
 * @remark  OS/2 only.
 */
RTDECL(int)  RTErrConvertFromOS2(unsigned uNativeCode);

/**
 * Converts Win32 error code to iprt status code.
 *
 * @returns iprt status code.
 * @param   uNativeCode    Win32 error code.
 * @remark  Windows only.
 */
RTDECL(int)  RTErrConvertFromWin32(unsigned uNativeCode);

/**
 * Converts an iprt status code to a errno status code.
 *
 * @returns errno status code.
 * @param   iErr    iprt status code.
 */
RTDECL(int)  RTErrConvertToErrno(int iErr);


#ifdef IN_RING3

/**
 * iprt status code message.
 */
typedef struct RTSTATUSMSG
{
    /** Pointer to the short message string. */
    const char *pszMsgShort;
    /** Pointer to the full message string. */
    const char *pszMsgFull;
    /** Pointer to the define string. */
    const char *pszDefine;
    /** Status code number. */
    int         iCode;
} RTSTATUSMSG;
/** Pointer to iprt status code message. */
typedef RTSTATUSMSG *PRTSTATUSMSG;
/** Pointer to const iprt status code message. */
typedef const RTSTATUSMSG *PCRTSTATUSMSG;

/**
 * Get the message structure corresponding to a given iprt status code.
 *
 * @returns Pointer to read-only message description.
 * @param   rc      The status code.
 */
RTDECL(PCRTSTATUSMSG) RTErrGet(int rc);

/**
 * Get the define corresponding to a given iprt status code.
 *
 * @returns Pointer to read-only string with the \#define identifier.
 * @param   rc      The status code.
 */
#define RTErrGetDefine(rc)      (RTErrGet(rc)->pszDefine)

/**
 * Get the short description corresponding to a given iprt status code.
 *
 * @returns Pointer to read-only string with the description.
 * @param   rc      The status code.
 */
#define RTErrGetShort(rc)       (RTErrGet(rc)->pszMsgShort)

/**
 * Get the full description corresponding to a given iprt status code.
 *
 * @returns Pointer to read-only string with the description.
 * @param   rc      The status code.
 */
#define RTErrGetFull(rc)        (RTErrGet(rc)->pszMsgFull)

#ifdef RT_OS_WINDOWS
/**
 * Windows error code message.
 */
typedef struct RTWINERRMSG
{
    /** Pointer to the full message string. */
    const char *pszMsgFull;
    /** Pointer to the define string. */
    const char *pszDefine;
    /** Error code number. */
    long        iCode;
} RTWINERRMSG;
/** Pointer to Windows error code message. */
typedef RTWINERRMSG *PRTWINERRMSG;
/** Pointer to const Windows error code message. */
typedef const RTWINERRMSG *PCRTWINERRMSG;

/**
 * Get the message structure corresponding to a given Windows error code.
 *
 * @returns Pointer to read-only message description.
 * @param   rc      The status code.
 */
RTDECL(PCRTWINERRMSG) RTErrWinGet(long rc);

/** On windows COM errors are part of the Windows error database. */
typedef RTWINERRMSG RTCOMERRMSG;

#else  /* !RT_OS_WINDOWS */

/**
 * COM/XPCOM error code message.
 */
typedef struct RTCOMERRMSG
{
    /** Pointer to the full message string. */
    const char *pszMsgFull;
    /** Pointer to the define string. */
    const char *pszDefine;
    /** Error code number. */
    uint32_t    iCode;
} RTCOMERRMSG;
#endif /* !RT_OS_WINDOWS */
/** Pointer to a XPCOM/COM error code message. */
typedef RTCOMERRMSG *PRTCOMERRMSG;
/** Pointer to const a XPCOM/COM error code message. */
typedef const RTCOMERRMSG *PCRTCOMERRMSG;

/**
 * Get the message structure corresponding to a given COM/XPCOM error code.
 *
 * @returns Pointer to read-only message description.
 * @param   rc      The status code.
 */
RTDECL(PCRTCOMERRMSG) RTErrCOMGet(uint32_t rc);

#endif /* IN_RING3 */

/** @} */


/* SED-START */

/** @name Misc. Status Codes
 * @{
 */
/** Success. */
#define VINF_SUCCESS                        0

/** General failure - DON'T USE THIS!!! */
#define VERR_GENERAL_FAILURE                (-1)
/** Invalid parameter. */
#define VERR_INVALID_PARAMETER              (-2)
/** Invalid parameter. */
#define VWRN_INVALID_PARAMETER              2
/** Invalid magic or cookie. */
#define VERR_INVALID_MAGIC                  (-3)
/** Invalid magic or cookie. */
#define VWRN_INVALID_MAGIC                  3
/** Invalid loader handle. */
#define VERR_INVALID_HANDLE                 (-4)
/** Invalid loader handle. */
#define VWRN_INVALID_HANDLE                 4
/** Failed to lock the address range. */
#define VERR_LOCK_FAILED                    (-5)
/** Invalid memory pointer. */
#define VERR_INVALID_POINTER                (-6)
/** Failed to patch the IDT. */
#define VERR_IDT_FAILED                     (-7)
/** Memory allocation failed. */
#define VERR_NO_MEMORY                      (-8)
/** Already loaded. */
#define VERR_ALREADY_LOADED                 (-9)
/** Permission denied. */
#define VERR_PERMISSION_DENIED              (-10)
/** Permission denied. */
#define VINF_PERMISSION_DENIED              10
/** Version mismatch. */
#define VERR_VERSION_MISMATCH               (-11)
/** The request function is not implemented. */
#define VERR_NOT_IMPLEMENTED                (-12)

/** Failed to allocate temporary memory. */
#define VERR_NO_TMP_MEMORY                  (-20)
/** Invalid file mode mask (RTFMODE). */
#define VERR_INVALID_FMODE                  (-21)
/** Incorrect call order. */
#define VERR_WRONG_ORDER                    (-22)
/** There is no TLS (thread local storage) available for storing the current thread. */
#define VERR_NO_TLS_FOR_SELF                (-23)
/** Failed to set the TLS (thread local storage) entry which points to our thread structure. */
#define VERR_FAILED_TO_SET_SELF_TLS         (-24)
/** Not able to allocate contiguous memory. */
#define VERR_NO_CONT_MEMORY                 (-26)
/** No memory available for page table or page directory. */
#define VERR_NO_PAGE_MEMORY                 (-27)
/** Already initialized. */
#define VINF_ALREADY_INITIALIZED            28
/** The specified thread is dead. */
#define VERR_THREAD_IS_DEAD                 (-29)
/** The specified thread is not waitable. */
#define VERR_THREAD_NOT_WAITABLE            (-30)
/** Pagetable not present. */
#define VERR_PAGE_TABLE_NOT_PRESENT         (-31)
/** Invalid context.
 * Typically an API was used by the wrong thread. */
#define VERR_INVALID_CONTEXT                (-32)
/** The per process timer is busy. */
#define VERR_TIMER_BUSY                     (-33)
/** Address conflict. */
#define VERR_ADDRESS_CONFLICT               (-34)
/** Unresolved (unknown) host platform error. */
#define VERR_UNRESOLVED_ERROR               (-35)
/** Invalid function. */
#define VERR_INVALID_FUNCTION               (-36)
/** Not supported. */
#define VERR_NOT_SUPPORTED                  (-37)
/** Access denied. */
#define VERR_ACCESS_DENIED                  (-38)
/** Call interrupted. */
#define VERR_INTERRUPTED                    (-39)
/** Timeout. */
#define VERR_TIMEOUT                        (-40)
/** Buffer too small to save result. */
#define VERR_BUFFER_OVERFLOW                (-41)
/** Buffer too small to save result. */
#define VINF_BUFFER_OVERFLOW                41
/** Data size overflow. */
#define VERR_TOO_MUCH_DATA                  (-42)
/** Max threads number reached. */
#define VERR_MAX_THRDS_REACHED              (-43)
/** Max process number reached. */
#define VERR_MAX_PROCS_REACHED              (-44)
/** The recipient process has refused the signal. */
#define VERR_SIGNAL_REFUSED                 (-45)
/** A signal is already pending. */
#define VERR_SIGNAL_PENDING                 (-46)
/** The signal being posted is not correct. */
#define VERR_SIGNAL_INVALID                 (-47)
/** The state changed.
 * This is a generic error message and needs a context to make sense. */
#define VERR_STATE_CHANGED                  (-48)
/** Warning, the state changed.
 * This is a generic error message and needs a context to make sense. */
#define VWRN_STATE_CHANGED                  48
/** Error while parsing UUID string */
#define VERR_INVALID_UUID_FORMAT            (-49)
/** The specified process was not found. */
#define VERR_PROCESS_NOT_FOUND              (-50)
/** The process specified to a non-block wait had not exited. */
#define VERR_PROCESS_RUNNING                (-51)
/** Retry the operation. */
#define VERR_TRY_AGAIN                      (-52)
/** Retry the operation. */
#define VINF_TRY_AGAIN                      52
/** Generic parse error. */
#define VERR_PARSE_ERROR                    (-53)
/** Value out of range. */
#define VERR_OUT_OF_RANGE                   (-54)
/** A numeric conversion encountered a value which was too big for the target. */
#define VERR_NUMBER_TOO_BIG                 (-55)
/** A numeric conversion encountered a value which was too big for the target. */
#define VWRN_NUMBER_TOO_BIG                 55
/** The number begin converted (string) contained no digits. */
#define VERR_NO_DIGITS                      (-56)
/** The number begin converted (string) contained no digits. */
#define VWRN_NO_DIGITS                      56
/** Encountered a '-' during conversion to an unsigned value. */
#define VERR_NEGATIVE_UNSIGNED              (-57)
/** Encountered a '-' during conversion to an unsigned value. */
#define VWRN_NEGATIVE_UNSIGNED              57
/** Error while characters translation (unicode and so). */
#define VERR_NO_TRANSLATION                 (-58)
/** Encountered unicode code point which is reserved for use as endian indicator (0xffff or 0xfffe). */
#define VERR_CODE_POINT_ENDIAN_INDICATOR    (-59)
/** Encountered unicode code point in the surrogate range (0xd800 to 0xdfff). */
#define VERR_CODE_POINT_SURROGATE           (-60)
/** A string claiming to be UTF-8 is incorrectly encoded. */
#define VERR_INVALID_UTF8_ENCODING          (-61)
/** Ad string claiming to be in UTF-16 is incorrectly encoded. */
#define VERR_INVALID_UTF16_ENCODING         (-62)
/** Encountered a unicode code point which cannot be represented as UTF-16. */
#define VERR_CANT_RECODE_AS_UTF16           (-63)
/** Got an out of memory condition trying to allocate a string. */
#define VERR_NO_STR_MEMORY                  (-64)
/** Got an out of memory condition trying to allocate a UTF-16 (/UCS-2) string. */
#define VERR_NO_UTF16_MEMORY                (-65)
/** Get an out of memory condition trying to allocate a code point array. */
#define VERR_NO_CODE_POINT_MEMORY           (-66)
/** Can't free the memory because it's used in mapping. */
#define VERR_MEMORY_BUSY                    (-67)
/** The timer can't be started because it's already active. */
#define VERR_TIMER_ACTIVE                   (-68)
/** The timer can't be stopped because i's already suspended. */
#define VERR_TIMER_SUSPENDED                (-69)
/** The operation was cancelled by the user (copy) or another thread (local ipc). */
#define VERR_CANCELLED                      (-70)
/** Failed to initialize a memory object.
 * Exactly what this means is OS specific. */
#define VERR_MEMOBJ_INIT_FAILED             (-71)
/** Out of memory condition when allocating memory with low physical backing. */
#define VERR_NO_LOW_MEMORY                  (-72)
/** Out of memory condition when allocating physical memory (without mapping). */
#define VERR_NO_PHYS_MEMORY                 (-73)
/** The address (virtual or physical) is too big. */
#define VERR_ADDRESS_TOO_BIG                (-74)
/** Failed to map a memory object. */
#define VERR_MAP_FAILED                     (-75)
/** Trailing characters. */
#define VERR_TRAILING_CHARS                 (-76)
/** Trailing characters. */
#define VWRN_TRAILING_CHARS                 76
/** Trailing spaces. */
#define VERR_TRAILING_SPACES                (-77)
/** Trailing spaces. */
#define VWRN_TRAILING_SPACES                77
/** Generic not found error. */
#define VERR_NOT_FOUND                      (-78)
/** Generic not found warning. */
#define VWRN_NOT_FOUND                      78
/** Generic invalid state error. */
#define VERR_INVALID_STATE                  (-79)
/** Generic invalid state warning. */
#define VWRN_INVALID_STATE                  79
/** Generic out of resources error. */
#define VERR_OUT_OF_RESOURCES               (-80)
/** Generic out of resources warning. */
#define VWRN_OUT_OF_RESOURCES               80
/** No more handles available, too many open handles. */
#define VERR_NO_MORE_HANDLES                (-81)
/** Preemption is disabled.
 * The requested operation can only be performed when preemption is enabled. */
#define VERR_PREEMPT_DISABLED               (-82)
/** End of string. */
#define VERR_END_OF_STRING                  (-83)
/** End of string. */
#define VINF_END_OF_STRING                  83
/** A page count is out of range. */
#define VERR_PAGE_COUNT_OUT_OF_RANGE        (-84)
/** Generic object destroyed status. */
#define VERR_OBJECT_DESTROYED               (-85)
/** Generic object was destroyed by the call status. */
#define VINF_OBJECT_DESTROYED               85
/** Generic dangling objects status. */
#define VERR_DANGLING_OBJECTS               (-86)
/** Generic dangling objects status. */
#define VWRN_DANGLING_OBJECTS               86
/** Invalid Base64 encoding. */
#define VERR_INVALID_BASE64_ENCODING        (-87)
/** Return instigated by a callback or similar. */
#define VERR_CALLBACK_RETURN                (-88)
/** Return instigated by a callback or similar. */
#define VINF_CALLBACK_RETURN                88
/** Authentication failure. */
#define VERR_AUTHENTICATION_FAILURE         (-89)
/** Not a power of two. */
#define VERR_NOT_POWER_OF_TWO               (-90)
/** Status code, typically given as a parameter, that isn't supposed to be used. */
#define VERR_IGNORED                        (-91)
/** Concurrent access to the object is not allowed. */
#define VERR_CONCURRENT_ACCESS              (-92)
/** The caller does not have a reference to the object.
 * This status is used when two threads is caught sharing the same object
 * reference. */
#define VERR_CALLER_NO_REFERENCE            (-93)
/** Invalid login data given. */
#define VERR_LOGON_FAILURE                  (-94)
/** Generic no change error. */
#define VERR_NO_CHANGE                      (-95)
/** Generic no change info. */
#define VINF_NO_CHANGE                      95
/** Out of memory condition when allocating executable memory. */
#define VERR_NO_EXEC_MEMORY                 (-96)

/** @} */


/** @name Common File/Disk/Pipe/etc Status Codes
 * @{
 */
/** Unresolved (unknown) file i/o error. */
#define VERR_FILE_IO_ERROR                  (-100)
/** File/Device open failed. */
#define VERR_OPEN_FAILED                    (-101)
/** File not found. */
#define VERR_FILE_NOT_FOUND                 (-102)
/** Path not found. */
#define VERR_PATH_NOT_FOUND                 (-103)
/** Invalid (malformed) file/path name. */
#define VERR_INVALID_NAME                   (-104)
/** File/Device already exists. */
#define VERR_ALREADY_EXISTS                 (-105)
/** Too many open files. */
#define VERR_TOO_MANY_OPEN_FILES            (-106)
/** Seek error. */
#define VERR_SEEK                           (-107)
/** Seek below file start. */
#define VERR_NEGATIVE_SEEK                  (-108)
/** Trying to seek on device. */
#define VERR_SEEK_ON_DEVICE                 (-109)
/** Reached the end of the file. */
#define VERR_EOF                            (-110)
/** Reached the end of the file. */
#define VINF_EOF                            110
/** Generic file read error. */
#define VERR_READ_ERROR                     (-111)
/** Generic file write error. */
#define VERR_WRITE_ERROR                    (-112)
/** Write protect error. */
#define VERR_WRITE_PROTECT                  (-113)
/** Sharing violation, file is being used by another process. */
#define VERR_SHARING_VIOLATION              (-114)
/** Unable to lock a region of a file. */
#define VERR_FILE_LOCK_FAILED               (-115)
/** File access error, another process has locked a portion of the file. */
#define VERR_FILE_LOCK_VIOLATION            (-116)
/** File or directory can't be created. */
#define VERR_CANT_CREATE                    (-117)
/** Directory can't be deleted. */
#define VERR_CANT_DELETE_DIRECTORY          (-118)
/** Can't move file to another disk. */
#define VERR_NOT_SAME_DEVICE                (-119)
/** The filename or extension is too long. */
#define VERR_FILENAME_TOO_LONG              (-120)
/** Media not present in drive. */
#define VERR_MEDIA_NOT_PRESENT              (-121)
/** The type of media was not recognized. Not formatted? */
#define VERR_MEDIA_NOT_RECOGNIZED           (-122)
/** Can't unlock - region was not locked. */
#define VERR_FILE_NOT_LOCKED                (-123)
/** Unrecoverable error: lock was lost. */
#define VERR_FILE_LOCK_LOST                 (-124)
/** Can't delete directory with files. */
#define VERR_DIR_NOT_EMPTY                  (-125)
/** A directory operation was attempted on a non-directory object. */
#define VERR_NOT_A_DIRECTORY                (-126)
/** A non-directory operation was attempted on a directory object. */
#define VERR_IS_A_DIRECTORY                 (-127)
/** Tried to grow a file beyond the limit imposed by the process or the filesystem. */
#define VERR_FILE_TOO_BIG                   (-128)
/** No pending request the aio context has to wait for completion. */
#define VERR_FILE_AIO_NO_REQUEST            (-129)
/** The request could not be canceled or prepared for another transfer
 *  because it is still in progress. */
#define VERR_FILE_AIO_IN_PROGRESS           (-130)
/** The request could not be canceled because it already completed. */
#define VERR_FILE_AIO_COMPLETED             (-131)
/** The I/O context couldn't be destroyed because there are still pending requests. */
#define VERR_FILE_AIO_BUSY                  (-132)
/** The requests couldn't be submitted because that would exceed the capacity of the context. */
#define VERR_FILE_AIO_LIMIT_EXCEEDED        (-133)
/** The request was canceled. */
#define VERR_FILE_AIO_CANCELED              (-134)
/** The request wasn't submitted so it can't be canceled. */
#define VERR_FILE_AIO_NOT_SUBMITTED         (-135)
/** A request was not prepared and thus could not be submitted. */
#define VERR_FILE_AIO_NOT_PREPARED          (-136)
/** Not all requests could be submitted due to resource shortage. */
#define VERR_FILE_AIO_INSUFFICIENT_RESSOURCES (-137)
/** Device or resource is busy. */
#define VERR_RESOURCE_BUSY                  (-138)
/** @} */


/** @name Generic Filesystem I/O Status Codes
 * @{
 */
/** Unresolved (unknown) disk i/o error.  */
#define VERR_DISK_IO_ERROR                  (-150)
/** Invalid drive number. */
#define VERR_INVALID_DRIVE                  (-151)
/** Disk is full. */
#define VERR_DISK_FULL                      (-152)
/** Disk was changed. */
#define VERR_DISK_CHANGE                    (-153)
/** Drive is locked. */
#define VERR_DRIVE_LOCKED                   (-154)
/** The specified disk or diskette cannot be accessed. */
#define VERR_DISK_INVALID_FORMAT            (-155)
/** Too many symbolic links. */
#define VERR_TOO_MANY_SYMLINKS              (-156)
/** The OS does not support setting the time stamps on a symbolic link. */
#define VERR_NS_SYMLINK_SET_TIME            (-157)
/** @} */


/** @name Generic Directory Enumeration Status Codes
 * @{
 */
/** Unresolved (unknown) search error. */
#define VERR_SEARCH_ERROR                   (-200)
/** No more files found. */
#define VERR_NO_MORE_FILES                  (-201)
/** No more search handles available. */
#define VERR_NO_MORE_SEARCH_HANDLES         (-202)
/** RTDirReadEx() failed to retrieve the extra data which was requested. */
#define VWRN_NO_DIRENT_INFO                 203
/** @} */


/** @name Internal Processing Errors
 * @{
 */
/** Internal error - we're screwed if this happens.  */
#define VERR_INTERNAL_ERROR                 (-225)
/** Internal error no. 2. */
#define VERR_INTERNAL_ERROR_2               (-226)
/** Internal error no. 3. */
#define VERR_INTERNAL_ERROR_3               (-227)
/** Internal error no. 4. */
#define VERR_INTERNAL_ERROR_4               (-228)
/** Internal error no. 5. */
#define VERR_INTERNAL_ERROR_5               (-229)
/** Internal error: Unexpected status code. */
#define VERR_IPE_UNEXPECTED_STATUS          (-230)
/** Internal error: Unexpected status code. */
#define VERR_IPE_UNEXPECTED_INFO_STATUS     (-231)
/** Internal error: Unexpected status code. */
#define VERR_IPE_UNEXPECTED_ERROR_STATUS    (-232)
/** Internal error: Uninitialized status code.
 * @remarks This is used by value elsewhere.  */
#define VERR_IPE_UNINITIALIZED_STATUS       (-233)
/** @} */


/** @name Generic Device I/O Status Codes
 * @{
 */
/** Unresolved (unknown) device i/o error. */
#define VERR_DEV_IO_ERROR                   (-250)
/** Device i/o: Bad unit. */
#define VERR_IO_BAD_UNIT                    (-251)
/** Device i/o: Not ready. */
#define VERR_IO_NOT_READY                   (-252)
/** Device i/o: Bad command. */
#define VERR_IO_BAD_COMMAND                 (-253)
/** Device i/o: CRC error. */
#define VERR_IO_CRC                         (-254)
/** Device i/o: Bad length. */
#define VERR_IO_BAD_LENGTH                  (-255)
/** Device i/o: Sector not found. */
#define VERR_IO_SECTOR_NOT_FOUND            (-256)
/** Device i/o: General failure. */
#define VERR_IO_GEN_FAILURE                 (-257)
/** @} */


/** @name Generic Pipe I/O Status Codes
 * @{
 */
/** Unresolved (unknown) pipe i/o error. */
#define VERR_PIPE_IO_ERROR                  (-300)
/** Broken pipe. */
#define VERR_BROKEN_PIPE                    (-301)
/** Bad pipe. */
#define VERR_BAD_PIPE                       (-302)
/** Pipe is busy. */
#define VERR_PIPE_BUSY                      (-303)
/** No data in pipe. */
#define VERR_NO_DATA                        (-304)
/** Pipe is not connected. */
#define VERR_PIPE_NOT_CONNECTED             (-305)
/** More data available in pipe. */
#define VERR_MORE_DATA                      (-306)
/** @} */


/** @name Generic Semaphores Status Codes
 * @{
 */
/** Unresolved (unknown) semaphore error. */
#define VERR_SEM_ERROR                      (-350)
/** Too many semaphores. */
#define VERR_TOO_MANY_SEMAPHORES            (-351)
/** Exclusive semaphore is owned by another process. */
#define VERR_EXCL_SEM_ALREADY_OWNED         (-352)
/** The semaphore is set and cannot be closed. */
#define VERR_SEM_IS_SET                     (-353)
/** The semaphore cannot be set again. */
#define VERR_TOO_MANY_SEM_REQUESTS          (-354)
/** Attempt to release mutex not owned by caller. */
#define VERR_NOT_OWNER                      (-355)
/** The semaphore has been opened too many times. */
#define VERR_TOO_MANY_OPENS                 (-356)
/** The maximum posts for the event semaphore has been reached. */
#define VERR_TOO_MANY_POSTS                 (-357)
/** The event semaphore has already been posted. */
#define VERR_ALREADY_POSTED                 (-358)
/** The event semaphore has already been reset. */
#define VERR_ALREADY_RESET                  (-359)
/** The semaphore is in use. */
#define VERR_SEM_BUSY                       (-360)
/** The previous ownership of this semaphore has ended. */
#define VERR_SEM_OWNER_DIED                 (-361)
/** Failed to open semaphore by name - not found. */
#define VERR_SEM_NOT_FOUND                  (-362)
/** Semaphore destroyed while waiting. */
#define VERR_SEM_DESTROYED                  (-363)
/** Nested ownership requests are not permitted for this semaphore type. */
#define VERR_SEM_NESTED                     (-364)
/** Deadlock detected. */
#define VERR_DEADLOCK                       (-365)
/** Ping-Pong listen or speak out of turn error. */
#define VERR_SEM_OUT_OF_TURN                (-366)
/** Tried to take a semaphore in a bad context. */
#define VERR_SEM_BAD_CONTEXT                (-367)
/** Don't spin for the semaphore, but it is safe to try grab it. */
#define VINF_SEM_BAD_CONTEXT                (367)
/** Wrong locking order detected. */
#define VERR_SEM_LV_WRONG_ORDER             (-368)
/** Wrong release order detected. */
#define VERR_SEM_LV_WRONG_RELEASE_ORDER     (-369)
/** Attempt to recursively enter a non-recurisve lock. */
#define VERR_SEM_LV_NESTED                  (-370)
/** Invalid parameters passed to the lock validator. */
#define VERR_SEM_LV_INVALID_PARAMETER       (-371)
/** The lock validator detected a deadlock. */
#define VERR_SEM_LV_DEADLOCK                (-372)
/** The lock validator detected an existing deadlock.
 * The deadlock was not caused by the current operation, but existed already. */
#define VERR_SEM_LV_EXISTING_DEADLOCK       (-373)
/** Not the lock owner according our records. */
#define VERR_SEM_LV_NOT_OWNER               (-374)
/** An illegal lock upgrade was attempted. */
#define VERR_SEM_LV_ILLEGAL_UPGRADE         (-375)
/** The thread is not a valid signaller of the event. */
#define VERR_SEM_LV_NOT_SIGNALLER           (-376)
/** Internal error in the lock validator or related components. */
#define VERR_SEM_LV_INTERNAL_ERROR          (-377)
/** @} */


/** @name Generic Network I/O Status Codes
 * @{
 */
/** Unresolved (unknown) network error. */
#define VERR_NET_IO_ERROR                       (-400)
/** The network is busy or is out of resources. */
#define VERR_NET_OUT_OF_RESOURCES               (-401)
/** Net host name not found. */
#define VERR_NET_HOST_NOT_FOUND                 (-402)
/** Network path not found. */
#define VERR_NET_PATH_NOT_FOUND                 (-403)
/** General network printing error. */
#define VERR_NET_PRINT_ERROR                    (-404)
/** The machine is not on the network. */
#define VERR_NET_NO_NETWORK                     (-405)
/** Name is not unique on the network. */
#define VERR_NET_NOT_UNIQUE_NAME                (-406)

/* These are BSD networking error codes - numbers correspond, don't mess! */
/** Operation in progress. */
#define VERR_NET_IN_PROGRESS                    (-436)
/** Operation already in progress. */
#define VERR_NET_ALREADY_IN_PROGRESS            (-437)
/** Attempted socket operation with a non-socket handle.
 * (This includes closed handles.) */
#define VERR_NET_NOT_SOCKET                     (-438)
/** Destination address required. */
#define VERR_NET_DEST_ADDRESS_REQUIRED          (-439)
/** Message too long. */
#define VERR_NET_MSG_SIZE                       (-440)
/** Protocol wrong type for socket. */
#define VERR_NET_PROTOCOL_TYPE                  (-441)
/** Protocol not available. */
#define VERR_NET_PROTOCOL_NOT_AVAILABLE         (-442)
/** Protocol not supported. */
#define VERR_NET_PROTOCOL_NOT_SUPPORTED         (-443)
/** Socket type not supported. */
#define VERR_NET_SOCKET_TYPE_NOT_SUPPORTED      (-444)
/** Operation not supported. */
#define VERR_NET_OPERATION_NOT_SUPPORTED        (-445)
/** Protocol family not supported. */
#define VERR_NET_PROTOCOL_FAMILY_NOT_SUPPORTED  (-446)
/** Address family not supported by protocol family. */
#define VERR_NET_ADDRESS_FAMILY_NOT_SUPPORTED   (-447)
/** Address already in use. */
#define VERR_NET_ADDRESS_IN_USE                 (-448)
/** Can't assign requested address. */
#define VERR_NET_ADDRESS_NOT_AVAILABLE          (-449)
/** Network is down. */
#define VERR_NET_DOWN                           (-450)
/** Network is unreachable. */
#define VERR_NET_UNREACHABLE                    (-451)
/** Network dropped connection on reset. */
#define VERR_NET_CONNECTION_RESET               (-452)
/** Software caused connection abort. */
#define VERR_NET_CONNECTION_ABORTED             (-453)
/** Connection reset by peer. */
#define VERR_NET_CONNECTION_RESET_BY_PEER       (-454)
/** No buffer space available. */
#define VERR_NET_NO_BUFFER_SPACE                (-455)
/** Socket is already connected. */
#define VERR_NET_ALREADY_CONNECTED              (-456)
/** Socket is not connected. */
#define VERR_NET_NOT_CONNECTED                  (-457)
/** Can't send after socket shutdown. */
#define VERR_NET_SHUTDOWN                       (-458)
/** Too many references: can't splice. */
#define VERR_NET_TOO_MANY_REFERENCES            (-459)
/** Too many references: can't splice. */
#define VERR_NET_CONNECTION_TIMED_OUT           (-460)
/** Connection refused. */
#define VERR_NET_CONNECTION_REFUSED             (-461)
/* ELOOP is not net. */
/* ENAMETOOLONG is not net. */
/** Host is down. */
#define VERR_NET_HOST_DOWN                      (-464)
/** No route to host. */
#define VERR_NET_HOST_UNREACHABLE               (-465)
/** Protocol error. */
#define VERR_NET_PROTOCOL_ERROR                 (-466)
/** @} */


/** @name TCP Status Codes
 * @{
 */
/** Stop the TCP server. */
#define VERR_TCP_SERVER_STOP                    (-500)
/** The server was stopped. */
#define VINF_TCP_SERVER_STOP                    500
/** The TCP server was shut down using RTTcpServerShutdown. */
#define VERR_TCP_SERVER_SHUTDOWN                (-501)
/** The TCP server was destroyed. */
#define VERR_TCP_SERVER_DESTROYED               (-502)
/** The TCP server has no client associated with it. */
#define VINF_TCP_SERVER_NO_CLIENT               503
/** @} */


/** @name L4 Specific Status Codes
 * @{
 */
/** Invalid offset in an L4 dataspace */
#define VERR_L4_INVALID_DS_OFFSET               (-550)
/** IPC error */
#define VERR_IPC                                (-551)
/** Item already used */
#define VERR_RESOURCE_IN_USE                    (-552)
/** Source/destination not found */
#define VERR_IPC_PROCESS_NOT_FOUND              (-553)
/** Receive timeout */
#define VERR_IPC_RECEIVE_TIMEOUT                (-554)
/** Send timeout */
#define VERR_IPC_SEND_TIMEOUT                   (-555)
/** Receive cancelled */
#define VERR_IPC_RECEIVE_CANCELLED              (-556)
/** Send cancelled */
#define VERR_IPC_SEND_CANCELLED                 (-557)
/** Receive aborted */
#define VERR_IPC_RECEIVE_ABORTED                (-558)
/** Send aborted */
#define VERR_IPC_SEND_ABORTED                   (-559)
/** Couldn't map pages during receive */
#define VERR_IPC_RECEIVE_MAP_FAILED             (-560)
/** Couldn't map pages during send */
#define VERR_IPC_SEND_MAP_FAILED                (-561)
/** Send pagefault timeout in receive */
#define VERR_IPC_RECEIVE_SEND_PF_TIMEOUT        (-562)
/** Send pagefault timeout in send */
#define VERR_IPC_SEND_SEND_PF_TIMEOUT           (-563)
/** (One) receive buffer was too small, or too few buffers */
#define VINF_IPC_RECEIVE_MSG_CUT                564
/** (One) send buffer was too small, or too few buffers */
#define VINF_IPC_SEND_MSG_CUT                   565
/** Dataspace manager server not found */
#define VERR_L4_DS_MANAGER_NOT_FOUND            (-566)
/** @} */


/** @name Loader Status Codes.
 * @{
 */
/** Invalid executable signature. */
#define VERR_INVALID_EXE_SIGNATURE              (-600)
/** The iprt loader recognized a ELF image, but doesn't support loading it. */
#define VERR_ELF_EXE_NOT_SUPPORTED              (-601)
/** The iprt loader recognized a PE image, but doesn't support loading it. */
#define VERR_PE_EXE_NOT_SUPPORTED               (-602)
/** The iprt loader recognized a LX image, but doesn't support loading it. */
#define VERR_LX_EXE_NOT_SUPPORTED               (-603)
/** The iprt loader recognized a LE image, but doesn't support loading it. */
#define VERR_LE_EXE_NOT_SUPPORTED               (-604)
/** The iprt loader recognized a NE image, but doesn't support loading it. */
#define VERR_NE_EXE_NOT_SUPPORTED               (-605)
/** The iprt loader recognized a MZ image, but doesn't support loading it. */
#define VERR_MZ_EXE_NOT_SUPPORTED               (-606)
/** The iprt loader recognized an a.out image, but doesn't support loading it. */
#define VERR_AOUT_EXE_NOT_SUPPORTED             (-607)
/** Bad executable. */
#define VERR_BAD_EXE_FORMAT                     (-608)
/** Symbol (export) not found. */
#define VERR_SYMBOL_NOT_FOUND                   (-609)
/** Module not found. */
#define VERR_MODULE_NOT_FOUND                   (-610)
/** The loader resolved an external symbol to an address to big for the image format. */
#define VERR_SYMBOL_VALUE_TOO_BIG               (-611)
/** The image is too big. */
#define VERR_IMAGE_TOO_BIG                      (-612)
/** The image base address is to high for this image type. */
#define VERR_IMAGE_BASE_TOO_HIGH                (-614)
/** Mismatching architecture. */
#define VERR_LDR_ARCH_MISMATCH                  (-615)
/** Mismatch between IPRT and native loader. */
#define VERR_LDR_MISMATCH_NATIVE                (-616)
/** Failed to resolve an imported (external) symbol. */
#define VERR_LDR_IMPORTED_SYMBOL_NOT_FOUND      (-617)
/** Generic loader failure. */
#define VERR_LDR_GENERAL_FAILURE                (-618)
/** Code signing error.  */
#define VERR_LDR_IMAGE_HASH                     (-619)
/** The PE loader encountered delayed imports, a feature which hasn't been implemented yet. */
#define VERR_LDRPE_DELAY_IMPORT                 (-620)
/** The PE loader encountered a malformed certificate. */
#define VERR_LDRPE_CERT_MALFORMED               (-621)
/** The PE loader encountered a certificate with an unsupported type or structure revision. */
#define VERR_LDRPE_CERT_UNSUPPORTED             (-622)
/** The PE loader doesn't know how to deal with the global pointer data directory entry yet. */
#define VERR_LDRPE_GLOBALPTR                    (-623)
/** The PE loader doesn't support the TLS data directory yet. */
#define VERR_LDRPE_TLS                          (-624)
/** The PE loader doesn't grok the COM descriptor data directory entry. */
#define VERR_LDRPE_COM_DESCRIPTOR               (-625)
/** The PE loader encountered an unknown load config directory/header size. */
#define VERR_LDRPE_LOAD_CONFIG_SIZE             (-626)
/** The PE loader encountered a lock prefix table, a feature which hasn't been implemented yet. */
#define VERR_LDRPE_LOCK_PREFIX_TABLE            (-627)
/** The ELF loader doesn't handle foreign endianness. */
#define VERR_LDRELF_ODD_ENDIAN                  (-630)
/** The ELF image is 'dynamic', the ELF loader can only deal with 'relocatable' images at present. */
#define VERR_LDRELF_DYN                         (-631)
/** The ELF image is 'executable', the ELF loader can only deal with 'relocatable' images at present. */
#define VERR_LDRELF_EXEC                        (-632)
/** The ELF image was created for an unsupported target machine type. */
#define VERR_LDRELF_MACHINE                     (-633)
/** The ELF version is not supported. */
#define VERR_LDRELF_VERSION                     (-634)
/** The ELF loader cannot handle multiple SYMTAB sections. */
#define VERR_LDRELF_MULTIPLE_SYMTABS            (-635)
/** The ELF loader encountered a relocation type which is not implemented. */
#define VERR_LDRELF_RELOCATION_NOT_SUPPORTED    (-636)
/** The ELF loader encountered a bad symbol index. */
#define VERR_LDRELF_INVALID_SYMBOL_INDEX        (-637)
/** The ELF loader encountered an invalid symbol name offset. */
#define VERR_LDRELF_INVALID_SYMBOL_NAME_OFFSET  (-638)
/** The ELF loader encountered an invalid relocation offset. */
#define VERR_LDRELF_INVALID_RELOCATION_OFFSET   (-639)
/** The ELF loader didn't find the symbol/string table for the image. */
#define VERR_LDRELF_NO_SYMBOL_OR_NO_STRING_TABS (-640)
/** @}*/

/** @name Debug Info Reader Status Codes.
 * @{
 */
/** The module contains no line number information. */
#define VERR_DBG_NO_LINE_NUMBERS                (-650)
/** The module contains no symbol information. */
#define VERR_DBG_NO_SYMBOLS                     (-651)
/** The specified segment:offset address was invalid. Typically an attempt at
 * addressing outside the segment boundary. */
#define VERR_DBG_INVALID_ADDRESS                (-652)
/** Invalid segment index. */
#define VERR_DBG_INVALID_SEGMENT_INDEX          (-653)
/** Invalid segment offset. */
#define VERR_DBG_INVALID_SEGMENT_OFFSET         (-654)
/** Invalid image relative virtual address. */
#define VERR_DBG_INVALID_RVA                    (-655)
/** Invalid image relative virtual address. */
#define VERR_DBG_SPECIAL_SEGMENT                (-656)
/** Address conflict within a module/segment.
 * Attempted to add a segment, symbol or line number that fully or partially
 * overlaps with an existing one. */
#define VERR_DBG_ADDRESS_CONFLICT               (-657)
/** Duplicate symbol within the module.
 * Attempted to add a symbol which name already exists within the module.  */
#define VERR_DBG_DUPLICATE_SYMBOL               (-658)
/** The segment index specified when adding a new segment is already in use. */
#define VERR_DBG_SEGMENT_INDEX_CONFLICT         (-659)
/** No line number was found for the specified address/ordinal/whatever. */
#define VERR_DBG_LINE_NOT_FOUND                 (-660)
/** The length of the symbol name is out of range.
 * This means it is an empty string or that it's greater or equal to
 * RTDBG_SYMBOL_NAME_LENGTH. */
#define VERR_DBG_SYMBOL_NAME_OUT_OF_RANGE       (-661)
/** The length of the file name is out of range.
 * This means it is an empty string or that it's greater or equal to
 * RTDBG_FILE_NAME_LENGTH. */
#define VERR_DBG_FILE_NAME_OUT_OF_RANGE         (-662)
/** The length of the segment name is out of range.
 * This means it is an empty string or that it is greater or equal to
 * RTDBG_SEGMENT_NAME_LENGTH. */
#define VERR_DBG_SEGMENT_NAME_OUT_OF_RANGE      (-663)
/** The specified address range wraps around. */
#define VERR_DBG_ADDRESS_WRAP                   (-664)
/** The file is not a valid NM map file. */
#define VERR_DBG_NOT_NM_MAP_FILE                (-665)
/** The file is not a valid /proc/kallsyms file. */
#define VERR_DBG_NOT_LINUX_KALLSYMS             (-666)
/** No debug module interpreter matching the debug info. */
#define VERR_DBG_NO_MATCHING_INTERPRETER        (-667)
/** @} */

/** @name Request Packet Status Codes.
 * @{
 */
/** Invalid RT request type.
 * For the RTReqAlloc() case, the caller just specified an illegal enmType. For
 * all the other occurrences it means indicates corruption, broken logic, or stupid
 * interface user. */
#define VERR_RT_REQUEST_INVALID_TYPE            (-700)
/** Invalid RT request state.
 * The state of the request packet was not the expected and accepted one(s). Either
 * the interface user screwed up, or we've got corruption/broken logic. */
#define VERR_RT_REQUEST_STATE                   (-701)
/** Invalid RT request packet.
 * One or more of the RT controlled packet members didn't contain the correct
 * values. Some thing's broken. */
#define VERR_RT_REQUEST_INVALID_PACKAGE         (-702)
/** The status field has not been updated yet as the request is still
 * pending completion. Someone queried the iStatus field before the request
 * has been fully processed. */
#define VERR_RT_REQUEST_STATUS_STILL_PENDING    (-703)
/** The request has been freed, don't read the status now.
 * Someone is reading the iStatus field of a freed request packet. */
#define VERR_RT_REQUEST_STATUS_FREED            (-704)
/** @} */

/** @name Environment Status Code
 * @{
 */
/** The specified environment variable was not found. (RTEnvGetEx) */
#define VERR_ENV_VAR_NOT_FOUND                  (-750)
/** The specified environment variable was not found. (RTEnvUnsetEx) */
#define VINF_ENV_VAR_NOT_FOUND                  (750)
/** @} */

/** @name Multiprocessor Status Codes.
 * @{
 */
/** The specified cpu is offline. */
#define VERR_CPU_OFFLINE                        (-800)
/** The specified cpu was not found. */
#define VERR_CPU_NOT_FOUND                      (-801)
/** @} */

/** @name RTGetOpt status codes
 * @{ */
/** RTGetOpt: Command line option not recognized. */
#define VERR_GETOPT_UNKNOWN_OPTION              (-825)
/** RTGetOpt: Command line option needs argument. */
#define VERR_GETOPT_REQUIRED_ARGUMENT_MISSING   (-826)
/** RTGetOpt: Command line option has argument with bad format. */
#define VERR_GETOPT_INVALID_ARGUMENT_FORMAT     (-827)
/** RTGetOpt: Not an option. */
#define VINF_GETOPT_NOT_OPTION                  828
/** RTGetOpt: Command line option needs an index. */
#define VERR_GETOPT_INDEX_MISSING               (-829)
/** @} */

/** @name RTCache status codes
 * @{ */
/** RTCache: cache is full. */
#define VERR_CACHE_FULL                         (-850)
/** RTCache: cache is empty. */
#define VERR_CACHE_EMPTY                        (-851)
/** @} */

/** @name RTMemCache status codes
 * @{ */
/** Reached the max cache size. */
#define VERR_MEM_CACHE_MAX_SIZE                 (-855)
/** @} */

/** @name RTS3 status codes
 * @{ */
/** Access denied error. */
#define VERR_S3_ACCESS_DENIED                   (-875)
/** The bucket/key wasn't found. */
#define VERR_S3_NOT_FOUND                       (-876)
/** Bucket already exists. */
#define VERR_S3_BUCKET_ALREADY_EXISTS           (-877)
/** Can't delete bucket with keys. */
#define VERR_S3_BUCKET_NOT_EMPTY                (-878)
/** The current operation was canceled. */
#define VERR_S3_CANCELED                        (-879)
/** @} */

/** @name RTManifest status codes
 * @{ */
/** A digest type used in the manifest file isn't supported. */
#define VERR_MANIFEST_UNSUPPORTED_DIGEST_TYPE   (-900)
/** An entry in the manifest file couldn't be interpreted correctly. */
#define VERR_MANIFEST_WRONG_FILE_FORMAT         (-901)
/** A digest doesn't match the corresponding file. */
#define VERR_MANIFEST_DIGEST_MISMATCH           (-902)
/** The file list doesn't match to the content of the manifest file. */
#define VERR_MANIFEST_FILE_MISMATCH             (-903)
/** @} */

/** @name RTTar status codes
 * @{ */
/** The checksum of a tar header record doesn't match. */
#define VERR_TAR_CHKSUM_MISMATCH                (-925)
/** @} */

/** @name RTPoll status codes
 * @{ */
/** The handle is not pollable. */
#define VERR_POLL_HANDLE_NOT_POLLABLE           (-950)
/** The handle ID is already present in the poll set. */
#define VERR_POLL_HANDLE_ID_EXISTS              (-951)
/** The handle ID was not found in the set. */
#define VERR_POLL_HANDLE_ID_NOT_FOUND           (-952)
/** The poll set is full. */
#define VERR_POLL_SET_IS_FULL                   (-953)
/** @} */

/* SED-END */

/** @} */

RT_C_DECLS_END

#endif

