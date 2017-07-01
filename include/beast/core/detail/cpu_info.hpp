//
// Copyright (c) 2017 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BEAST_DETAIL_CPU_INFO_HPP
#define BEAST_DETAIL_CPU_INFO_HPP

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
    bool sse3() const           { return (f1c_ & (1U <<  0)) != 0; }
    bool pclmuldq() const       { return (f1c_ & (1U <<  1)) != 0; }
    bool dtes64() const         { return (f1c_ & (1U <<  2)) != 0; }
    bool monitor() const        { return (f1c_ & (1U <<  3)) != 0; }
    bool dscpl() const          { return (f1c_ & (1U <<  4)) != 0; }
    bool vmx() const            { return (f1c_ & (1U <<  5)) != 0; }
    bool smx() const            { return (f1c_ & (1U <<  6)) != 0; }
    bool eist() const           { return (f1c_ & (1U <<  7)) != 0; }
    bool tm2() const            { return (f1c_ & (1U <<  8)) != 0; }
    bool ssse3() const          { return (f1c_ & (1U <<  9)) != 0; }
    bool cnxtid() const         { return (f1c_ & (1U << 10)) != 0; }
    bool fma() const            { return (f1c_ & (1U << 12)) != 0; }
    bool cx16() const           { return (f1c_ & (1U << 13)) != 0; }
    bool xtpr() const           { return (f1c_ & (1U << 14)) != 0; }
    bool pdcm() const           { return (f1c_ & (1U << 15)) != 0; }
    bool pcid() const           { return (f1c_ & (1U << 17)) != 0; }
    bool dca() const            { return (f1c_ & (1U << 18)) != 0; }
    bool sse41() const          { return (f1c_ & (1U << 19)) != 0; }
    bool sse42() const          { return (f1c_ & (1U << 20)) != 0; }
    bool x2apic() const         { return (f1c_ & (1U << 21)) != 0; }
    bool movbe() const          { return (f1c_ & (1U << 22)) != 0; }
    bool popcnt() const         { return (f1c_ & (1U << 23)) != 0; }
    bool tscdeadline() const    { return (f1c_ & (1U << 24)) != 0; }
    bool aes() const            { return (f1c_ & (1U << 25)) != 0; }
    bool xsave() const          { return (f1c_ & (1U << 26)) != 0; }
    bool osxsave() const        { return (f1c_ & (1U << 27)) != 0; }
    bool avx() const            { return (f1c_ & (1U << 28)) != 0; }
    bool f16c() const           { return (f1c_ & (1U << 29)) != 0; }
    bool rdrand() const         { return (f1c_ & (1U << 30)) != 0; }

    bool fpu() const            { return (f1d_ & (1U <<  0)) != 0; }
    bool vme() const            { return (f1d_ & (1U <<  1)) != 0; }
    bool de() const             { return (f1d_ & (1U <<  2)) != 0; }
    bool pse() const            { return (f1d_ & (1U <<  3)) != 0; }
    bool tsc() const            { return (f1d_ & (1U <<  4)) != 0; }
    bool msr() const            { return (f1d_ & (1U <<  5)) != 0; }
    bool pae() const            { return (f1d_ & (1U <<  6)) != 0; }
    bool mce() const            { return (f1d_ & (1U <<  7)) != 0; }
    bool cx8() const            { return (f1d_ & (1U <<  8)) != 0; }
    bool apic() const           { return (f1d_ & (1U <<  9)) != 0; }
    bool sep() const            { return (f1d_ & (1U << 11)) != 0; }
    bool mtrr() const           { return (f1d_ & (1U << 12)) != 0; }
    bool pge() const            { return (f1d_ & (1U << 13)) != 0; }
    bool mca() const            { return (f1d_ & (1U << 14)) != 0; }
    bool cmov() const           { return (f1d_ & (1U << 15)) != 0; }
    bool pat() const            { return (f1d_ & (1U << 16)) != 0; }
    bool pse36() const          { return (f1d_ & (1U << 17)) != 0; }
    bool psn() const            { return (f1d_ & (1U << 18)) != 0; }
    bool clfsh() const          { return (f1d_ & (1U << 19)) != 0; }
    bool ds() const             { return (f1d_ & (1U << 21)) != 0; }
    bool acpi() const           { return (f1d_ & (1U << 22)) != 0; }
    bool mmx() const            { return (f1d_ & (1U << 23)) != 0; }
    bool fxsr() const           { return (f1d_ & (1U << 24)) != 0; }
    bool sse() const            { return (f1d_ & (1U << 25)) != 0; }
    bool sse2() const           { return (f1d_ & (1U << 26)) != 0; }
    bool ss() const             { return (f1d_ & (1U << 27)) != 0; }
    bool htt() const            { return (f1d_ & (1U << 28)) != 0; }
    bool tm() const             { return (f1d_ & (1U << 29)) != 0; }
    bool pbe() const            { return (f1d_ & (1U << 31)) != 0; }

    bool bmi1() const           { return (f7b_ & (1U <<  3)) != 0; }
    bool hle() const            { return (f7b_ & (1U <<  4)) != 0; }
    bool avx2() const           { return (f7b_ & (1U <<  5)) != 0; }
    bool smep() const           { return (f7b_ & (1U <<  7)) != 0; }
    bool bmi2() const           { return (f7b_ & (1U <<  8)) != 0; }
    bool erms() const           { return (f7b_ & (1U <<  9)) != 0; }
    bool invpcid() const        { return (f7b_ & (1U << 10)) != 0; }
    bool rtm() const            { return (f7b_ & (1U << 11)) != 0; }
    bool mpx() const            { return (f7b_ & (1U << 14)) != 0; }
    bool avx512f() const        { return (f7b_ & (1U << 16)) != 0; }
    bool avx512dq() const       { return (f7b_ & (1U << 17)) != 0; }
    bool rdseed() const         { return (f7b_ & (1U << 18)) != 0; }
    bool adx() const            { return (f7b_ & (1U << 19)) != 0; }
    bool smap() const           { return (f7b_ & (1U << 20)) != 0; }
    bool avx512ifma() const     { return (f7b_ & (1U << 21)) != 0; }
    bool pcommit() const        { return (f7b_ & (1U << 22)) != 0; }
    bool clflushopt() const     { return (f7b_ & (1U << 23)) != 0; }
    bool clwb() const           { return (f7b_ & (1U << 24)) != 0; }
    bool avx512pf() const       { return (f7b_ & (1U << 26)) != 0; }
    bool avx512er() const       { return (f7b_ & (1U << 27)) != 0; }
    bool avx512cd() const       { return (f7b_ & (1U << 28)) != 0; }
    bool sha() const            { return (f7b_ & (1U << 29)) != 0; }
    bool avx512bw() const       { return (f7b_ & (1U << 30)) != 0; }
    bool avx512vl() const       { return (f7b_ & (1U << 31)) != 0; }

    bool prefetchwt1() const    { return (f7c_ & (1U <<  0)) != 0; }
    bool avx512vbmi() const     { return (f7c_ & (1U <<  1)) != 0; }

    cpu_info();

private:
    std::uint32_t f1c_ = 0;
    std::uint32_t f1d_ = 0;
    std::uint32_t f7b_ = 0;
    std::uint32_t f7c_ = 0;
};

#if ! BEAST_NO_INTRINSICS && defined(_MSC_VER)
inline
cpu_info::
cpu_info()
{
    int reg[4];
    __cpuid(static_cast<int*>(reg), 0);
    const int n = reg[0];
    if(n >= 1)
    {
        __cpuid(static_cast<int*>(reg), 1);
        f1c_ = std::uint32_t(reg[2]);
        f1d_ = std::uint32_t(reg[3]);
    }
    if(n >= 7)
    {
        __cpuidex(static_cast<int*>(reg), 7, 0);
        f7b_ = std::uint32_t(reg[1]);
        f7c_ = std::uint32_t(reg[2]);
    }
}

#elif ! BEAST_NO_INTRINSICS && defined(__i386__)    \
    && defined(__PIC__)     && defined(__GNUC__)    \
    && ! defined(__clang__)
inline
cpu_info::
cpu_info()
{
    // We have to backup ebx in 32 bit PIC
    // code because it is reserved by the ABI
    std::uint32_t n;
    __asm__(
        "pushl %%ebx\n\t"
        "cpuid\n\t"
        "popl %%ebx\n\t"
        : "=a"(n)
        : "a"(0)
        : "ecx", "edx");
    if(n >= 1)
    {
        std::uint32_t f1a;
        __asm__(
            "pushl %%ebx\n\t"
            "cpuid\n\t"
            "popl %%ebx\n\t"
            : "=a"(f1a), "=c"(f1c_), "=d"(f1d_)
            : "a"(1)
            :);
    }
    if(n >= 7)
    {
        __asm__(
            "pushl %%ebx\n\t"
            "cpuid\n\t"
            "movl %%ebx, %%eax\n\r"
            "popl %%ebx"
            : "=a"(f7b_), "=c"(f7c_)
            : "a"(7), "c"(0)
            : "edx");
    }
}

#elif ! BEAST_NO_INTRINSICS && defined(__i386__)
inline
cpu_info::
cpu_info()
{
    uint32_t n;
    __asm__("cpuid" : "=a"(n) : "a"(0) : "ebx", "ecx", "edx");
    if(n >= 1)
    {
        std::uint32_t f1a;
        __asm__("cpuid" : "=a"(f1a), "=c"(f1c_), "=d"(f1d_) : "a"(1) : "ebx");
    }
    if(n >= 7)
    {
        std::uint32_t f7a;
        __asm__("cpuid"
              : "=a"(f7a), "=b"(f7b_), "=c"(f7c_)
              : "a"(7), "c"(0)
              : "edx");
    }
}

#else
inline
cpu_info::cpu_info()
{
}

#endif


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

#else

#endif

template<class = void>
cpu_info const&
get_cpu_info()
{
    static cpu_info const ci;
    return ci;
}

} // detail
} // beast

#endif
