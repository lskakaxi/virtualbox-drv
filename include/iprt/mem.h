/** @file
 * IPRT - Memory Management and Manipulation.
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

#ifndef ___iprt_mem_h
#define ___iprt_mem_h


#include <iprt/cdefs.h>
#include <iprt/types.h>
#ifdef __cplusplus      /** @todo remove when spitting. */
# include <iprt/cpp/autores.h>
#endif


#ifdef IN_RC
# error "There are no RTMem APIs available Guest Context!"
#endif


/** @defgroup grp_rt_mem    RTMem - Memory Management and Manipulation
 * @ingroup grp_rt
 * @{
 */

RT_C_DECLS_BEGIN

/** @def RTMEM_ALIGNMENT
 * The alignment of the memory blocks returned by RTMemAlloc(), RTMemAllocZ(),
 * RTMemRealloc(), RTMemTmpAlloc() and RTMemTmpAllocZ() for allocations greater
 * than RTMEM_ALIGNMENT.
 *
 * @note This alignment is not forced if the electric fence is active!
 */
#define RTMEM_ALIGNMENT    8

/**
 * Allocates temporary memory.
 *
 * Temporary memory blocks are used for not too large memory blocks which
 * are believed not to stick around for too long. Using this API instead
 * of RTMemAlloc() not only gives the heap manager room for optimization
 * but makes the code easier to read.
 *
 * @returns Pointer to the allocated memory.
 * @returns NULL on failure.
 * @param   cb      Size in bytes of the memory block to allocated.
 */
RTDECL(void *)  RTMemTmpAlloc(size_t cb) RT_NO_THROW;

/**
 * Allocates zero'ed temporary memory.
 *
 * Same as RTMemTmpAlloc() but the memory will be zero'ed.
 *
 * @returns Pointer to the allocated memory.
 * @returns NULL on failure.
 * @param   cb      Size in bytes of the memory block to allocated.
 */
RTDECL(void *)  RTMemTmpAllocZ(size_t cb) RT_NO_THROW;

/**
 * Free temporary memory.
 *
 * @param   pv      Pointer to memory block.
 */
RTDECL(void)    RTMemTmpFree(void *pv) RT_NO_THROW;


/**
 * Allocates memory.
 *
 * @returns Pointer to the allocated memory.
 * @returns NULL on failure.
 * @param   cb      Size in bytes of the memory block to allocated.
 */
RTDECL(void *)  RTMemAlloc(size_t cb) RT_NO_THROW;

/**
 * Allocates zero'ed memory.
 *
 * Instead of memset(pv, 0, sizeof()) use this when you want zero'ed
 * memory. This keeps the code smaller and the heap can skip the memset
 * in about 0.42% of calls :-).
 *
 * @returns Pointer to the allocated memory.
 * @returns NULL on failure.
 * @param   cb      Size in bytes of the memory block to allocated.
 */
RTDECL(void *)  RTMemAllocZ(size_t cb) RT_NO_THROW;

/**
 * Wrapper around RTMemAlloc for automatically aligning variable sized
 * allocations so that the various electric fence heaps works correctly.
 *
 * @returns See RTMemAlloc.
 * @param   cbUnaligned         The unaligned size.
 */
RTDECL(void *) RTMemAllocVar(size_t cbUnaligned);

/**
 * Wrapper around RTMemAllocZ for automatically aligning variable sized
 * allocations so that the various electric fence heaps works correctly.
 *
 * @returns See RTMemAllocZ.
 * @param   cbUnaligned         The unaligned size.
 */
RTDECL(void *) RTMemAllocZVar(size_t cbUnaligned);

/**
 * Duplicates a chunk of memory into a new heap block.
 *
 * @returns New heap block with the duplicate data.
 * @returns NULL if we're out of memory.
 * @param   pvSrc   The memory to duplicate.
 * @param   cb      The amount of memory to duplicate.
 */
RTDECL(void *) RTMemDup(const void *pvSrc, size_t cb) RT_NO_THROW;

/**
 * Duplicates a chunk of memory into a new heap block with some
 * additional zeroed memory.
 *
 * @returns New heap block with the duplicate data.
 * @returns NULL if we're out of memory.
 * @param   pvSrc   The memory to duplicate.
 * @param   cbSrc   The amount of memory to duplicate.
 * @param   cbExtra The amount of extra memory to allocate and zero.
 */
RTDECL(void *) RTMemDupEx(const void *pvSrc, size_t cbSrc, size_t cbExtra) RT_NO_THROW;

/**
 * Reallocates memory.
 *
 * @returns Pointer to the allocated memory.
 * @returns NULL on failure.
 * @param   pvOld   The memory block to reallocate.
 * @param   cbNew   The new block size (in bytes).
 */
RTDECL(void *)  RTMemRealloc(void *pvOld, size_t cbNew) RT_NO_THROW;

/**
 * Frees memory.
 *
 * @param   pv      Pointer to memory block.
 */
RTDECL(void)    RTMemFree(void *pv) RT_NO_THROW;

/**
 * Allocates memory which may contain code.
 *
 * @returns Pointer to the allocated memory.
 * @returns NULL on failure.
 * @param   cb      Size in bytes of the memory block to allocate.
 */
RTDECL(void *)  RTMemExecAlloc(size_t cb) RT_NO_THROW;

/**
 * Free executable/read/write memory allocated by RTMemExecAlloc().
 *
 * @param   pv      Pointer to memory block.
 */
RTDECL(void)    RTMemExecFree(void *pv) RT_NO_THROW;

#if defined(IN_RING0) && defined(RT_ARCH_AMD64) && defined(RT_OS_LINUX)
/**
 * Donate read+write+execute memory to the exec heap.
 *
 * This API is specific to AMD64 and Linux/GNU. A kernel module that desires to
 * use RTMemExecAlloc on AMD64 Linux/GNU will have to donate some statically
 * allocated memory in the module if it wishes for GCC generated code to work.
 * GCC can only generate modules that work in the address range ~2GB to ~0
 * currently.
 *
 * The API only accept one single donation.
 *
 * @returns IPRT status code.
 * @param   pvMemory    Pointer to the memory block.
 * @param   cb          The size of the memory block.
 */
RTR0DECL(int) RTR0MemExecDonate(void *pvMemory, size_t cb) RT_NO_THROW;
#endif /* R0+AMD64+LINUX */

/**
 * Allocate page aligned memory.
 *
 * @returns Pointer to the allocated memory.
 * @returns NULL if we're out of memory.
 * @param   cb  Size of the memory block. Will be rounded up to page size.
 */
RTDECL(void *) RTMemPageAlloc(size_t cb) RT_NO_THROW;

/**
 * Allocate zero'ed page aligned memory.
 *
 * @returns Pointer to the allocated memory.
 * @returns NULL if we're out of memory.
 * @param   cb  Size of the memory block. Will be rounded up to page size.
 */
RTDECL(void *) RTMemPageAllocZ(size_t cb) RT_NO_THROW;

/**
 * Free a memory block allocated with RTMemPageAlloc() or RTMemPageAllocZ().
 *
 * @param   pv      Pointer to the block as it was returned by the allocation function.
 *                  NULL will be ignored.
 * @param   cb      The allocation size.  Will be rounded up to page size.
 *                  Ignored if @a pv is NULL.
 */
RTDECL(void) RTMemPageFree(void *pv, size_t cb) RT_NO_THROW;

/** Page level protection flags for RTMemProtect().
 * @{
 */
/** No access at all. */
#define RTMEM_PROT_NONE   0
/** Read access. */
#define RTMEM_PROT_READ   1
/** Write access. */
#define RTMEM_PROT_WRITE  2
/** Execute access. */
#define RTMEM_PROT_EXEC   4
/** @} */

/**
 * Change the page level protection of a memory region.
 *
 * @returns iprt status code.
 * @param   pv          Start of the region. Will be rounded down to nearest page boundary.
 * @param   cb          Size of the region. Will be rounded up to the nearest page boundary.
 * @param   fProtect    The new protection, a combination of the RTMEM_PROT_* defines.
 */
RTDECL(int) RTMemProtect(void *pv, size_t cb, unsigned fProtect) RT_NO_THROW;


#ifdef IN_RING0

/**
 * Allocates physical contiguous memory (below 4GB).
 * The allocation is page aligned and the content is undefined.
 *
 * @returns Pointer to the memory block. This is page aligned.
 * @param   pPhys   Where to store the physical address.
 * @param   cb      The allocation size in bytes. This is always
 *                  rounded up to PAGE_SIZE.
 */
RTR0DECL(void *) RTMemContAlloc(PRTCCPHYS pPhys, size_t cb) RT_NO_THROW;

/**
 * Frees memory allocated ysing RTMemContAlloc().
 *
 * @param   pv      Pointer to return from RTMemContAlloc().
 * @param   cb      The cb parameter passed to RTMemContAlloc().
 */
RTR0DECL(void) RTMemContFree(void *pv, size_t cb) RT_NO_THROW;

/**
 * Copy memory from an user mode buffer into a kernel buffer.
 *
 * @retval  VINF_SUCCESS on success.
 * @retval  VERR_ACCESS_DENIED on error.
 *
 * @param   pvDst       The kernel mode destination address.
 * @param   R3PtrSrc    The user mode source address.
 * @param   cb          The number of bytes to copy.
 */
RTR0DECL(int) RTR0MemUserCopyFrom(void *pvDst, RTR3PTR R3PtrSrc, size_t cb);

/**
 * Copy memory from a kernel buffer into a user mode one.
 *
 * @retval  VINF_SUCCESS on success.
 * @retval  VERR_ACCESS_DENIED on error.
 *
 * @param   R3PtrDst    The user mode destination address.
 * @param   pvSrc       The kernel mode source address.
 * @param   cb          The number of bytes to copy.
 */
RTR0DECL(int) RTR0MemUserCopyTo(RTR3PTR R3PtrDst, void const *pvSrc, size_t cb);

/**
 * Tests if the specified address is in the user addressable range.
 *
 * This function does not check whether the memory at that address is accessible
 * or anything of that sort, only if the address it self is in the user mode
 * range.
 *
 * @returns true if it's in the user addressable range. false if not.
 * @param   R3Ptr       The user mode pointer to test.
 *
 * @remarks Some systems may have overlapping kernel and user address ranges.
 *          One prominent example of this is the x86 version of Mac OS X. Use
 *          RTR0MemAreKrnlAndUsrDifferent() to check.
 */
RTR0DECL(bool) RTR0MemUserIsValidAddr(RTR3PTR R3Ptr);

/**
 * Tests if the specified address is in the kernel mode range.
 *
 * This function does not check whether the memory at that address is accessible
 * or anything of that sort, only if the address it self is in the kernel mode
 * range.
 *
 * @returns true if it's in the kernel range. false if not.
 * @param   pv          The alleged kernel mode pointer.
 *
 * @remarks Some systems may have overlapping kernel and user address ranges.
 *          One prominent example of this is the x86 version of Mac OS X. Use
 *          RTR0MemAreKrnlAndUsrDifferent() to check.
 */
RTR0DECL(bool) RTR0MemKernelIsValidAddr(void *pv);

/**
 * Are user mode and kernel mode address ranges distinctly different.
 *
 * This determins whether RTR0MemKernelIsValidAddr and RTR0MemUserIsValidAddr
 * can be used for deciding whether some arbitrary address is a user mode or a
 * kernel mode one.
 *
 * @returns true if they are, false if not.
 */
RTR0DECL(bool) RTR0MemAreKrnlAndUsrDifferent(void);

#endif /* IN_RING0 */


/** @name Electrical Fence Version of some APIs.
 * @{
 */

/**
 * Same as RTMemTmpAlloc() except that it's fenced.
 *
 * @returns Pointer to the allocated memory.
 * @returns NULL on failure.
 * @param   cb      Size in bytes of the memory block to allocate.
 */
RTDECL(void *)  RTMemEfTmpAlloc(size_t cb, RT_SRC_POS_DECL) RT_NO_THROW;

/**
 * Same as RTMemTmpAllocZ() except that it's fenced.
 *
 * @returns Pointer to the allocated memory.
 * @returns NULL on failure.
 * @param   cb      Size in bytes of the memory block to allocate.
 */
RTDECL(void *)  RTMemEfTmpAllocZ(size_t cb, RT_SRC_POS_DECL) RT_NO_THROW;

/**
 * Same as RTMemTmpFree() except that it's for fenced memory.
 *
 * @param   pv      Pointer to memory block.
 */
RTDECL(void)    RTMemEfTmpFree(void *pv, RT_SRC_POS_DECL) RT_NO_THROW;

/**
 * Same as RTMemAlloc() except that it's fenced.
 *
 * @returns Pointer to the allocated memory. Free with RTMemEfFree().
 * @returns NULL on failure.
 * @param   cb      Size in bytes of the memory block to allocate.
 */
RTDECL(void *)  RTMemEfAlloc(size_t cb, RT_SRC_POS_DECL) RT_NO_THROW;

/**
 * Same as RTMemAllocZ() except that it's fenced.
 *
 * @returns Pointer to the allocated memory.
 * @returns NULL on failure.
 * @param   cb      Size in bytes of the memory block to allocate.
 */
RTDECL(void *)  RTMemEfAllocZ(size_t cb, RT_SRC_POS_DECL) RT_NO_THROW;

/**
 * Same as RTMemAllocVar() except that it's fenced.
 *
 * @returns Pointer to the allocated memory. Free with RTMemEfFree().
 * @returns NULL on failure.
 * @param   cbUnaligned Size in bytes of the memory block to allocate.
 */
RTDECL(void *)  RTMemEfAllocVar(size_t cbUnaligned, RT_SRC_POS_DECL) RT_NO_THROW;

/**
 * Same as RTMemAllocZVar() except that it's fenced.
 *
 * @returns Pointer to the allocated memory.
 * @returns NULL on failure.
 * @param   cbUnaligned Size in bytes of the memory block to allocate.
 */
RTDECL(void *)  RTMemEfAllocZVar(size_t cbUnaligned, RT_SRC_POS_DECL) RT_NO_THROW;

/**
 * Same as RTMemRealloc() except that it's fenced.
 *
 * @returns Pointer to the allocated memory.
 * @returns NULL on failure.
 * @param   pvOld   The memory block to reallocate.
 * @param   cbNew   The new block size (in bytes).
 */
RTDECL(void *)  RTMemEfRealloc(void *pvOld, size_t cbNew, RT_SRC_POS_DECL) RT_NO_THROW;

/**
 * Free memory allocated by any of the RTMemEf* allocators.
 *
 * @param   pv      Pointer to memory block.
 */
RTDECL(void)    RTMemEfFree(void *pv, RT_SRC_POS_DECL) RT_NO_THROW;

/**
 * Same as RTMemDup() except that it's fenced.
 *
 * @returns New heap block with the duplicate data.
 * @returns NULL if we're out of memory.
 * @param   pvSrc   The memory to duplicate.
 * @param   cb      The amount of memory to duplicate.
 */
RTDECL(void *) RTMemEfDup(const void *pvSrc, size_t cb, RT_SRC_POS_DECL) RT_NO_THROW;

/**
 * Same as RTMemEfDupEx except that it's fenced.
 *
 * @returns New heap block with the duplicate data.
 * @returns NULL if we're out of memory.
 * @param   pvSrc   The memory to duplicate.
 * @param   cbSrc   The amount of memory to duplicate.
 * @param   cbExtra The amount of extra memory to allocate and zero.
 */
RTDECL(void *) RTMemEfDupEx(const void *pvSrc, size_t cbSrc, size_t cbExtra, RT_SRC_POS_DECL) RT_NO_THROW;

/** @def RTMEM_WRAP_TO_EF_APIS
 * Define RTMEM_WRAP_TO_EF_APIS to wrap RTMem APIs to RTMemEf APIs.
 */
#if defined(RTMEM_WRAP_TO_EF_APIS) && defined(IN_RING3) && !defined(RTMEM_NO_WRAP_TO_EF_APIS)
# define RTMemTmpAlloc(cb)                  RTMemEfTmpAlloc((cb), RT_SRC_POS)
# define RTMemTmpAllocZ(cb)                 RTMemEfTmpAllocZ((cb), RT_SRC_POS)
# define RTMemTmpFree(pv)                   RTMemEfTmpFree((pv), RT_SRC_POS)
# define RTMemAlloc(cb)                     RTMemEfAlloc((cb), RT_SRC_POS)
# define RTMemAllocZ(cb)                    RTMemEfAllocZ((cb), RT_SRC_POS)
# define RTMemAllocVar(cbUnaligned)         RTMemEfAllocVar((cbUnaligned), RT_SRC_POS)
# define RTMemAllocZVar(cbUnaligned)        RTMemEfAllocZVar((cbUnaligned), RT_SRC_POS)
# define RTMemRealloc(pvOld, cbNew)         RTMemEfRealloc((pvOld), (cbNew), RT_SRC_POS)
# define RTMemFree(pv)                      RTMemEfFree((pv), RT_SRC_POS)
# define RTMemDup(pvSrc, cb)                RTMemEfDup((pvSrc), (cb), RT_SRC_POS)
# define RTMemDupEx(pvSrc, cbSrc, cbExtra)  RTMemEfDupEx((pvSrc), (cbSrc), (cbExtra), RT_SRC_POS)
#endif
#ifdef DOXYGEN_RUNNING
# define RTMEM_WRAP_TO_EF_APIS
#endif

/**
 * Fenced drop-in replacement for RTMemTmpAlloc.
 * @copydoc RTMemTmpAlloc
 */
RTDECL(void *)  RTMemEfTmpAllocNP(size_t cb) RT_NO_THROW;

/**
 * Fenced drop-in replacement for RTMemTmpAllocZ.
 * @copydoc RTMemTmpAllocZ
 */
RTDECL(void *)  RTMemEfTmpAllocZNP(size_t cb) RT_NO_THROW;

/**
 * Fenced drop-in replacement for RTMemTmpFree.
 * @copydoc RTMemTmpFree
 */
RTDECL(void)    RTMemEfTmpFreeNP(void *pv) RT_NO_THROW;

/**
 * Fenced drop-in replacement for RTMemAlloc.
 * @copydoc RTMemAlloc
 */
RTDECL(void *)  RTMemEfAllocNP(size_t cb) RT_NO_THROW;

/**
 * Fenced drop-in replacement for RTMemAllocZ.
 * @copydoc RTMemAllocZ
 */
RTDECL(void *)  RTMemEfAllocZNP(size_t cb) RT_NO_THROW;

/**
 * Fenced drop-in replacement for RTMemAllocVar
 * @copydoc RTMemAllocVar
 */
RTDECL(void *)  RTMemEfAllocVarNP(size_t cbUnaligned) RT_NO_THROW;

/**
 * Fenced drop-in replacement for RTMemAllocZVar.
 * @copydoc RTMemAllocZVar
 */
RTDECL(void *)  RTMemEfAllocZVarNP(size_t cbUnaligned) RT_NO_THROW;

/**
 * Fenced drop-in replacement for RTMemRealloc.
 * @copydoc RTMemRealloc
 */
RTDECL(void *)  RTMemEfReallocNP(void *pvOld, size_t cbNew) RT_NO_THROW;

/**
 * Fenced drop-in replacement for RTMemFree.
 * @copydoc RTMemFree
 */
RTDECL(void)    RTMemEfFreeNP(void *pv) RT_NO_THROW;

/**
 * Fenced drop-in replacement for RTMemDupEx.
 * @copydoc RTMemDupEx
 */
RTDECL(void *) RTMemEfDupNP(const void *pvSrc, size_t cb) RT_NO_THROW;

/**
 * Fenced drop-in replacement for RTMemDupEx.
 * @copydoc RTMemDupEx
 */
RTDECL(void *) RTMemEfDupExNP(const void *pvSrc, size_t cbSrc, size_t cbExtra) RT_NO_THROW;

/** @} */

RT_C_DECLS_END


#ifdef __cplusplus /** @todo Split this out into iprt/cpp/mem.h! */
# include <iprt/assert.h>

/**
 * Template function wrapping RTMemFree to get the correct Destruct
 * signature for RTAutoRes.
 *
 * We can't use a more complex template here, because the g++ on RHEL 3
 * chokes on it with an internal compiler error.
 *
 * @tparam  T           The data type that's being managed.
 * @param   aMem        Pointer to the memory that should be free.
 */
template <class T>
inline void RTMemAutoDestructor(T *aMem) RT_NO_THROW
{
    RTMemFree(aMem);
}


/**
 * RTMemAutoPtr allocator which uses RTMemTmpAlloc().
 *
 * @returns Allocated memory on success, NULL on failure.
 * @param   pvOld       What to reallocate, shall always be NULL.
 * @param   cbNew       The amount of memory to allocate (in bytes).
 */
inline void *RTMemTmpAutoAllocator(void *pvOld, size_t cbNew) RT_NO_THROW
{
    AssertReturn(!pvOld, NULL);
    return RTMemTmpAlloc(cbNew);
}


/**
 * Template function wrapping RTMemTmpFree to get the correct Destruct
 * signature for RTAutoRes.
 *
 * We can't use a more complex template here, because the g++ on RHEL 3
 * chokes on it with an internal compiler error.
 *
 * @tparam  T           The data type that's being managed.
 * @param   aMem        Pointer to the memory that should be free.
 */
template <class T>
inline void RTMemTmpAutoDestructor(T *aMem) RT_NO_THROW
{
    RTMemTmpFree(aMem);
}


/**
 * Template function wrapping RTMemEfFree to get the correct Destruct
 * signature for RTAutoRes.
 *
 * We can't use a more complex template here, because the g++ on RHEL 3
 * chokes on it with an internal compiler error.
 *
 * @tparam  T           The data type that's being managed.
 * @param   aMem        Pointer to the memory that should be free.
 */
template <class T>
inline void RTMemEfAutoFree(T *aMem) RT_NO_THROW
{
    RTMemEfFreeNP(aMem);
}


/**
 * Template function wrapping NULL to get the correct NilRes signature
 * for RTAutoRes.
 *
 * @tparam  T           The data type that's being managed.
 * @returns NULL with the right type.
 */
template <class T>
inline T * RTMemAutoNil(void) RT_NO_THROW
{
    return (T *)(NULL);
}


/**
 * An auto pointer-type template class for managing memory allocating
 * via C APIs like RTMem (the default).
 *
 * The main purpose of this class is to automatically free memory that
 * isn't explicitly used (release()'ed) when the object goes out of scope.
 *
 * As an additional service it can also make the allocations and
 * reallocations for you if you like, but it can also take of memory
 * you hand it.
 *
 * @tparam  T           The data type to manage allocations for.
 * @tparam  Destruct    The function to be used to free the resource.
 *                      This will default to RTMemFree.
 * @tparam  Allocator   The function to be used to allocate or reallocate
 *                      the managed memory.
 *                      This is standard realloc() like stuff, so it's possible
 *                      to support simple allocation without actually having
 *                      to support reallocating memory if that's a problem.
 *                      This will default to RTMemRealloc.
 */
template <class T,
          void Destruct(T *) = RTMemAutoDestructor<T>,
# ifdef RTMEM_WRAP_TO_EF_APIS
          void *Allocator(void *, size_t) = RTMemEfReallocNP
# else
          void *Allocator(void *, size_t) = RTMemRealloc
# endif
          >
class RTMemAutoPtr
    : public RTAutoRes<T *, Destruct, RTMemAutoNil<T> >
{
public:
    /**
     * Constructor.
     *
     * @param   aPtr    Memory pointer to manage. Defaults to NULL.
     */
    RTMemAutoPtr(T *aPtr = NULL)
        : RTAutoRes<T *, Destruct, RTMemAutoNil<T> >(aPtr)
    {
    }

    /**
     * Constructor that allocates memory.
     *
     * @param   a_cElements The number of elements (of the data type) to allocate.
     * @param   a_fZeroed   Whether the memory should be memset with zeros after
     *                      the allocation. Defaults to false.
     */
    RTMemAutoPtr(size_t a_cElements, bool a_fZeroed = false)
        : RTAutoRes<T *, Destruct, RTMemAutoNil<T> >((T *)Allocator(NULL, a_cElements * sizeof(T)))
    {
        if (a_fZeroed && RT_LIKELY(this->get() != NULL))
            memset(this->get(), '\0', a_cElements * sizeof(T));
    }

    /**
     * Free current memory and start managing aPtr.
     *
     * @param   aPtr    Memory pointer to manage.
     */
    RTMemAutoPtr &operator=(T *aPtr)
    {
        this->RTAutoRes<T *, Destruct, RTMemAutoNil<T> >::operator=(aPtr);
        return *this;
    }

    /**
     * Dereference with * operator.
     */
    T &operator*()
    {
         return *this->get();
    }

    /**
     * Dereference with -> operator.
     */
    T *operator->()
    {
        return this->get();
    }

    /**
     * Accessed with the subscript operator ([]).
     *
     * @returns Reference to the element.
     * @param   a_i     The element to access.
     */
    T &operator[](size_t a_i)
    {
        return this->get()[a_i];
    }

    /**
     * Allocates memory and start manage it.
     *
     * Any previously managed memory will be freed before making
     * the new allocation.
     *
     * @returns Success indicator.
     * @retval  true if the new allocation succeeds.
     * @retval  false on failure, no memory is associated with the object.
     *
     * @param   a_cElements The number of elements (of the data type) to allocate.
     *                      This defaults to 1.
     * @param   a_fZeroed   Whether the memory should be memset with zeros after
     *                      the allocation. Defaults to false.
     */
    bool alloc(size_t a_cElements = 1, bool a_fZeroed = false)
    {
        this->reset(NULL);
        T *pNewMem = (T *)Allocator(NULL, a_cElements * sizeof(T));
        if (a_fZeroed && RT_LIKELY(pNewMem != NULL))
            memset(pNewMem, '\0', a_cElements * sizeof(T));
        this->reset(pNewMem);
        return pNewMem != NULL;
    }

    /**
     * Reallocate or allocates the memory resource.
     *
     * Free the old value if allocation fails.
     *
     * The content of any additional memory that was allocated is
     * undefined when using the default allocator.
     *
     * @returns Success indicator.
     * @retval  true if the new allocation succeeds.
     * @retval  false on failure, no memory is associated with the object.
     *
     * @param   a_cElements The new number of elements (of the data type) to
     *                      allocate. The size of the allocation is the number of
     *                      elements times the size of the data type - this is
     *                      currently what's passed down to the Allocator.
     *                      This defaults to 1.
     */
    bool realloc(size_t a_cElements = 1)
    {
        T *aNewValue = (T *)Allocator(this->get(), a_cElements * sizeof(T));
        if (RT_LIKELY(aNewValue != NULL))
            this->release();
        /* We want this both if aNewValue is non-NULL and if it is NULL. */
        this->reset(aNewValue);
        return aNewValue != NULL;
    }
};


#endif /* __cplusplus */


/** @} */


#endif

