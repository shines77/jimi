
#include "jimic/math/log10.h"

#include "jimic/libc/ieee754.h"

#include <math.h>

#include <intrin.h>
#pragma intrinsic(_BitScanReverse)

static const unsigned int s_power10[16] = {
    9, 99, 999, 9999, 99999,
    999999, 9999999, 99999999,
    999999999, 4294967295,
    // only fill for aligned to 64 bytes
    0, 0, 0, 0, 0, 0,
};

JMC_DECLARE_NONSTD(unsigned int)
jmc_uint_log10_sys(uint32_t val)
{
    return (unsigned int)log10((uint32_t)val);
}

//
// Reference: https://helloacm.com/fast-integer-log10/
//
JMC_DECLARE_NONSTD(unsigned int)
jmc_uint_log10_comp(uint32_t val)
{
    return (val >= 1000000000u) ? 9 : (val >= 100000000u) ? 8 :
        (val >= 10000000u) ? 7 : (val >= 1000000u) ? 6 :
        (val >= 100000u) ? 5 : (val >= 10000u) ? 4 :
        (val >= 1000u) ? 3 : (val >= 100u) ? 2 : (val >= 10u) ? 1u : 0u;
}

/**
 * log10(2^64) = 19.265919722494796493679289262368
 * 131072 / 100000 = 1.31072
 * log10(2^64) * 131072 / 100000 = 25.25222629866837966019531802197
 * zoom = 2525222.629866837966019531802197
 */

JMC_DECLARE_NONSTD(unsigned int)
jmc_uint_log10(uint32_t val)
{
    unsigned int exponent;
    unsigned int exp10;
    unsigned char isNonzero;

    //
    // _BitScanReverse, _BitScanReverse64
    // Reference: http://msdn.microsoft.com/en-us/library/fbxyd7zd.aspx
    //
    isNonzero = _BitScanReverse((unsigned long *)&exponent, (unsigned long)val);
    if (isNonzero != 0) {
        //exponent++;
    }
    else {
        return 0;
    }

    // must 2,525,222 < 4,194,304 ( 2^32 / 1024)
    // exp10 = exponent * 2525222UL;
    exp10 = exponent * 2525222UL;
    // exp10 = exp10 / 131072 / 64;
    exp10 = exp10 >> 23;

    if (val > s_power10[exp10])
        exp10++;

    return (unsigned int)exp10; 
}

JMC_DECLARE_NONSTD(unsigned int)
jmc_uint_log10_v2(uint32_t val)
{
    unsigned int exponent;
    unsigned int exp10;
    unsigned char isNonzero;

    if (val < 10)
        return 0;

    //
    // _BitScanReverse, _BitScanReverse64
    // Reference: http://msdn.microsoft.com/en-us/library/fbxyd7zd.aspx
    //
    isNonzero = _BitScanReverse((unsigned long *)&exponent, (unsigned long)val);

    // must 2,525,222 < 4,194,304 ( 2^32 / 1024)
    // exp10 = exponent * 2525222UL;
    exp10 = exponent * 2525222UL;
    // exp10 = exp10 / 131072 / 64;
    exp10 = exp10 >> 23;

    if (val > s_power10[exp10])
        exp10++;

    return (unsigned int)exp10;
}

JMC_DECLARE_NONSTD(unsigned int)
jmc_uint_log10_v3(uint32_t val)
{
    unsigned int exponent;
    unsigned int exp10;
    unsigned char isNonzero;

    if (val >= 10) {
        //
        // _BitScanReverse, _BitScanReverse64
        // Reference: http://msdn.microsoft.com/en-us/library/fbxyd7zd.aspx
        //
        isNonzero = _BitScanReverse((unsigned long *)&exponent, (unsigned long)val);

        // must 2,525,222 < 4,194,304 ( 2^32 / 1024)
        // exp10 = exponent * 2525222UL;
        exp10 = exponent * 2525222UL;
        // exp10 = exp10 / 131072 / 64;
        exp10 = exp10 >> 23;

        if (val > s_power10[exp10])
            exp10++;

        return (unsigned int)exp10;
    }
    else {
        return 0;
    }
}

#ifdef _WIN64

JMC_DECLARE_NONSTD(unsigned int)
jmc_uint64_log10(uint64_t val)
{
    jmc_ieee754_double *d64;
    unsigned int exponent;
    unsigned int exp10;
    unsigned char isNonzero;

    d64 = (jmc_ieee754_double *)&val;
    // exponent = (exponent_mask32 >> (52 - 32)) - 1023;
    //
    // _BitScanReverse, _BitScanReverse64
    // Reference: http://msdn.microsoft.com/en-us/library/fbxyd7zd.aspx
    //
    isNonzero = _BitScanReverse64((unsigned long *)&exponent, (unsigned __int64)val);
    if (isNonzero != 0) {
        //exponent++;
    }
    else {
        return 0;
    }

    // must 2,525,222 < 4,194,304 ( 2^32 / 1024)
    // exp10 = exponent * 2525222UL;
    exp10 = exponent * 2525222UL;
    // exp10 = exp10 / 131072 / 64;
    exp10 = exp10 >> 23;
    return (unsigned int)exp10; 
}

#endif
