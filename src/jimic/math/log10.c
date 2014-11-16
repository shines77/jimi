
#include "jimic/math/log10.h"

#include "jimic/libc/ieee754.h"

#include <math.h>

//
// Title: log10使用什么算法实现？
// From URL: http://segmentfault.com/q/1010000000201351
//

#if (defined(_MSC_VER) || defined(__ICL) || defined(__INTEL_COMPILER)) && (defined(_WIN32) || defined(_WIN64))

__declspec(naked)
JMC_DECLARE_NONSTD(double)
ieee754_log10_x87(double val)
{
    //
    // FLD1/FLDL2T/FLDL2E/FLDPI/FLDLG2/FLDLN2/FLDZ -- Load Constant
    //
    // Refrence: http://www.jaist.ac.jp/iscenter-new/mpc/altix/altixdata/opt/intel/vtune/doc/users_guide/mergedProjects/analyzer_ec/mergedProjects/reference_olh/mergedProjects/instructions/instruct32_hh/vc102.htm
    //

    // fldl2t, Push log2(10) onto the FPU register stack.
    // fldl2e, Push log2(e) onto the FPU register stack.
    // fldln2, Push ln(2) onto the FPU register stack.
    // fldlg2, Push log10(2) onto the FPU register stack.
    // fldpi,  Push PI onto the FPU register stack.
    // fld1,   Push +1.0 onto the FPU register stack.
    // fldz,   Push +0.0 onto the FPU register stack.

    // Operation:
    //   TOP <= TOP - 1;
    //   ST(0) <= CONSTANT;

    //
    // 80x87 Instruction Set (x87 - Pentium) 
    // http://www.cs.dartmouth.edu/~mckeeman/cs48/mxcom/doc/x87.html
    //

    __asm {
        ; Local STACK = 0 bytes
        ; Local ARGS  = 0 bytes
        ; [x] = ARG_1 = qword ptr [esp +  4 + STACK + ARGS]

        fld     qword ptr [esp + 4 + 0 + 0]
                                ; Load double x to ST(0);
        fldlg2                  ; Push log10(2) onto the FPU register stack.
                                ; st(0) <= log10(2)
        fxch                    ; Exchange floating point registers
                                ; FCXH is pairable in the V pipe with all FX pairable instructions
                                ; swap ST(0), ST(1)
        fyl2x                   ; fyl2x:    Compute ST(0) <= Y * log2(x)
                                ; fyl2xp1:  Compute ST(0) <= Y * log2(x+1)
        ret
    }
}

#elif defined(__GUNC__) || defined(__linux__) || defined(__llvm__)

JMC_DECLARE_NONSTD(double)
ieee754_log10_x87(double val)
{
    register double ret;
    __asm__(
        "fldlg2\n\t" 
        "fxch\n\t" 
        "fyl2x"
        :"=t"(ret)
        :"0"(val)
    );
    return ret;
}

#else

JMC_DECLARE_NONSTD(double)
ieee754_log10_x87(double val)
{
    return log(val);
}

#endif

JMC_DECLARE_NONSTD(double)
ieee754_log10_x87_v2(double val)
{
    //register double ret_val;
    double *addr = &val;

    //
    // FLD1/FLDL2T/FLDL2E/FLDPI/FLDLG2/FLDLN2/FLDZ -- Load Constant
    //
    // Refrence: http://www.jaist.ac.jp/iscenter-new/mpc/altix/altixdata/opt/intel/vtune/doc/users_guide/mergedProjects/analyzer_ec/mergedProjects/reference_olh/mergedProjects/instructions/instruct32_hh/vc102.htm
    //

    // fldl2t, Push log2(10) onto the FPU register stack.
    // fldl2e, Push log2(e) onto the FPU register stack.
    // fldln2, Push ln(2) onto the FPU register stack.
    // fldlg2, Push log10(2) onto the FPU register stack.
    // fldpi,  Push PI onto the FPU register stack.
    // fld1,   Push +1.0 onto the FPU register stack.
    // fldz,   Push +0.0 onto the FPU register stack.

    // Operation:
    //   TOP <= TOP - 1;
    //   ST(0) <= CONSTANT;

    //
    // 80x87 Instruction Set (x87 - Pentium) 
    // http://www.cs.dartmouth.edu/~mckeeman/cs48/mxcom/doc/x87.html
    //

    __asm {
        mov     eax, addr       ;
        fld     qword ptr [eax] ; fload double x;
        //fld     val             ;
        fldlg2                  ; Push log10(2) onto the FPU register stack.
                                ; st(0) <= log10(2)
        fxch                    ; Exchange floating point registers
                                ; FCXH is pairable in the V pipe with all FX pairable instructions
                                ; swap ST(0), ST(1)
        fyl2x                   ; fyl2x:    Compute ST(0) <= Y * log2(x)
                                ; fyl2xp1:  Compute ST(0) <= Y * log2(x+1)
        //fst     ret_val         ; 
    }
    //return ret_val;
}

//
// Reference: http://stackoverflow.com/questions/10810105/explanation-wanted-log10-faster-than-log-and-log2-but-only-with-o2-and-greater
// Reference: https://helloacm.com/fast-integer-log10/
//

JMC_DECLARE_NONSTD(int)
ieee754_log10_crt_1(double val)
{
#if 1
    // The reciprocal of ln(10)
    static const double s_log10_r = 1.0 / 2.3025850929940456840179914546844;
    //static const double s_log10_r = 1.0 / 2.3025850929940459;
#else
    static double s_log10;
    s_log10 = (double)log((double)10.0);
#endif
    return (int)((double)log(val) * s_log10_r);
}

JMC_DECLARE_NONSTD(int)
ieee754_log10_crt_2(double val)
{
    return (int)((double)log10(val));
}

JMC_DECLARE_NONSTD(int)
ieee754_int_log10_x87(double val)
{
    return (int)((double)ieee754_log10_x87(val));
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
