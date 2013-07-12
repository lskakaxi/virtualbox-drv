/* $Id: strformatrt.cpp $ */
/** @file
 * IPRT - IPRT String Formatter Extensions.
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

/** @page pg_rt_str_format_rt   The IPRT String Format Extensions
 *
 * The string formatter supports most of the non-float format types and flags.
 * See RTStrFormatV() for the full tail there. In addition we've added a number
 * of iprt specific format types for the iprt typedefs and other useful stuff.
 * Note that several of these are similar to \%p and doesn't care much if you try
 * add formating flags/width/precision.
 *
 *
 * Group 1, the basic runtime typedefs (excluding those which obviously are pointer).
 *      - \%RTbool          - Takes a bool value and prints 'true', 'false', or '!%d!'.
 *      - \%RTfile          - Takes a #RTFILE value.
 *      - \%RTfmode         - Takes a #RTFMODE value.
 *      - \%RTfoff          - Takes a #RTFOFF value.
 *      - \%RTfp16          - Takes a #RTFAR16 value.
 *      - \%RTfp32          - Takes a #RTFAR32 value.
 *      - \%RTfp64          - Takes a #RTFAR64 value.
 *      - \%RTgid           - Takes a #RTGID value.
 *      - \%RTino           - Takes a #RTINODE value.
 *      - \%RTint           - Takes a #RTINT value.
 *      - \%RTiop           - Takes a #RTIOPORT value.
 *      - \%RTldrm          - Takes a #RTLDRMOD value.
 *      - \%RTmac           - Takes a #PCRTMAC pointer.
 *      - \%RTnaddr         - Takes a #PCRTNETADDR value.
 *      - \%RTnaipv4        - Takes a #RTNETADDRIPV4 value.
 *      - \%RTnaipv6        - Takes a #PCRTNETADDRIPV6 value.
 *      - \%RTnthrd         - Takes a #RTNATIVETHREAD value.
 *      - \%RTnthrd         - Takes a #RTNATIVETHREAD value.
 *      - \%RTproc          - Takes a #RTPROCESS value.
 *      - \%RTptr           - Takes a #RTINTPTR or #RTUINTPTR value (but not void *).
 *      - \%RTreg           - Takes a #RTCCUINTREG value.
 *      - \%RTsel           - Takes a #RTSEL value.
 *      - \%RTsem           - Takes a #RTSEMEVENT, #RTSEMEVENTMULTI, #RTSEMMUTEX, #RTSEMFASTMUTEX, or #RTSEMRW value.
 *      - \%RTsock          - Takes a #RTSOCKET value.
 *      - \%RTthrd          - Takes a #RTTHREAD value.
 *      - \%RTuid           - Takes a #RTUID value.
 *      - \%RTuint          - Takes a #RTUINT value.
 *      - \%RTunicp         - Takes a #RTUNICP value.
 *      - \%RTutf16         - Takes a #RTUTF16 value.
 *      - \%RTuuid          - Takes a #PCRTUUID and will print the UUID as a string.
 *      - \%RTxuint         - Takes a #RTUINT or #RTINT value, formatting it as hex.
 *      - \%RGi             - Takes a #RTGCINT value.
 *      - \%RGp             - Takes a #RTGCPHYS value.
 *      - \%RGr             - Takes a #RTGCUINTREG value.
 *      - \%RGu             - Takes a #RTGCUINT value.
 *      - \%RGv             - Takes a #RTGCPTR, #RTGCINTPTR or #RTGCUINTPTR value.
 *      - \%RGx             - Takes a #RTGCUINT or #RTGCINT value, formatting it as hex.
 *      - \%RHi             - Takes a #RTHCINT value.
 *      - \%RHp             - Takes a #RTHCPHYS value.
 *      - \%RHr             - Takes a #RTHCUINTREG value.
 *      - \%RHu             - Takes a #RTHCUINT value.
 *      - \%RHv             - Takes a #RTHCPTR, #RTHCINTPTR or #RTHCUINTPTR value.
 *      - \%RHx             - Takes a #RTHCUINT or #RTHCINT value, formatting it as hex.
 *      - \%RRv             - Takes a #RTRCPTR, #RTRCINTPTR or #RTRCUINTPTR value.
 *      - \%RCi             - Takes a #RTINT value.
 *      - \%RCp             - Takes a #RTCCPHYS value.
 *      - \%RCr             - Takes a #RTCCUINTREG value.
 *      - \%RCu             - Takes a #RTUINT value.
 *      - \%RCv             - Takes a #uintptr_t, #intptr_t, void * value.
 *      - \%RCx             - Takes a #RTUINT or #RTINT value, formatting it as hex.
 *
 *
 * Group 2, the generic integer types which are prefered over relying on what
 * bit-count a 'long', 'short',  or 'long long' has on a platform. This are
 * highly prefered for the [u]intXX_t kind of types.
 *      - \%RI[8|16|32|64]  - Signed integer value of the specifed bit count.
 *      - \%RU[8|16|32|64]  - Unsigned integer value of the specifed bit count.
 *      - \%RX[8|16|32|64]  - Hexadecimal integer value of the specifed bit count.
 *
 *
 * Group 3, hex dumpers and other complex stuff which requires more than simple formatting.
 *      - \%Rhxd            - Takes a pointer to the memory which is to be dumped in typical
 *                            hex format. Use the precision to specify the length, and the width to
 *                            set the number of bytes per line. Default width and precision is 16.
 *      - \%Rhxs            - Takes a pointer to the memory to be displayed as a hex string,
 *                            i.e. a series of space separated bytes formatted as two digit hex value.
 *                            Use the precision to specify the length. Default length is 16 bytes.
 *                            The width, if specified, is ignored.
 *      - \%Rrc             - Takes an integer iprt status code as argument. Will insert the
 *                            status code define corresponding to the iprt status code.
 *      - \%Rrs             - Takes an integer iprt status code as argument. Will insert the
 *                            short description of the specified status code.
 *      - \%Rrf             - Takes an integer iprt status code as argument. Will insert the
 *                            full description of the specified status code.
 *      - \%Rra             - Takes an integer iprt status code as argument. Will insert the
 *                            status code define + full description.
 *      - \%Rwc             - Takes a long Windows error code as argument. Will insert the status
 *                            code define corresponding to the Windows error code.
 *      - \%Rwf             - Takes a long Windows error code as argument. Will insert the
 *                            full description of the specified status code.
 *      - \%Rwa             - Takes a long Windows error code as argument. Will insert the
 *                            error code define + full description.
 *
 *      - \%Rhrc            - Takes a COM/XPCOM status code as argument. Will insert the status
 *                            code define corresponding to the Windows error code.
 *      - \%Rhrf            - Takes a COM/XPCOM status code as argument. Will insert the
 *                            full description of the specified status code.
 *      - \%Rhra            - Takes a COM/XPCOM error code as argument. Will insert the
 *                            error code define + full description.
 *
 *      - \%Rfn             - Pretty printing of a function or method. It drops the
 *                            return code and parameter list.
 *      - \%Rbn             - Prints the base name.  For dropping the path in
 *                            order to save space when printing a path name.
 *
 * On other platforms, \%Rw? simply prints the argument in a form of 0xXXXXXXXX.
 *
 *
 * Group 4, structure dumpers.
 *
 *      - \%RDtimespec      - Takes a PCRTTIMESPEC.
 *
 *
 * Group 5, XML / HTML escapers.
 *      - \%RMas            - Takes a string pointer (const char *) and outputs
 *                            it as an attribute value with the proper escaping.
 *                            This typically ends up in double quotes.
 *
 *      - \%RMes            - Takes a string pointer (const char *) and outputs
 *                            it as an element with the necessary escaping.
 *
 *
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#define LOG_GROUP RTLOGGROUP_STRING
#include <iprt/string.h>
#include "internal/iprt.h"

#include <iprt/log.h>
#include <iprt/assert.h>
#include <iprt/string.h>
#include <iprt/stdarg.h>
#ifdef IN_RING3
# include <iprt/thread.h>
# include <iprt/err.h>
#endif
#include <iprt/ctype.h>
#include <iprt/time.h>
#include <iprt/net.h>
#include <iprt/path.h>
#include "internal/string.h"



/**
 * Callback to format iprt formatting extentions.
 * See @ref pg_rt_str_format_rt for a reference on the format types.
 *
 * @returns The number of bytes formatted.
 * @param   pfnOutput       Pointer to output function.
 * @param   pvArgOutput     Argument for the output function.
 * @param   ppszFormat      Pointer to the format string pointer. Advance this till the char
 *                          after the format specifier.
 * @param   pArgs           Pointer to the argument list. Use this to fetch the arguments.
 * @param   cchWidth        Format Width. -1 if not specified.
 * @param   cchPrecision    Format Precision. -1 if not specified.
 * @param   fFlags          Flags (RTSTR_NTFS_*).
 * @param   chArgSize       The argument size specifier, 'l' or 'L'.
 */
size_t rtstrFormatRt(PFNRTSTROUTPUT pfnOutput, void *pvArgOutput, const char **ppszFormat, va_list *pArgs, int cchWidth, int cchPrecision, unsigned fFlags, char chArgSize)
{
    const char *pszFormatOrg = *ppszFormat;
    char ch = *(*ppszFormat)++;
    if (ch == 'R')
    {
        ch = *(*ppszFormat)++;
        switch (ch)
        {
            /*
             * Groups 1 and 2.
             */
            case 'T':
            case 'G':
            case 'H':
            case 'R':
            case 'C':
            case 'I':
            case 'X':
            case 'U':
            {
                /*
                 * Interpret the type.
                 */
                typedef enum
                {
                    RTSF_INT,
                    RTSF_INTW,
                    RTSF_BOOL,
                    RTSF_FP16,
                    RTSF_FP32,
                    RTSF_FP64,
                    RTSF_IPV4,
                    RTSF_IPV6,
                    RTSF_MAC,
                    RTSF_NETADDR,
                    RTSF_UUID
                } RTSF;
                static const struct
                {
                    uint8_t     cch;        /**< the length of the string. */
                    char        sz[10];     /**< the part following 'R'. */
                    uint8_t     cb;         /**< the size of the type. */
                    uint8_t     u8Base;     /**< the size of the type. */
                    RTSF        enmFormat;  /**< The way to format it. */
                    uint16_t    fFlags;     /**< additional RTSTR_F_* flags. */
                }
                /** Sorted array of types, looked up using binary search! */
                s_aTypes[] =
                {
#define STRMEM(str) sizeof(str) - 1, str
                    { STRMEM("Ci"),      sizeof(RTINT),          10, RTSF_INT,   RTSTR_F_VALSIGNED },
                    { STRMEM("Cp"),      sizeof(RTCCPHYS),       16, RTSF_INTW,  0 },
                    { STRMEM("Cr"),      sizeof(RTCCUINTREG),    16, RTSF_INTW,  0 },
                    { STRMEM("Cu"),      sizeof(RTUINT),         10, RTSF_INT,   0 },
                    { STRMEM("Cv"),      sizeof(void *),         16, RTSF_INTW,  0 },
                    { STRMEM("Cx"),      sizeof(RTUINT),         16, RTSF_INT,   0 },
                    { STRMEM("Gi"),      sizeof(RTGCINT),        10, RTSF_INT,   RTSTR_F_VALSIGNED },
                    { STRMEM("Gp"),      sizeof(RTGCPHYS),       16, RTSF_INTW,  0 },
                    { STRMEM("Gr"),      sizeof(RTGCUINTREG),    16, RTSF_INTW,  0 },
                    { STRMEM("Gu"),      sizeof(RTGCUINT),       10, RTSF_INT,   0 },
                    { STRMEM("Gv"),      sizeof(RTGCPTR),        16, RTSF_INTW,  0 },
                    { STRMEM("Gx"),      sizeof(RTGCUINT),       16, RTSF_INT,   0 },
                    { STRMEM("Hi"),      sizeof(RTHCINT),        10, RTSF_INT,   RTSTR_F_VALSIGNED },
                    { STRMEM("Hp"),      sizeof(RTHCPHYS),       16, RTSF_INTW,  0 },
                    { STRMEM("Hr"),      sizeof(RTGCUINTREG),    16, RTSF_INTW,  0 },
                    { STRMEM("Hu"),      sizeof(RTHCUINT),       10, RTSF_INT,   0 },
                    { STRMEM("Hv"),      sizeof(RTHCPTR),        16, RTSF_INTW,  0 },
                    { STRMEM("Hx"),      sizeof(RTHCUINT),       16, RTSF_INT,   0 },
                    { STRMEM("I16"),     sizeof(int16_t),        10, RTSF_INT,   RTSTR_F_VALSIGNED },
                    { STRMEM("I32"),     sizeof(int32_t),        10, RTSF_INT,   RTSTR_F_VALSIGNED },
                    { STRMEM("I64"),     sizeof(int64_t),        10, RTSF_INT,   RTSTR_F_VALSIGNED },
                    { STRMEM("I8"),      sizeof(int8_t),         10, RTSF_INT,   RTSTR_F_VALSIGNED },
                    { STRMEM("Rv"),      sizeof(RTRCPTR),        16, RTSF_INTW,  0 },
                    { STRMEM("Tbool"),   sizeof(bool),           10, RTSF_BOOL,  0 },
                    { STRMEM("Tfile"),   sizeof(RTFILE),         10, RTSF_INT,   0 },
                    { STRMEM("Tfmode"),  sizeof(RTFMODE),        16, RTSF_INTW,  0 },
                    { STRMEM("Tfoff"),   sizeof(RTFOFF),         10, RTSF_INT,   RTSTR_F_VALSIGNED },
                    { STRMEM("Tfp16"),   sizeof(RTFAR16),        16, RTSF_FP16,  RTSTR_F_ZEROPAD },
                    { STRMEM("Tfp32"),   sizeof(RTFAR32),        16, RTSF_FP32,  RTSTR_F_ZEROPAD },
                    { STRMEM("Tfp64"),   sizeof(RTFAR64),        16, RTSF_FP64,  RTSTR_F_ZEROPAD },
                    { STRMEM("Tgid"),    sizeof(RTGID),          10, RTSF_INT,   RTSTR_F_VALSIGNED },
                    { STRMEM("Tino"),    sizeof(RTINODE),        16, RTSF_INTW,  0 },
                    { STRMEM("Tint"),    sizeof(RTINT),          10, RTSF_INT,   RTSTR_F_VALSIGNED },
                    { STRMEM("Tiop"),    sizeof(RTIOPORT),       16, RTSF_INTW,  0 },
                    { STRMEM("Tldrm"),   sizeof(RTLDRMOD),       16, RTSF_INTW,  0 },
                    { STRMEM("Tmac"),    sizeof(PCRTMAC),        16, RTSF_MAC,   0 },
                    { STRMEM("Tnaddr"),  sizeof(PCRTNETADDR),    10, RTSF_NETADDR,0 },
                    { STRMEM("Tnaipv4"), sizeof(RTNETADDRIPV4),  10, RTSF_IPV4,  0 },
                    { STRMEM("Tnaipv6"), sizeof(PCRTNETADDRIPV6),16, RTSF_IPV6,  0 },
                    { STRMEM("Tnthrd"),  sizeof(RTNATIVETHREAD), 16, RTSF_INTW,  0 },
                    { STRMEM("Tproc"),   sizeof(RTPROCESS),      16, RTSF_INTW,  0 },
                    { STRMEM("Tptr"),    sizeof(RTUINTPTR),      16, RTSF_INTW,  0 },
                    { STRMEM("Treg"),    sizeof(RTCCUINTREG),    16, RTSF_INTW,  0 },
                    { STRMEM("Tsel"),    sizeof(RTSEL),          16, RTSF_INTW,  0 },
                    { STRMEM("Tsem"),    sizeof(RTSEMEVENT),     16, RTSF_INTW,  0 },
                    { STRMEM("Tsock"),   sizeof(RTSOCKET),       10, RTSF_INT,   0 },
                    { STRMEM("Tthrd"),   sizeof(RTTHREAD),       16, RTSF_INTW,  0 },
                    { STRMEM("Tuid"),    sizeof(RTUID),          10, RTSF_INT,   RTSTR_F_VALSIGNED },
                    { STRMEM("Tuint"),   sizeof(RTUINT),         10, RTSF_INT,   0 },
                    { STRMEM("Tunicp"),  sizeof(RTUNICP),        16, RTSF_INTW,  RTSTR_F_ZEROPAD },
                    { STRMEM("Tutf16"),  sizeof(RTUTF16),        16, RTSF_INTW,  RTSTR_F_ZEROPAD },
                    { STRMEM("Tuuid"),   sizeof(PCRTUUID),       16, RTSF_UUID,  0 },
                    { STRMEM("Txint"),   sizeof(RTUINT),         16, RTSF_INT,   0 },
                    { STRMEM("U16"),     sizeof(uint16_t),       10, RTSF_INT,   0 },
                    { STRMEM("U32"),     sizeof(uint32_t),       10, RTSF_INT,   0 },
                    { STRMEM("U64"),     sizeof(uint64_t),       10, RTSF_INT,   0 },
                    { STRMEM("U8"),      sizeof(uint8_t),        10, RTSF_INT,   0 },
                    { STRMEM("X16"),     sizeof(uint16_t),       16, RTSF_INT,   0 },
                    { STRMEM("X32"),     sizeof(uint32_t),       16, RTSF_INT,   0 },
                    { STRMEM("X64"),     sizeof(uint64_t),       16, RTSF_INT,   0 },
                    { STRMEM("X8"),      sizeof(uint8_t),        16, RTSF_INT,   0 },
#undef STRMEM
                };
                static const char s_szNull[] = "<NULL>";

                const char *pszType = *ppszFormat - 1;
                int         iStart  = 0;
                int         iEnd    = RT_ELEMENTS(s_aTypes) - 1;
                int         i       = RT_ELEMENTS(s_aTypes) / 2;

                union
                {
                    uint8_t             u8;
                    uint16_t            u16;
                    uint32_t            u32;
                    uint64_t            u64;
                    int8_t              i8;
                    int16_t             i16;
                    int32_t             i32;
                    int64_t             i64;
                    RTFAR16             fp16;
                    RTFAR32             fp32;
                    RTFAR64             fp64;
                    bool                fBool;
                    PCRTMAC             pMac;
                    RTNETADDRIPV4       Ipv4Addr;
                    PCRTNETADDRIPV6     pIpv6Addr;
                    PCRTNETADDR         pNetAddr;
                    PCRTUUID            pUuid;
                } u;
                char        szBuf[80];
                unsigned    cch;

                AssertMsg(!chArgSize, ("Not argument size '%c' for RT types! '%.10s'\n", chArgSize, pszFormatOrg));

                /*
                 * Lookup the type - binary search.
                 */
                for (;;)
                {
                    int iDiff = strncmp(pszType, s_aTypes[i].sz, s_aTypes[i].cch);
                    if (!iDiff)
                        break;
                    if (iEnd == iStart)
                    {
                        AssertMsgFailed(("Invalid format type '%.10s'!\n", pszFormatOrg));
                        return 0;
                    }
                    if (iDiff < 0)
                        iEnd = i - 1;
                    else
                        iStart = i + 1;
                    if (iEnd < iStart)
                    {
                        AssertMsgFailed(("Invalid format type '%.10s'!\n", pszFormatOrg));
                        return 0;
                    }
                    i = iStart + (iEnd - iStart) / 2;
                }

                /*
                 * Advance the format string and merge flags.
                 */
                *ppszFormat += s_aTypes[i].cch - 1;
                fFlags |= s_aTypes[i].fFlags;

                /*
                 * Fetch the argument.
                 * It's important that a signed value gets sign-extended up to 64-bit.
                 */
                RT_ZERO(u);
                if (fFlags & RTSTR_F_VALSIGNED)
                {
                    switch (s_aTypes[i].cb)
                    {
                        case sizeof(int8_t):
                            u.i64 = va_arg(*pArgs, /*int8_t*/int);
                            fFlags |= RTSTR_F_8BIT;
                            break;
                        case sizeof(int16_t):
                            u.i64 = va_arg(*pArgs, /*int16_t*/int);
                            fFlags |= RTSTR_F_16BIT;
                            break;
                        case sizeof(int32_t):
                            u.i64 = va_arg(*pArgs, int32_t);
                            fFlags |= RTSTR_F_32BIT;
                            break;
                        case sizeof(int64_t):
                            u.i64 = va_arg(*pArgs, int64_t);
                            fFlags |= RTSTR_F_64BIT;
                            break;
                        default:
                            AssertMsgFailed(("Invalid format error, size %d'!\n", s_aTypes[i].cb));
                            break;
                    }
                }
                else
                {
                    switch (s_aTypes[i].cb)
                    {
                        case sizeof(uint8_t):
                            u.u8 = va_arg(*pArgs, /*uint8_t*/unsigned);
                            fFlags |= RTSTR_F_8BIT;
                            break;
                        case sizeof(uint16_t):
                            u.u16 = va_arg(*pArgs, /*uint16_t*/unsigned);
                            fFlags |= RTSTR_F_16BIT;
                            break;
                        case sizeof(uint32_t):
                            u.u32 = va_arg(*pArgs, uint32_t);
                            fFlags |= RTSTR_F_32BIT;
                            break;
                        case sizeof(uint64_t):
                            u.u64 = va_arg(*pArgs, uint64_t);
                            fFlags |= RTSTR_F_64BIT;
                            break;
                        case sizeof(RTFAR32):
                            u.fp32 = va_arg(*pArgs, RTFAR32);
                            break;
                        case sizeof(RTFAR64):
                            u.fp64 = va_arg(*pArgs, RTFAR64);
                            break;
                        default:
                            AssertMsgFailed(("Invalid format error, size %d'!\n", s_aTypes[i].cb));
                            break;
                    }
                }

                /*
                 * Format the output.
                 */
                switch (s_aTypes[i].enmFormat)
                {
                    case RTSF_INT:
                    {
                        cch = RTStrFormatNumber(szBuf, u.u64, s_aTypes[i].u8Base, cchWidth, cchPrecision, fFlags);
                        break;
                    }

                    /* hex which defaults to max width. */
                    case RTSF_INTW:
                    {
                        Assert(s_aTypes[i].u8Base == 16);
                        if (cchWidth < 0)
                        {
                            cchWidth = s_aTypes[i].cb * 2 + (fFlags & RTSTR_F_SPECIAL ? 2 : 0);
                            fFlags |= RTSTR_F_ZEROPAD;
                        }
                        cch = RTStrFormatNumber(szBuf, u.u64, s_aTypes[i].u8Base, cchWidth, cchPrecision, fFlags);
                        break;
                    }

                    case RTSF_BOOL:
                    {
                        static const char s_szTrue[]  = "true ";
                        static const char s_szFalse[] = "false";
                        if (u.u64 == 1)
                            return pfnOutput(pvArgOutput, s_szTrue,  sizeof(s_szTrue) - 1);
                        if (u.u64 == 0)
                            return pfnOutput(pvArgOutput, s_szFalse, sizeof(s_szFalse) - 1);
                        /* invalid boolean value */
                        return RTStrFormat(pfnOutput, pvArgOutput, NULL, 0, "!%lld!", u.u64);
                    }

                    case RTSF_FP16:
                    {
                        fFlags &= ~(RTSTR_F_VALSIGNED | RTSTR_F_BIT_MASK | RTSTR_F_WIDTH | RTSTR_F_PRECISION | RTSTR_F_THOUSAND_SEP);
                        cch = RTStrFormatNumber(&szBuf[0], u.fp16.sel, 16, 4, -1, fFlags | RTSTR_F_16BIT);
                        Assert(cch == 4);
                        szBuf[4] = ':';
                        cch = RTStrFormatNumber(&szBuf[5], u.fp16.off, 16, 4, -1, fFlags | RTSTR_F_16BIT);
                        Assert(cch == 4);
                        cch = 4 + 1 + 4;
                        break;
                    }
                    case RTSF_FP32:
                    {
                        fFlags &= ~(RTSTR_F_VALSIGNED | RTSTR_F_BIT_MASK | RTSTR_F_WIDTH | RTSTR_F_PRECISION | RTSTR_F_THOUSAND_SEP);
                        cch = RTStrFormatNumber(&szBuf[0], u.fp32.sel, 16, 4, -1, fFlags | RTSTR_F_16BIT);
                        Assert(cch == 4);
                        szBuf[4] = ':';
                        cch = RTStrFormatNumber(&szBuf[5], u.fp32.off, 16, 8, -1, fFlags | RTSTR_F_32BIT);
                        Assert(cch == 8);
                        cch = 4 + 1 + 8;
                        break;
                    }
                    case RTSF_FP64:
                    {
                        fFlags &= ~(RTSTR_F_VALSIGNED | RTSTR_F_BIT_MASK | RTSTR_F_WIDTH | RTSTR_F_PRECISION | RTSTR_F_THOUSAND_SEP);
                        cch = RTStrFormatNumber(&szBuf[0], u.fp64.sel, 16, 4, -1, fFlags | RTSTR_F_16BIT);
                        Assert(cch == 4);
                        szBuf[4] = ':';
                        cch = RTStrFormatNumber(&szBuf[5], u.fp64.off, 16, 16, -1, fFlags | RTSTR_F_64BIT);
                        Assert(cch == 16);
                        cch = 4 + 1 + 16;
                        break;
                    }

                    case RTSF_IPV4:
                        return RTStrFormat(pfnOutput, pvArgOutput, NULL, 0,
                                           "%u.%u.%u.%u",
                                           u.Ipv4Addr.au8[0],
                                           u.Ipv4Addr.au8[1],
                                           u.Ipv4Addr.au8[2],
                                           u.Ipv4Addr.au8[3]);

                    case RTSF_IPV6:
                    {
                        if (VALID_PTR(u.pIpv6Addr))
                            return RTStrFormat(pfnOutput, pvArgOutput, NULL, 0,
                                               "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                                               u.pIpv6Addr->au8[0],
                                               u.pIpv6Addr->au8[1],
                                               u.pIpv6Addr->au8[2],
                                               u.pIpv6Addr->au8[3],
                                               u.pIpv6Addr->au8[4],
                                               u.pIpv6Addr->au8[5],
                                               u.pIpv6Addr->au8[6],
                                               u.pIpv6Addr->au8[7],
                                               u.pIpv6Addr->au8[8],
                                               u.pIpv6Addr->au8[9],
                                               u.pIpv6Addr->au8[10],
                                               u.pIpv6Addr->au8[11],
                                               u.pIpv6Addr->au8[12],
                                               u.pIpv6Addr->au8[13],
                                               u.pIpv6Addr->au8[14],
                                               u.pIpv6Addr->au8[15]);
                        return pfnOutput(pvArgOutput, s_szNull, sizeof(s_szNull) - 1);
                    }

                    case RTSF_MAC:
                    {
                        if (VALID_PTR(u.pMac))
                            return RTStrFormat(pfnOutput, pvArgOutput, NULL, 0,
                                               "%02x:%02x:%02x:%02x:%02x:%02x",
                                               u.pMac->au8[0],
                                               u.pMac->au8[1],
                                               u.pMac->au8[2],
                                               u.pMac->au8[3],
                                               u.pMac->au8[4],
                                               u.pMac->au8[5]);
                        return pfnOutput(pvArgOutput, s_szNull, sizeof(s_szNull) - 1);
                    }

                    case RTSF_NETADDR:
                    {
                        if (VALID_PTR(u.pNetAddr))
                        {
                            switch (u.pNetAddr->enmType)
                            {
                                case RTNETADDRTYPE_IPV4:
                                    if (u.pNetAddr->uPort == RTNETADDR_PORT_NA)
                                        return RTStrFormat(pfnOutput, pvArgOutput, NULL, 0,
                                                           "%u.%u.%u.%u",
                                                           u.pNetAddr->uAddr.IPv4.au8[0],
                                                           u.pNetAddr->uAddr.IPv4.au8[1],
                                                           u.pNetAddr->uAddr.IPv4.au8[2],
                                                           u.pNetAddr->uAddr.IPv4.au8[3]);
                                    return RTStrFormat(pfnOutput, pvArgOutput, NULL, 0,
                                                       "%u.%u.%u.%u:%u",
                                                       u.pNetAddr->uAddr.IPv4.au8[0],
                                                       u.pNetAddr->uAddr.IPv4.au8[1],
                                                       u.pNetAddr->uAddr.IPv4.au8[2],
                                                       u.pNetAddr->uAddr.IPv4.au8[3],
                                                       u.pNetAddr->uPort);

                                case RTNETADDRTYPE_IPV6:
                                    if (u.pNetAddr->uPort == RTNETADDR_PORT_NA)
                                        return RTStrFormat(pfnOutput, pvArgOutput, NULL, 0,
                                                           "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                                                           u.pNetAddr->uAddr.IPv6.au8[0],
                                                           u.pNetAddr->uAddr.IPv6.au8[1],
                                                           u.pNetAddr->uAddr.IPv6.au8[2],
                                                           u.pNetAddr->uAddr.IPv6.au8[3],
                                                           u.pNetAddr->uAddr.IPv6.au8[4],
                                                           u.pNetAddr->uAddr.IPv6.au8[5],
                                                           u.pNetAddr->uAddr.IPv6.au8[6],
                                                           u.pNetAddr->uAddr.IPv6.au8[7],
                                                           u.pNetAddr->uAddr.IPv6.au8[8],
                                                           u.pNetAddr->uAddr.IPv6.au8[9],
                                                           u.pNetAddr->uAddr.IPv6.au8[10],
                                                           u.pNetAddr->uAddr.IPv6.au8[11],
                                                           u.pNetAddr->uAddr.IPv6.au8[12],
                                                           u.pNetAddr->uAddr.IPv6.au8[13],
                                                           u.pNetAddr->uAddr.IPv6.au8[14],
                                                           u.pNetAddr->uAddr.IPv6.au8[15]);
                                    return RTStrFormat(pfnOutput, pvArgOutput, NULL, 0,
                                                       "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x %u",
                                                       u.pNetAddr->uAddr.IPv6.au8[0],
                                                       u.pNetAddr->uAddr.IPv6.au8[1],
                                                       u.pNetAddr->uAddr.IPv6.au8[2],
                                                       u.pNetAddr->uAddr.IPv6.au8[3],
                                                       u.pNetAddr->uAddr.IPv6.au8[4],
                                                       u.pNetAddr->uAddr.IPv6.au8[5],
                                                       u.pNetAddr->uAddr.IPv6.au8[6],
                                                       u.pNetAddr->uAddr.IPv6.au8[7],
                                                       u.pNetAddr->uAddr.IPv6.au8[8],
                                                       u.pNetAddr->uAddr.IPv6.au8[9],
                                                       u.pNetAddr->uAddr.IPv6.au8[10],
                                                       u.pNetAddr->uAddr.IPv6.au8[11],
                                                       u.pNetAddr->uAddr.IPv6.au8[12],
                                                       u.pNetAddr->uAddr.IPv6.au8[13],
                                                       u.pNetAddr->uAddr.IPv6.au8[14],
                                                       u.pNetAddr->uAddr.IPv6.au8[15],
                                                       u.pNetAddr->uPort);

                                case RTNETADDRTYPE_MAC:
                                    return RTStrFormat(pfnOutput, pvArgOutput, NULL, 0,
                                                       "%02x:%02x:%02x:%02x:%02x:%02x",
                                                       u.pNetAddr->uAddr.Mac.au8[0],
                                                       u.pNetAddr->uAddr.Mac.au8[1],
                                                       u.pNetAddr->uAddr.Mac.au8[2],
                                                       u.pNetAddr->uAddr.Mac.au8[3],
                                                       u.pNetAddr->uAddr.Mac.au8[4],
                                                       u.pNetAddr->uAddr.Mac.au8[5]);

                                default:
                                    return RTStrFormat(pfnOutput, pvArgOutput, NULL, 0,
                                                       "unsupported-netaddr-type=%u", u.pNetAddr->enmType);

                            }
                        }
                        return pfnOutput(pvArgOutput, s_szNull, sizeof(s_szNull) - 1);
                    }

                    case RTSF_UUID:
                    {
                        if (VALID_PTR(u.pUuid))
                        {
                            /* cannot call RTUuidToStr because of GC/R0. */
                            return RTStrFormat(pfnOutput, pvArgOutput, NULL, 0,
                                               "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                                               u.pUuid->Gen.u32TimeLow,
                                               u.pUuid->Gen.u16TimeMid,
                                               u.pUuid->Gen.u16TimeHiAndVersion,
                                               u.pUuid->Gen.u8ClockSeqHiAndReserved,
                                               u.pUuid->Gen.u8ClockSeqLow,
                                               u.pUuid->Gen.au8Node[0],
                                               u.pUuid->Gen.au8Node[1],
                                               u.pUuid->Gen.au8Node[2],
                                               u.pUuid->Gen.au8Node[3],
                                               u.pUuid->Gen.au8Node[4],
                                               u.pUuid->Gen.au8Node[5]);
                        }
                        return pfnOutput(pvArgOutput, s_szNull, sizeof(s_szNull) - 1);
                    }

                    default:
                        AssertMsgFailed(("Internal error %d\n", s_aTypes[i].enmFormat));
                        return 0;
                }

                /*
                 * Finally, output the formatted string and return.
                 */
                return pfnOutput(pvArgOutput, szBuf, cch);
            }


            /* Group 3 */

            /*
             * Base name printing.
             */
            case 'b':
            {
                switch (*(*ppszFormat)++)
                {
                    case 'n':
                    {
                        const char *pszLastSep;
                        const char *psz = pszLastSep = va_arg(*pArgs, const char *);
                        if (!VALID_PTR(psz))
                            return pfnOutput(pvArgOutput, "<null>", sizeof("<null>") - 1);

                        while ((ch = *psz) != '\0')
                        {
                            if (RTPATH_IS_SEP(ch))
                            {
                                do
                                    psz++;
                                while ((ch = *psz) != '\0' && RTPATH_IS_SEP(ch));
                                if (!ch)
                                    break;
                                pszLastSep = psz;
                            }
                            psz++;
                        }

                        return pfnOutput(pvArgOutput, pszLastSep, psz - pszLastSep);
                    }

                    default:
                        AssertMsgFailed(("Invalid status code format type '%.10s'!\n", pszFormatOrg));
                        break;
                }
                break;
            }


            /*
             * Pretty function / method name printing.
             */
            case 'f':
            {
                switch (*(*ppszFormat)++)
                {
                    /*
                     * Pretty function / method name printing.
                     * This isn't 100% right (see classic signal prototype) and it assumes
                     * standardized names, but it'll do for today.
                     */
                    case 'n':
                    {
                        const char *pszStart;
                        const char *psz = pszStart = va_arg(*pArgs, const char *);
                        if (!VALID_PTR(psz))
                            return pfnOutput(pvArgOutput, "<null>", sizeof("<null>") - 1);

                        while ((ch = *psz) != '\0' && ch != '(')
                        {
                            if (RT_C_IS_BLANK(ch))
                            {
                                psz++;
                                while ((ch = *psz) != '\0' && (RT_C_IS_BLANK(ch) || ch == '('))
                                    psz++;
                                if (ch)
                                    pszStart = psz;
                            }
                            else if (ch == '(')
                                break;
                            else
                                psz++;
                        }

                        return pfnOutput(pvArgOutput, pszStart, psz - pszStart);
                    }

                    default:
                        AssertMsgFailed(("Invalid status code format type '%.10s'!\n", pszFormatOrg));
                        break;
                }
                break;
            }


            /*
             * hex dumping and COM/XPCOM.
             */
            case 'h':
            {
                switch (*(*ppszFormat)++)
                {
                    /*
                     * Hex stuff.
                     */
                    case 'x':
                    {
                        uint8_t *pu8 = va_arg(*pArgs, uint8_t *);
                        if (cchPrecision <= 0)
                            cchPrecision = 16;
                        if (pu8)
                        {
                            switch (*(*ppszFormat)++)
                            {
                                /*
                                 * Regular hex dump.
                                 */
                                case 'd':
                                {
                                    size_t cch = 0;
                                    int off = 0;

                                    if (cchWidth <= 0)
                                        cchWidth = 16;

                                    while (off < cchPrecision)
                                    {
                                        int i;
                                        cch += RTStrFormat(pfnOutput, pvArgOutput, NULL, 0, "%s%0*x %04x:", off ? "\n" : "", sizeof(pu8) * 2, (uintptr_t)pu8, off);
                                        for (i = 0; i < cchWidth && off + i < cchPrecision ; i++)
                                            cch += RTStrFormat(pfnOutput, pvArgOutput, NULL, 0,
                                                               off + i < cchPrecision ? !(i & 7) && i ? "-%02x" : " %02x" : "   ", pu8[i]);
                                        while (i++ < cchWidth)
                                            cch += pfnOutput(pvArgOutput, "   ", 3);

                                        cch += pfnOutput(pvArgOutput, " ", 1);

                                        for (i = 0; i < cchWidth && off + i < cchPrecision; i++)
                                        {
                                            uint8_t u8 = pu8[i];
                                            cch += pfnOutput(pvArgOutput, u8 < 127 && u8 >= 32 ? (const char *)&u8 : ".", 1);
                                        }

                                        /* next */
                                        pu8 += cchWidth;
                                        off += cchWidth;
                                    }
                                    return cch;
                                }

                                /*
                                 * Hex string.
                                 */
                                case 's':
                                {
                                    if (cchPrecision-- > 0)
                                    {
                                        size_t cch = RTStrFormat(pfnOutput, pvArgOutput, NULL, 0, "%02x", *pu8++);
                                        for (; cchPrecision > 0; cchPrecision--, pu8++)
                                            cch += RTStrFormat(pfnOutput, pvArgOutput, NULL, 0, " %02x", *pu8);
                                        return cch;
                                    }
                                    break;
                                }

                                default:
                                    AssertMsgFailed(("Invalid status code format type '%.10s'!\n", pszFormatOrg));
                                    break;
                            }
                        }
                        else
                            return pfnOutput(pvArgOutput, "<null>", sizeof("<null>") - 1);
                        break;
                    }


#ifdef IN_RING3
                    /*
                     * XPCOM / COM status code: %Rhrc, %Rhrf, %Rhra
                     * ASSUMES: If Windows Then COM else XPCOM.
                     */
                    case 'r':
                    {
                        uint32_t hrc = va_arg(*pArgs, uint32_t);
                        PCRTCOMERRMSG pMsg = RTErrCOMGet(hrc);
                        switch (*(*ppszFormat)++)
                        {
                            case 'c':
                                return pfnOutput(pvArgOutput, pMsg->pszDefine, strlen(pMsg->pszDefine));
                            case 'f':
                                return pfnOutput(pvArgOutput, pMsg->pszMsgFull,strlen(pMsg->pszMsgFull));
                            case 'a':
                                return RTStrFormat(pfnOutput, pvArgOutput, NULL, 0, "%s (0x%08X) - %s", pMsg->pszDefine, hrc, pMsg->pszMsgFull);
                            default:
                                AssertMsgFailed(("Invalid status code format type '%.10s'!\n", pszFormatOrg));
                                return 0;
                        }
                        break;
                    }
#endif /* IN_RING3 */

                    default:
                        AssertMsgFailed(("Invalid status code format type '%.10s'!\n", pszFormatOrg));
                        return 0;

                }
                break;
            }

            /*
             * iprt status code: %Rrc, %Rrs, %Rrf, %Rra.
             */
            case 'r':
            {
                int rc = va_arg(*pArgs, int);
#ifdef IN_RING3                         /* we don't want this anywhere else yet. */
                PCRTSTATUSMSG pMsg = RTErrGet(rc);
                switch (*(*ppszFormat)++)
                {
                    case 'c':
                        return pfnOutput(pvArgOutput, pMsg->pszDefine,    strlen(pMsg->pszDefine));
                    case 's':
                        return pfnOutput(pvArgOutput, pMsg->pszMsgShort,  strlen(pMsg->pszMsgShort));
                    case 'f':
                        return pfnOutput(pvArgOutput, pMsg->pszMsgFull,   strlen(pMsg->pszMsgFull));
                    case 'a':
                        return RTStrFormat(pfnOutput, pvArgOutput, NULL, 0, "%s (%d) - %s", pMsg->pszDefine, rc, pMsg->pszMsgFull);
                    default:
                        AssertMsgFailed(("Invalid status code format type '%.10s'!\n", pszFormatOrg));
                        return 0;
                }
#else /* !IN_RING3 */
                switch (*(*ppszFormat)++)
                {
                    case 'c':
                    case 's':
                    case 'f':
                    case 'a':
                        return RTStrFormat(pfnOutput, pvArgOutput, NULL, 0, "%d", rc);
                    default:
                        AssertMsgFailed(("Invalid status code format type '%.10s'!\n", pszFormatOrg));
                        return 0;
                }
#endif /* !IN_RING3 */
                break;
            }

#if defined(IN_RING3)
            /*
             * Windows status code: %Rwc, %Rwf, %Rwa
             */
            case 'w':
            {
                long rc = va_arg(*pArgs, long);
# if defined(RT_OS_WINDOWS)
                PCRTWINERRMSG pMsg = RTErrWinGet(rc);
# endif
                switch (*(*ppszFormat)++)
                {
# if defined(RT_OS_WINDOWS)
                    case 'c':
                        return pfnOutput(pvArgOutput, pMsg->pszDefine, strlen(pMsg->pszDefine));
                    case 'f':
                        return pfnOutput(pvArgOutput, pMsg->pszMsgFull,strlen(pMsg->pszMsgFull));
                    case 'a':
                        return RTStrFormat(pfnOutput, pvArgOutput, NULL, 0, "%s (0x%08X) - %s", pMsg->pszDefine, rc, pMsg->pszMsgFull);
# else
                    case 'c':
                    case 'f':
                    case 'a':
                        return RTStrFormat(pfnOutput, pvArgOutput, NULL, 0, "0x%08X", rc);
# endif
                    default:
                        AssertMsgFailed(("Invalid status code format type '%.10s'!\n", pszFormatOrg));
                        return 0;
                }
                break;
            }
#endif /* IN_RING3 */

            /*
             * Group 4, structure dumpers.
             */
            case 'D':
            {
                /*
                 * Interpret the type.
                 */
                typedef enum
                {
                    RTST_TIMESPEC
                } RTST;
/** Set if it's a pointer */
#define RTST_FLAGS_POINTER  RT_BIT(0)
                static const struct
                {
                    uint8_t     cch;        /**< the length of the string. */
                    char        sz[16-2];   /**< the part following 'R'. */
                    uint8_t     cb;         /**< the size of the argument. */
                    uint8_t     fFlags;     /**< RTST_FLAGS_* */
                    RTST        enmType;    /**< The structure type. */
                }
                /** Sorted array of types, looked up using binary search! */
                s_aTypes[] =
                {
#define STRMEM(str) sizeof(str) - 1, str
                    { STRMEM("Dtimespec"),   sizeof(PCRTTIMESPEC),  RTST_FLAGS_POINTER, RTST_TIMESPEC},
#undef STRMEM
                };
                const char *pszType = *ppszFormat - 1;
                int         iStart  = 0;
                int         iEnd    = RT_ELEMENTS(s_aTypes) - 1;
                int         i       = RT_ELEMENTS(s_aTypes) / 2;

                union
                {
                    const void     *pv;
                    uint64_t        u64;
                    PCRTTIMESPEC    pTimeSpec;
                } u;

                AssertMsg(!chArgSize, ("Not argument size '%c' for RT types! '%.10s'\n", chArgSize, pszFormatOrg));

                /*
                 * Lookup the type - binary search.
                 */
                for (;;)
                {
                    int iDiff = strncmp(pszType, s_aTypes[i].sz, s_aTypes[i].cch);
                    if (!iDiff)
                        break;
                    if (iEnd == iStart)
                    {
                        AssertMsgFailed(("Invalid format type '%.10s'!\n", pszFormatOrg));
                        return 0;
                    }
                    if (iDiff < 0)
                        iEnd = i - 1;
                    else
                        iStart = i + 1;
                    if (iEnd < iStart)
                    {
                        AssertMsgFailed(("Invalid format type '%.10s'!\n", pszFormatOrg));
                        return 0;
                    }
                    i = iStart + (iEnd - iStart) / 2;
                }
                *ppszFormat += s_aTypes[i].cch - 1;

                /*
                 * Fetch the argument.
                 */
                u.u64 = 0;
                switch (s_aTypes[i].cb)
                {
                    case sizeof(const void *):
                        u.pv = va_arg(*pArgs, const void *);
                        break;
                    default:
                        AssertMsgFailed(("Invalid format error, size %d'!\n", s_aTypes[i].cb));
                        break;
                }

                /*
                 * If it's a pointer, we'll check if it's valid before going on.
                 */
                if ((s_aTypes[i].fFlags & RTST_FLAGS_POINTER) && !VALID_PTR(u.pv))
                    return pfnOutput(pvArgOutput, "<null>", sizeof("<null>") - 1);

                /*
                 * Format the output.
                 */
                switch (s_aTypes[i].enmType)
                {
                    case RTST_TIMESPEC:
                        return RTStrFormat(pfnOutput, pvArgOutput, NULL, NULL, "%'lld ns", RTTimeSpecGetNano(u.pTimeSpec));

                    default:
                        AssertMsgFailed(("Invalid/unhandled enmType=%d\n", s_aTypes[i].enmType));
                        break;
                }
                break;
            }

#ifdef IN_RING3
            /*
             * Group 5, XML / HTML escapers.
             */
            case 'M':
            {
                char chWhat = (*ppszFormat)[0];
                bool fAttr  = chWhat == 'a';
                char chType = (*ppszFormat)[1];
                AssertMsgBreak(chWhat == 'a' || chWhat == 'e', ("Invalid IPRT format type '%.10s'!\n", pszFormatOrg));
                *ppszFormat += 2;
                switch (chType)
                {
                    case 's':
                    {
                        static const char   s_szElemEscape[] = "<>&\"'";
                        static const char   s_szAttrEscape[] = "<>&\"\n\r"; /* more? */
                        const char * const  pszEscape =  fAttr ?             s_szAttrEscape  :             s_szElemEscape;
                        size_t       const  cchEscape = (fAttr ? RT_ELEMENTS(s_szAttrEscape) : RT_ELEMENTS(s_szElemEscape)) - 1;
                        size_t      cchOutput = 0;
                        const char *pszStr    = va_arg(*pArgs, char *);
                        ssize_t     cchStr;
                        ssize_t     offCur;
                        ssize_t     offLast;

                        if (!VALID_PTR(pszStr))
                            pszStr = "<NULL>";
                        cchStr = RTStrNLen(pszStr, (unsigned)cchPrecision);

                        if (fAttr)
                            cchOutput += pfnOutput(pvArgOutput, "\"", 1);
                        if (!(fFlags & RTSTR_F_LEFT))
                            while (--cchWidth >= cchStr)
                                cchOutput += pfnOutput(pvArgOutput, " ", 1);

                        offLast = offCur = 0;
                        while (offCur < cchStr)
                        {
                            if (memchr(pszEscape, pszStr[offCur], cchEscape))
                            {
                                if (offLast < offCur)
                                    cchOutput += pfnOutput(pvArgOutput, &pszStr[offLast], offCur - offLast);
                                switch (pszStr[offCur])
                                {
                                    case '<':   cchOutput += pfnOutput(pvArgOutput, "&lt;", 4); break;
                                    case '>':   cchOutput += pfnOutput(pvArgOutput, "&gt;", 4); break;
                                    case '&':   cchOutput += pfnOutput(pvArgOutput, "&amp;", 5); break;
                                    case '\'':  cchOutput += pfnOutput(pvArgOutput, "&apos;", 6); break;
                                    case '"':   cchOutput += pfnOutput(pvArgOutput, "&qout;", 6); break;
                                    case '\n':  cchOutput += pfnOutput(pvArgOutput, "&#xA;", 5); break;
                                    case '\r':  cchOutput += pfnOutput(pvArgOutput, "&#xD;", 5); break;
                                    default:
                                        AssertFailed();
                                }
                                offLast = offCur + 1;
                            }
                            offCur++;
                        }
                        if (offLast < offCur)
                            cchOutput += pfnOutput(pvArgOutput, &pszStr[offLast], offCur - offLast);

                        while (--cchWidth >= cchStr)
                            cchOutput += pfnOutput(pvArgOutput, " ", 1);
                        if (fAttr)
                            cchOutput += pfnOutput(pvArgOutput, "\"", 1);
                        return cchOutput;
                    }

                    default:
                        AssertMsgFailed(("Invalid IPRT format type '%.10s'!\n", pszFormatOrg));
                }
                break;
            }
#endif /* IN_RING3 */

            /*
             * Invalid/Unknown. Bitch about it.
             */
            default:
                AssertMsgFailed(("Invalid IPRT format type '%.10s'!\n", pszFormatOrg));
                break;
        }
    }
    else
        AssertMsgFailed(("Invalid IPRT format type '%.10s'!\n", pszFormatOrg));

    NOREF(pszFormatOrg);
    return 0;
}

