
#include "jimic/math/log10.h"

#include "jimic/libc/ieee754.h"

#include <math.h>

JMC_DECLARE_NONSTD(int)
ieee754_log10_crt_1(double val)
{
#if 1
    static const double s_log10 = 2.3025850929940456840179914546844;
    //static const double s_log10 = 2.3025850929940459;
#else
    static double s_log10;
    s_log10 = (double)log((double)10.0);
#endif
    return (int)((double)log(val) / s_log10);
}

JMC_DECLARE_NONSTD(int)
ieee754_log10_crt_2(double val)
{
    return (int)((double)log10(val));
}

/**
 * log10(2^64) = 19.265919722494796493679289262368
 * 131072 / 100000 = 1.31072
 * log10(2^64) * 131072 / 100000 = 25.25222629866837966019531802197
 * zoom = 2525222.629866837966019531802197
 */

JMC_DECLARE_NONSTD(int)
jmc_log10_fast(double val)
{
    jmc_ieee754_double *d64;
    int exponent;
    int exp10;

    d64 = (jmc_ieee754_double *)&val;
    // exponent = (exponent_mask32 >> (52 - 32)) - 1023;
    exponent = d64->ieee.exponent - JMC_IEEE754_DOUBLE_EXPONENT_BIAS;

    // exponent is positive (exponent >= 0 && exponent <= 1024)
    if (exponent >= 0 && exponent <= JMC_IEEE754_DOUBLE_EXPONENT_MAX) {
        // must 2,525,222 < 4,194,304 ( 2^32 / 1024)
        // exp10 = exponent * 2525222UL;
        exp10 = (unsigned int)exponent * 2525222UL;
        // exp10 = exp10 / 131072 / 64;
        exp10 = (int)((unsigned int)exp10 >> 23);
        return (int)exp10;
    }
    else {
        // exp10 = -exponent * 2525222UL;
        exponent = -exponent;
        exp10 = (unsigned int)exponent * 2525222UL;
        // exp10 = exp10 / 131072 / 64, exp10 = exp10 >> (17 + 6), 131072 = 2^17;
        exp10 = (int)((unsigned int)exp10 >> 23);
        return (int)-exp10;
    }   
}

JMC_DECLARE_NONSTD(int)
jmc_log10_fast_v0(double val)
{
    jmc_ieee754_double *d64;
    int exponent;
    int exp10;
    int exp10_remain;

    d64 = (jmc_ieee754_double *)&val;
    // exponent = (exponent_mask32 >> (52 - 32)) - 1023;
    exponent = d64->ieee.exponent - JMC_IEEE754_DOUBLE_EXPONENT_BIAS;

    // exponent is positive (exponent >= 0 && exponent <= 1024)
    if (exponent >= 0 && exponent <= JMC_IEEE754_DOUBLE_EXPONENT_MAX) {
        // exp10 = (exponent / 64) * 19;
        exp10 = (exponent >> 6) * 19;
        // exp10_remain = (exponent & 63) * 19 / 64 / 4 * 4;
        //exp10_remain = (((exponent & 63) * 19) >> 8);
        //exp10 += (exp10_remain << 2);
        exp10_remain = (((exponent & 63) * 19) >> 6);
        exp10 += exp10_remain;
    }
    else {
        exp10 = 0;
    }
    return exp10;
}

/* log10(2^64) = 19.265919722494796493679289262368 */

JMC_DECLARE_NONSTD(int)
jmc_log10_fast_v1(double val)
{
    jmc_ieee754_double *d64;
    int exponent;
    int exp10;
    int exp10_remain;

    d64 = (jmc_ieee754_double *)&val;
    // exponent = (exponent_mask32 >> (52 - 32)) - 1023;
    exponent = d64->ieee.exponent - JMC_IEEE754_DOUBLE_EXPONENT_BIAS;

    // exponent is positive (exponent >= 0 && exponent <= 1024)
    if (exponent >= 0 && exponent <= JMC_IEEE754_DOUBLE_EXPONENT_MAX) {
        // must 19,265,919 < 33,554,432 ( 2^32 / 64 / 2)
        // exp10 = (exponent / 64) * 19265919;
        exp10 = (exponent >> 6) * 19265919;
        // exp10_remain = (exponent & 63) * 19265919 / 64 / 4 * 4;
        //exp10_remain = (((exponent & 63) * 19265919) >> 8);
        //exp10 += (exp10_remain << 2);
        exp10_remain = (((exponent & 63) * 19265919) >> 6);
        exp10 += exp10_remain;
        exp10 = exp10 / 1000000;
    }
    else {
        exp10 = 0;
    }
    return exp10;
}

/**
 * log10(2^64) = 19.265919722494796493679289262368
 * 131072 / 100000 = 1.31072
 * log10(2^64) * 131072 / 100000 = 25.25222629866837966019531802197
 * zoom = 2525222.629866837966019531802197
 */

JMC_DECLARE_NONSTD(int)
jmc_log10_fast_v2(double val)
{
    jmc_ieee754_double *d64;
    int exponent;
    unsigned int exp10;

    d64 = (jmc_ieee754_double *)&val;
    // exponent = (exponent_mask32 >> (52 - 32)) - 1023;
    exponent = d64->ieee.exponent - JMC_IEEE754_DOUBLE_EXPONENT_BIAS;

    // exponent is positive (exponent >= 0 && exponent <= 1024)
    if (exponent >= 0 && exponent <= JMC_IEEE754_DOUBLE_EXPONENT_MAX) {
        // must 2,525,222 < 4,194,304 ( 2^32 / 1024)
        // exp10 = exponent * 2525222UL;
        exp10 = (unsigned int)exponent * 2525222UL;
        // exp10 = exp10 / 131072 / 64;
        exp10 >>= 23;
    }
    else {
        exp10 = 0;
    }
    return (int)exp10;
}

/**
 * log10(2^64) = 19.265919722494796493679289262368
 * 1048576 / 1000000 = 1.048576
 * log10(2^64) * 1048576 / 1000000 = 20.201781038934703728156254417576
 */

JMC_DECLARE_NONSTD(int)
jmc_log10_fast_v2a(double val)
{
    jmc_ieee754_double *d64;
    int exponent;
    int exp10;
    int exp10_remain;

    d64 = (jmc_ieee754_double *)&val;
    // exponent = (exponent_mask32 >> (52 - 32)) - 1023;
    exponent = d64->ieee.exponent - JMC_IEEE754_DOUBLE_EXPONENT_BIAS;

    // exponent is positive (exponent >= 0 && exponent <= 1024)
    if (exponent >= 0 && exponent <= JMC_IEEE754_DOUBLE_EXPONENT_MAX) {
        // must 20,201,781 < 33,554,432 ( 2^32 / 64 / 2)
        // exp10 = (exponent / 64) * 20201781;
        exp10 = (exponent >> 6) * 20201781;
        // exp10_remain = (exponent & 63) * 20201781 / 64 / 4 * 4;
        //exp10_remain = (((exponent & 63) * 20201781) >> 8);
        //exp10 += (exp10_remain << 2);
        exp10_remain = (((exponent & 63) * 20201781) >> 6);
        exp10 += exp10_remain;
        // exp10 = exp10 / 1048576;
        exp10 >>= 20;
    }
    else {
        exp10 = 0;
    }
    return exp10;
}

/**
 * log10(2^64) = 19.265919722494796493679289262368
 * 2^27 / 100000000 = 1.34217728
 * log10(2^64) * 2^27 / 100000000 = 25.858279729836420772040005654498
 * zoom = 2585827972.9836420772040005654498
 */

JMC_DECLARE_NONSTD(int)
jmc_log10_fast_v3(double val)
{
    jmc_ieee754_double *d64;
    int exponent;
    int exp10;
    jmc_ieee754_double exp10_64;

    d64 = (jmc_ieee754_double *)&val;
    // exponent = (exponent_mask32 >> (52 - 32)) - 1023;
    exponent = d64->ieee.exponent - JMC_IEEE754_DOUBLE_EXPONENT_BIAS;

    // exponent is positive (exponent >= 0 && exponent <= 1024)
    if (exponent >= 0 && exponent <= JMC_IEEE754_DOUBLE_EXPONENT_MAX) {
        // must 20,201,781 < 33,554,432 ( 2^32 / 64 / 2)
        // exp10 = (exponent / 64) * 2585827972;
        exp10_64.u64 = (uint64_t)((unsigned int)exponent * 2585827972ULL);
        // exp10 = exp10 / 134217728;   // 2^27
        //exp10 >>= 27;
        exp10 = exp10_64.u32.high >> 1;
    }
    else {
        exp10 = 0;
    }
    return exp10;
}

/**
 * log10(2^64) = 19.265919722494796493679289262368
 * 2^26 / 100000000 = 0.67108864
 * log10(2^64) * 2^26 / 100000000 = 12.929139864918210386020002827249
 * zoom = 1292913986.4918210386020002827249
 */

JMC_DECLARE_NONSTD(int)
jmc_log10_fast64(double val)
{
    jmc_ieee754_double *d64;
    int exponent;
    int exp10;
    jmc_ieee754_double exp10_64 = { 0 };

    d64 = (jmc_ieee754_double *)&val;
    // exponent = (exponent_mask32 >> (52 - 30)) - 1023;
    exponent = d64->ieee.exponent - JMC_IEEE754_DOUBLE_EXPONENT_BIAS;

    // exponent is positive (exponent >= 0 && exponent <= 1024)
    if (exponent >= 0 && exponent <= JMC_IEEE754_DOUBLE_EXPONENT_MAX) {
        // must 20,201,781 < 33,554,432 ( 2^32 / 64 / 2)
        // exp10 = (exponent / 64) * 1292913986;
        //exp10_64.u64 = (uint64_t)((unsigned int)exponent * 1292913986ULL);
        // exp10 = exp10 / 134217728;   // 2^26
        //exp10 >>= 26;
        //exp10 = (int)exp10_64.high;
        exp10 = (int)((uint64_t)((unsigned int)exponent * 1292913986ULL) >> 32);
    }
    else {
        exp10 = 0;
    }
    return exp10;
}
