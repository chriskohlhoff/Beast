//
// Copyright (c) 2017 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BEAST_DETAIL_INTRINSIC_HPP
#define BEAST_DETAIL_INTRINSIC_HPP

#ifndef BEAST_NO_INTRINSICS
# if defined(__GNUC__) || defined(_MSC_VER)
#  define BEAST_NO_INTRINSICS 0
# else
#  define BEAST_NO_INTRINSICS 1
# endif
#endif

#if ! BEAST_NO_INTRINSICS
# if defined(_MSC_VER)
#  include <intrin.h> // __cpuid
# endif
#endif

namespace beast {
namespace detail {

struct cpu_info
{
    bool has_sse4_2 = false;
};

#if ! BEAST_NO_INTRINSICS

/*  Portions from Boost
    Copyright Andrey Semashev 2007 - 2015.
*/
template<class = void>
void
cpuid(
    std::uint32_t& eax,
    std::uint32_t& ebx,
    std::uint32_t& ecx,
    std::uint32_t& edx)
{
#if defined(__GNUC__)
# if defined(__i386__) && defined(__PIC__) && __PIC__ != 0
    // We have to backup ebx in 32 bit PIC code because it is reserved by the ABI
    uint32_t ebx_backup;
    __asm__ __volatile__
    (
        "movl %%ebx, %0\n\t"
        "movl %1, %%ebx\n\t"
        "cpuid\n\t"
        "movl %%ebx, %1\n\t"
        "movl %0, %%ebx\n\t"
            : "=m" (ebx_backup), "+m" (ebx), "+a" (eax), "+c" (ecx), "+d" (edx)
    );
# else
    __asm__ __volatile__
    (
        "cpuid\n\t"
            : "+a" (eax), "+b" (ebx), "+c" (ecx), "+d" (edx)
    );
# endif
#elif defined(_MSC_VER)
    int regs[4] = {};
    __cpuid(regs, eax);
    eax = regs[0];
    ebx = regs[1];
    ecx = regs[2];
    edx = regs[3];
#else
#error Boost.Log: Unexpected compiler
#endif
}

template<class = void>
cpu_info const&
get_cpu_info()
{
    struct cpu_info_init : cpu_info
    {       
        cpu_info_init()
        {
        }
    };

    static cpu_info_init const ci;
    return ci;
}

#else

template<class = void>
cpu_info const&
get_cpu_info()
{
    static cpu_info const ci;
    return ci;
}

#endif

} // detail
} // beast

#endif
