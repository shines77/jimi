
#include "ftol_test.h"

#include "jimi/basic/stddef.h"
#include "jimi/system/stop_watch.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//
// 代码优化－之－优化浮点数取整 
//
// Reference: http://blog.csdn.net/housisong/article/details/1616026
//
// Reference: http://blog.codingnow.com/2006/02/double_to_int_magic_number.html
//

volatile long testResult;       // 使用一个全局域的volatile变量以避免编译器把需要测试的代码优化掉
#ifndef _DEBUG
const long    testDataCount = 10000000;
#else
const long    testDataCount = 10000;
#endif
const long    testCount = 20;
float         fSrc[testDataCount];
double        dSrc[testDataCount];

void ftol_test_0()
{
    long tmp = 0;
    for (long i = 0; i < testDataCount; ++i)
        tmp += (long)fSrc[i];  // 需要优化的浮点数取整
    testResult = tmp;
}

#if defined(WIN64) || defined(_WIN64) || defined(_M_X64) || defined(_M_AMD64) \
 || defined(_M_IA64) || defined(_M_ARM64) || defined(__amd64__) || defined(__x86_64__)

void ftol_test_fpu()
{
}

#else

void ftol_test_fpu()
{
    unsigned short RC_Old;
    unsigned short RC_Edit;
    long isrc;

#if !defined(JIMI_MSVC_CLANG) || (JIMI_MSVC_CLANG == 0)

    __asm {
        // 设置FPU的取整方式  为了直接使用fistp浮点指令
        FNSTCW  RC_Old             // 保存协处理器控制字,用来恢复
        FNSTCW  RC_Edit            // 保存协处理器控制字,用来修改
        FWAIT
        OR      RC_Edit, 0x0F00    // 改为 RC=11  使FPU向零取整
        FLDCW   RC_Edit            // 载入协处理器控制字,RC场已经修改

        mov     ecx, testDataCount
        xor     eax, eax
        test    ecx, ecx
        jle     EndLoop

        lea     edx, [fSrc + ecx * 4]
        neg     ecx
StartLoop:
        fld     dword ptr[edx + ecx * 4]
        fistp   isrc
        add     eax, isrc

        inc     ecx
        jnz     StartLoop

EndLoop:
        mov  testResult, eax;

        //恢复FPU的取整方式
        FWAIT
        FLDCW   RC_Old
    }

#endif
}

#endif // __amd64__

inline long _ftol_ieee(float f)
{ 
    long a         = *(long *)(&f);
    unsigned long mantissa  = (a & ((1 << 23) - 1)) | (1 << 23);    // 不支持非规格化浮点数
    long exponent  = ((a & 0x7fffffffL) >> 23);
    long r         = (mantissa << 8) >> (31 + 127 - exponent);
    long sign      = (a >> 31); 
    return ((r ^ (sign)) - sign) & (~((exponent - 127) >> 31));
}

void ftol_test_ieee()
{
    long tmp = 0;
    for (long i = 0; i < testDataCount; ++i) 
        tmp += _ftol_ieee(fSrc[i]);  
    testResult = tmp;
}

inline long ftol_ieee_M(float x) 
{ 
    static const float magic_f = (3 << 21);
    static const long magic_i = 0x4ac00000;
    float ftmp = x + magic_f;
    return  (*((long *)&ftmp) - magic_i) >> 1; 
}

void ftol_test_ieee_M()
{
    long tmp = 0;
    for (long i = 0; i < testDataCount; ++i) 
        tmp += ftol_ieee_M(fSrc[i]);  
    testResult = tmp;
}

#if defined(WIN64) || defined(_WIN64) || defined(_M_X64) || defined(_M_AMD64) \
 || defined(_M_IA64) || defined(_M_ARM64) || defined(__amd64__) || defined(__x86_64__)

void ftol_test_sse2()
{
}

#else

void ftol_test_sse2()
{

#if !defined(JIMI_MSVC_CLANG) || (JIMI_MSVC_CLANG == 0)

    __asm {
        mov         ecx, testDataCount
        xor         eax, eax
        test        ecx, ecx
        jle         EndLoop

        lea         edx, [fSrc + ecx * 4]
        neg         ecx
StartLoop:
        cvttss2si   ebx, dword ptr [edx + ecx * 4]
        add         eax, ebx

        inc         ecx
        jnz         StartLoop

EndLoop:
        mov         testResult, eax;
    }

#endif
}

#endif // __amd64__

void dtol_test_0()
{
    long tmp = 0;
    for (long i = 0; i < testDataCount; ++i) {
        tmp += (long)dSrc[i];   // 需要优化的浮点取整
    }
    testResult = tmp;
}

#if defined(WIN64) || defined(_WIN64) || defined(_M_X64) || defined(_M_AMD64) \
 || defined(_M_IA64) || defined(_M_ARM64) || defined(__amd64__) || defined(__x86_64__)

void dtol_test_fpu()
{
}

#else

void dtol_test_fpu()
{
    unsigned short RC_Old;
    unsigned short RC_Edit;
    long isrc;

#if !defined(JIMI_MSVC_CLANG) || (JIMI_MSVC_CLANG == 0)

    __asm  // 设置FPU的取整方式  为了直接使用fistp浮点指令
    {
        FNSTCW  RC_Old             // 保存协处理器控制字,用来恢复
        FNSTCW  RC_Edit            // 保存协处理器控制字,用来修改
        FWAIT
        OR      RC_Edit, 0x0F00    // 改为 RC=11  使FPU向零取整     
        FLDCW   RC_Edit            // 载入协处理器控制字,RC场已经修改

        mov     ecx, testDataCount
        xor     eax, eax
        test    ecx, ecx
        jle     EndLoop

        lea     edx, [dSrc + ecx * 8]
        neg     ecx
StartLoop:
        fld     qword ptr [edx + ecx * 8]
        fistp   isrc
        add     eax, isrc

        inc     ecx
        jnz     StartLoop

EndLoop:
        mov     testResult, eax;

        FWAIT
        FLDCW   RC_Old 
    }

#endif
}

#endif // __amd64__

inline int64_t _dtol_ieee(double f)
{ 
    uint64_t i64        = *(uint64_t *)(&f);
    uint64_t mantissa   = (i64 & ((1ULL << 52) - 1)) | (1ULL << 52);    // 不支持非规格化浮点数
    uint32_t exponent   = ((i64 & 0x7FF0000000000000ULL) >> 52);
    int64_t r           = (mantissa << 11) >> (63 + 2047 - exponent);
    int64_t sign        = (i64 >> 63); 
    return ((r ^ (sign)) - sign) & (~(((uint64_t)exponent - 2047) >> 63));
}

void dtol_test_ieee()
{
    long tmp = 0;
    for (long i = 0; i < testDataCount; ++i) 
        tmp += (long)_dtol_ieee(dSrc[i]);  
    testResult = tmp;
}

inline int64_t _dtol_ieee_MagicNumber(double x)  
{ 
    static const double magic = 6755399441055744.0;     // (1 << 51) | (1 << 52)
    double tmp = x;
    tmp += (x > 0) ? -0.499999999999 : +0.499999999999; // 如果需要4舍5入取整就去掉这一行
    tmp += magic;
    return *(long *)&tmp;
}

void dtol_test_ieee_MagicNumber()
{
    long tmp = 0;
    for (long i = 0; i < testDataCount; ++i) 
        tmp += (long)_dtol_ieee_MagicNumber(dSrc[i]);  
    testResult = tmp;
}

inline long _dtol_ieee_MagicNumber2(double x)  
{ 
    static const double magic = 6755399441055744.0;     // (1 << 51) | (1 << 52)
    double tmp = x;
    tmp += magic;
    return *(long *)&tmp;
}

void dtol_test_ieee_MagicNumber2()
{
    long tmp = 0;
    for (long i = 0; i < testDataCount; ++i) 
        tmp += _dtol_ieee_MagicNumber2(dSrc[i]);  
    testResult = tmp;
}

#if defined(WIN64) || defined(_WIN64) || defined(_M_X64) || defined(_M_AMD64) \
 || defined(_M_IA64) || defined(_M_ARM64) || defined(__amd64__) || defined(__x86_64__)

void dtol_test_sse2()
{
}

#else

void dtol_test_sse2()
{

#if !defined(JIMI_MSVC_CLANG) || (JIMI_MSVC_CLANG == 0)

    __asm {
        mov         ecx, testDataCount
        xor         eax, eax
        test        ecx, ecx
        jle         EndLoop

        lea         edx, [dSrc + ecx * 8]
        neg         ecx
StartLoop:
        cvttsd2si   ebx, qword ptr [edx + ecx * 8]
        add         eax, ebx

        inc         ecx
        jnz         StartLoop

EndLoop:
        mov         testResult, eax;
    }

#endif
}

#endif // __amd64__

void ftol_test_main()
{
    jimi::stop_watch sw;

    // init
    for (long i = 0; i < testDataCount; ++i)
        fSrc[i] = (float)(rand() * (1.0 / RAND_MAX) * (rand() - (RAND_MAX >> 1)) * rand() * (1.0 / RAND_MAX));

    printf("--------------------------------------------------------------\n\n");

    // test
    sw.restart();
    for (long c = 0; c < testCount; ++c)
        ftol_test_0();
    sw.stop();

    // out
    printf("ftol_test_0(),          Result = %ld, time = %0.3f ms\n", testResult, sw.getMillisec());
    printf("\n");

    // test
    sw.restart();
    for (long c = 0; c < testCount; ++c)
        ftol_test_fpu();
    sw.stop();

    // out
    printf("ftol_test_fpu(),        Result = %ld, time = %0.3f ms\n", testResult, sw.getMillisec());
    printf("\n");

    // test
    sw.restart();
    for (long c = 0; c < testCount; ++c)
        ftol_test_ieee();
    sw.stop();

    // out
    printf("ftol_test_ieee(),       Result = %ld, time = %0.3f ms\n", testResult, sw.getMillisec());
    printf("\n");

    // test
    sw.restart();
    for (long c = 0; c < testCount; ++c)
        ftol_test_ieee_M();
    sw.stop();

    // out
    printf("ftol_test_ieee_M(),     Result = %ld, time = %0.3f ms\n", testResult, sw.getMillisec());
    printf("\n");

    // test
    sw.restart();
    for (long c = 0; c < testCount; ++c)
        ftol_test_sse2();
    sw.stop();

    // out
    printf("ftol_test_sse2(),       Result = %ld, time = %0.3f ms\n", testResult, sw.getMillisec());
    printf("\n");
}

//
// 代码优化－之－优化浮点数取整 
//
// Reference: http://blog.csdn.net/housisong/article/details/1616026
//
// Reference: http://blog.codingnow.com/2006/02/double_to_int_magic_number.html
//

void dtol_test_main()
{
    jimi::stop_watch sw;

    // init
    for (long i = 0; i < testDataCount; ++i)
        dSrc[i] = (double)(rand() * (1.0 / RAND_MAX) * (rand() - (RAND_MAX >> 1)) * rand() * (1.0 / RAND_MAX));

    printf("--------------------------------------------------------------\n\n");

    // test
    sw.restart();
    for (long c = 0; c < testCount; ++c)
        dtol_test_0();
    sw.stop();

    // out
    printf("dtol_test_0(),          Result = %ld, time = %0.3f ms\n", testResult, sw.getMillisec());
    printf("\n");

    // test
    sw.restart();
    for (long c = 0; c < testCount; ++c)
        dtol_test_fpu();
    sw.stop();

    // out
    printf("dtol_test_fpu(),        Result = %ld, time = %0.3f ms\n", testResult, sw.getMillisec());
    printf("\n");

    // test
    sw.restart();
    for (long c = 0; c < testCount; ++c)
        dtol_test_ieee();
    sw.stop();

    // out
    printf("dtol_test_ieee(),       Result = %ld, time = %0.3f ms\n", testResult, sw.getMillisec());
    printf("\n");

    // test
    sw.restart();
    for (long c = 0; c < testCount; ++c)
        dtol_test_ieee_MagicNumber();
    sw.stop();

    // out
    printf("dtol_test_ieee_MagicNumber(),   Result = %ld, time = %0.3f ms\n", testResult, sw.getMillisec());
    printf("\n");

    // test
    sw.restart();
    for (long c = 0; c < testCount; ++c)
        dtol_test_ieee_MagicNumber2();
    sw.stop();

    // out
    printf("dtol_test_ieee_MagicNumber2(),  Result = %ld, time = %0.3f ms\n", testResult, sw.getMillisec());
    printf("\n");

    // test
    sw.restart();
    for (long c = 0; c < testCount; ++c)
        dtol_test_sse2();
    sw.stop();

    // out
    printf("dtol_test_sse2(),               Result = %ld, time = %0.3f ms\n", testResult, sw.getMillisec());
    printf("\n");
}
