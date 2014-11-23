//
// UnitTest.cpp : 定义控制台应用程序的入口点。
//

// whether use crtdbg check? / 是否使用 crtdbg 的内存泄露或越界检测?
#ifndef JIMI_USE_CRTDBG_CHECK
#define JIMI_USE_CRTDBG_CHECK    1
#endif

#include "UnitTest.h"

// crtdbg_env.h must be front the stdlib.h
// crtdbg_env.h 必须放在 stdlib.h 声明之前
#include "jimic/system/crtdbg_env.h"

#include <stdio.h>
#include <stdlib.h>
#ifdef _MSC_VER
#include <conio.h>
#endif // _MSC_VER
#include <time.h>

#if !defined(JIMIC_MSVC_CLANG) || (JIMIC_MSVC_CLANG == 0)
#include <intrin.h>
#endif

#include <math.h>       /* for _isnanf(), sqrt() */
#include <float.h>

#include <cstdlib>
#include <cstdio>
#include <string>
///*
#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>
#if !defined(_MSC_VER) || (_MSC_VER < 1600)
#include <locale>
#endif
#include <algorithm>
#include <functional>
//*/

#if defined(JIMI_HAS_CXX11_VARIADIC_TEMPLATES) && (JIMI_HAS_CXX11_VARIADIC_TEMPLATES != 0)
#include <cctype>
#endif

//#include <type_traits>
#include <array>        // for std::array

#include "jimi/core/jimi_def.h"

#include "jimic/system/console.h"
#include "jimic/string/dtos.h"
#include "jimic/string/string.h"
#include "jimic/stdio/sprintf.h"
#include "jimic/stdio/csharp_sprintf.h"

#include "jimic/string/jmf_strings.h"
#include "jimic/string/iconv_win.h"
#if JIMI_IS_WINDOWS
#include "jimic/platform/win/fast_memcpy.h"
#else
#include "jimic/platform/linux/fast_memcpy.h"
#endif

#include "jimi/util/cmd_line.h"
#include "jimi/thread/Thread.h"
#include "jimi/thread/Event.h"
#include "jimi/mt/mutex.h"
#include "jimi/mt/scoped_lock.h"
#include "jimi/system/stop_watch.h"

#include "jimi/lang/Object.h"
#include "jimi/lang/String.h"
#include "jimi/lang/Formatter.h"

#include "jimi/lang/SmallString.h"

#include "jimi/log/Logger.h"
#include "jimi/system/Program.h"
#include "jimi/system/Console.h"

#if defined(JIMI_HAS_BOOST_LOCALE) && (JIMI_HAS_BOOST_LOCALE != 0)
#include <boost/locale.hpp>
#include <boost/locale/encoding.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#endif

#if defined(JIMI_HAS_CXX11_VARIADIC_TEMPLATES) && (JIMI_HAS_CXX11_VARIADIC_TEMPLATES != 0)
#include "cpp11_format.h"
#endif

#include "SampleThread.h"

#include "UnitTests/ftol_test.h"
#include "UnitTests/snprintf_test.h"
#include "UnitTests/String_StrLen_Test.h"
#include "UnitTests/String_StrLwr_Test.h"
#include "FastMemcpy.h"

#if !defined(JIMIC_MSVC_CLANG) || (JIMIC_MSVC_CLANG == 0)

/**
 * for asmlib header file
 */
#include "asmlib/asmlib.h"

/**
 * for asmlib lib import
 */
#pragma comment(lib, "libacof32.lib")

#else

void * A_memcpy (void * dest, const void * src, size_t count)
{
    return memcpy(dest, src, count);
}

#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#if defined(_MSC_VER) && (_MSC_VER != 0)
#pragma warning(push)

#pragma warning(disable : 4995)         // 禁止不带_s函数的废弃warning信息
#pragma warning(disable : 4996)
#endif  /* _MSC_VER */

/**
 * for asmlib
 */
#pragma comment(lib, "libacof32.lib")

/**
 * for boost::locale
 */
#if defined(JIMI_HAS_BOOST_LOCALE) && (JIMI_HAS_BOOST_LOCALE != 0)
#pragma comment(lib, "libboost_locale-vc120-mt-gd-1_55.lib")
#endif

using namespace jimi;
using namespace std;

USING_NS_UNITEST

NS_UNITEST_BEGIN

#define SetCPUAffinityMask2(m1, m2)             ((((m2) & 1) << 1) | ((m1) & 1))
#define SetCPUAffinityMask3(m1, m2, m3)         ((((m3) & 1) << 2) | (((m2) & 1) << 1) | ((m1) & 1))
#define SetCPUAffinityMask4(m1, m2, m3, m4)     ((((m4) & 1) << 3) | (((m3) & 1) << 2) | (((m2) & 1) << 1) | ((m1) & 1))

#define SetCPUAffinityMask8(m1, m2, m3, m4, m5, m6, m7, m8) \
    ((SetCPUAffinityMask4(m5, m6, m7, m8) << 4) | SetCPUAffinityMask4(m1, m2, m3, m4))

//
// linux: sched_setaffinity()
//        http://man7.org/linux/man-pages/man2/sched_setaffinity.2.html
//

void jimi_set_thread_affinity(uint32_t dwCPUMask)
{
#if defined(_MSC_VER)
    bool echo = false;
#if defined(_WIN32) || defined(_WIN64)
    if (echo)
        printf("\n");
    HANDLE hCurrentProcess = GetCurrentProcess();
    DWORD dwProcessAffinity = 0, dwSystemAffinity = 0;
    DWORD dwAffinityMask = SetCPUAffinityMask4(1, 0, 0, 0);
    BOOL bAffResult;
    if (dwCPUMask != 0)
        dwAffinityMask = dwCPUMask;
    bAffResult = GetProcessAffinityMask(hCurrentProcess, &dwProcessAffinity, &dwSystemAffinity);
    if (bAffResult) {
        if (dwProcessAffinity != dwSystemAffinity) {
            if (echo)
                printf("This process can not utilize all processors.\n");
        }

        while ((dwAffinityMask != 0) && (dwAffinityMask <= dwProcessAffinity)) {
            // Check to make sure we can utilize this processsor first.
            if ((dwAffinityMask & dwProcessAffinity) != 0) {
                bAffResult = SetProcessAffinityMask(hCurrentProcess, dwAffinityMask);
                if (bAffResult) {
                    // Wait for the process affinity effected
                    Sleep(0);
                    //
                    if (echo)
                        printf("SetProcessAffinityMask(): dwAffinityMask = 0x%08X\n", dwAffinityMask);
                    DWORD dwProcessAffinityNew = 0;
                    bAffResult = GetProcessAffinityMask(hCurrentProcess, &dwProcessAffinityNew, &dwSystemAffinity);
                    if (dwProcessAffinityNew == dwAffinityMask) {
                        if (echo)
                            printf("SetProcessAffinityMask(): Success.\n");
                        bAffResult = SetThreadAffinityMask(GetCurrentThread(), dwAffinityMask);
                        Sleep(0);
                        break;
                    }
                }
            }
        }
    }
    if (echo)
        printf("\n");
#endif  /* _WIN32 || _WIN64 */
#endif  /* _MSC_VER */
}

/* 测试std::string是否使用了COW(Copy On Write) */

void String_Copy_On_Write_Test()
{
#if 0
    std::string str1 = "hello world";
    std::string str2 = str1;

    printf ("Sharing the memory:\n");
    printf ("\tstr1's address: %x\n", str1.c_str() );
    printf ("\tstr2's address: %x\n", str2.c_str() );

    str1[1] = 'q';
    str2[1] = 'w';

    printf ("After Copy-On-Write:\n");
    printf ("\tstr1's address: %x\n", str1.c_str() );
    printf ("\tstr2's address: %x\n", str2.c_str() );

    printf("\n");
#endif
}

void String_Performance_Test()
{
    jimi::stop_watch sw;
#ifndef _DEBUG
    const int LOOP_TIMES = 1000000;
#else
    const int LOOP_TIMES = 2000;
#endif
    size_t len1, len2;
    char buffer1[512];
    char buffer2[512];
    double time1, time2, time3, time4;
    double time5, time6, time7, time8;
    int i, j = 0, loop_times = 0;

    printf("==============================================================================\n\n");
    printf("  String_Performance_Test()\n\n");
    printf("==============================================================================\n\n");

    sw.restart();
    for (i = 0; i < (LOOP_TIMES >> 0); ++i) {
        strcpy(buffer1, "abcdefghijk");
        //strcpy(buffer1 + (i & 2), "abcdefghijk");
        /*
        strcpy(buffer1 + 0, "abcdefghijk");
        strcpy(buffer1 + 4, "abcdefghijk");
        strcpy(buffer1 + 0, "abcdefghijk");
        strcpy(buffer1 + 4, "abcdefghijk");
        //*/
        len1 = strlen(buffer1);
    }
    sw.stop();
    time1 = sw.getMillisec();

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        strcpy_s(buffer2, jm_countof(buffer2), "abcdefghijk");
        len2 = strlen("abcdefghijk");
    }
    sw.stop();
    time2 = sw.getMillisec();

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        //jimi::String str = "abcdefghijk";
        //jimi::String str("abcdefghijk", sizeof("abcdefghijk") - 1);
        jimi::String str("abcdefghijk");
    }
    sw.stop();
    time3 = sw.getMillisec();

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        //std::string str = "abcdefghijk";
        //std::string str("abcdefghijk", sizeof("abcdefghijk") - 1);
        std::string str("abcdefghijk");
    }
    sw.stop();
    time4 = sw.getMillisec();

    printf("buffer1 = %s, len1 = %d, len2 = %d\n\n", buffer1, len1, len2);

    sw.restart();
    for (i = 0; i < (LOOP_TIMES >> 0); ++i) {
        strcpy(buffer1, "abcdefghijklmnopqrstuvwxyz");
        //strcpy(buffer1 + (i & 2), "abcdefghijklmnopqrstuvwxyz");
        /*
        strcpy(buffer1 + 0, "abcdefghijklmnopqrstuvwxyz");
        strcpy(buffer1 + 4, "abcdefghijklmnopqrstuvwxyz");
        strcpy(buffer1 + 0, "abcdefghijklmnopqrstuvwxyz");
        strcpy(buffer1 + 4, "abcdefghijklmnopqrstuvwxyz");
        //*/
        len1 = strlen(buffer1);
    }
    sw.stop();
    time5 = sw.getMillisec();

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        strcpy_s(buffer2, jm_countof(buffer2), "abcdefghijklmnopqrstuvwxyz");
        len2 = strlen(buffer2);
    }
    sw.stop();
    time6 = sw.getMillisec();

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        //jimi::String str = "abcdefghijklmnopqrstuvwxyz";
        //jimi::String str("abcdefghijklmnopqrstuvwxyz", sizeof("abcdefghijklmnopqrstuvwxyz") - 1);
        jimi::String str("abcdefghijklmnopqrstuvwxyz");
    }
    sw.stop();
    time7 = sw.getMillisec();

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        //std::string str = "abcdefghijklmnopqrstuvwxyz";
        //std::string str("abcdefghijklmnopqrstuvwxyz", sizeof("abcdefghijklmnopqrstuvwxyz") - 1);
        std::string str("abcdefghijklmnopqrstuvwxyz");
    }
    sw.stop();
    time8 = sw.getMillisec();

    printf("buffer1 = %s, len1 = %d, len2 = %d\n\n", buffer1, len1, len2);

    printf("%-30s time = %0.5f ms.\n", "strcpy()     str = \"abcdefghijk\";",   time1);
    printf("%-30s time = %0.5f ms.\n", "strcpy_s()   str = \"abcdefghijk\";",   time2);
    printf("%-30s time = %0.5f ms.\n", "std::string  str = \"abcdefghijk\";",   time4);
    printf("%-30s time = %0.5f ms.\n", "jimi::String str = \"abcdefghijk\";",   time3);
    printf("\n");
    printf("%-30s time = %0.5f ms.\n", "strcpy()     str = \"abcdefg...xyz\";", time5);
    printf("%-30s time = %0.5f ms.\n", "strcpy_s()   str = \"abcdefg...xyz\";", time6);
    printf("%-30s time = %0.5f ms.\n", "std::string  str = \"abcdefg...xyz\";", time8);
    printf("%-30s time = %0.5f ms.\n", "jimi::String str = \"abcdefg...xyz\";", time7);

    printf("\n");
}

void String_Performance_Test2()
{
#ifndef _DEBUG
    const int LOOP_TIMES = 1000000;
#else
    const int LOOP_TIMES = 2000;
#endif
    size_t len1, len2;
    char buffer1[512];
    char buffer2[512];
    double time1, time2, time3, time4;
    double time5, time6, time7, time8;
    int i, j = 0, loop_times = 0;
    jimi::stop_watch sw;

    printf("==============================================================================\n\n");
    printf("  String_Performance_Test2()\n\n");
    printf("==============================================================================\n\n");

    sw.restart();
    for (i = 0; i < (LOOP_TIMES >> 0); ++i) {
        strcpy(buffer1, "a");
        //strcpy(buffer1 + (i & 2), "abcdefghijk");
        /*
        strcpy(buffer1 + 0, "abcdefghijk");
        strcpy(buffer1 + 4, "abcdefghijk");
        strcpy(buffer1 + 0, "abcdefghijk");
        strcpy(buffer1 + 4, "abcdefghijk");
        //*/
        len1 = strlen(buffer1);
    }
    sw.stop();
    time1 = sw.getMillisec();

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        strcpy_s(buffer2, jm_countof(buffer2), "a");
        len2 = strlen("a");
    }
    sw.stop();
    time2 = sw.getMillisec();

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        //jimi::String str = "a";
        //jimi::String str("a", sizeof("abcdefghijk") - 1);
        jimi::String str('a');
    }
    sw.stop();
    time3 = sw.getMillisec();

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        //std::string str = 'a';
        std::string str(1, 'a');
    }
    sw.stop();
    time4 = sw.getMillisec();

    printf("buffer1 = %s, len1 = %d, len2 = %d\n\n", buffer1, len1, len2);

    sw.restart();
    for (i = 0; i < (LOOP_TIMES >> 0); ++i) {
        strcpy(buffer1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
        len1 = strlen(buffer1);
    }
    sw.stop();
    time5 = sw.getMillisec();

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        strcpy_s(buffer2, jm_countof(buffer2), "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
        len2 = strlen(buffer2);
    }
    sw.stop();
    time6 = sw.getMillisec();

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        //jimi::String str = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";
        //jimi::String str("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz", sizeof("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz") - 1);
        jimi::String str("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    }
    sw.stop();
    time7 = sw.getMillisec();

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        //std::string str = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";
        //std::string str("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz", sizeof("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz") - 1);
        std::string str("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
    }
    sw.stop();
    time8 = sw.getMillisec();

    printf("buffer1 = %s, len1 = %d, len2 = %d\n\n", buffer1, len1, len2);

    printf("%-30s time = %0.5f ms.\n", "strcpy()     str = \"a\";",   time1);
    printf("%-30s time = %0.5f ms.\n", "strcpy_s()   str = \"a\";",   time2);
    printf("%-30s time = %0.5f ms.\n", "std::string  str = \"a\";",   time4);
    printf("%-30s time = %0.5f ms.\n", "jimi::String str = \"a\";",   time3);
    printf("\n");
    printf("%-30s time = %0.5f ms.\n", "strcpy()     str = \"abcdefg...xyz\";", time5);
    printf("%-30s time = %0.5f ms.\n", "strcpy_s()   str = \"abcdefg...xyz\";", time6);
    printf("%-30s time = %0.5f ms.\n", "std::string  str = \"abcdefg...xyz\";", time8);
    printf("%-30s time = %0.5f ms.\n", "jimi::String str = \"abcdefg...xyz\";", time7);

    printf("\n");
}

static char jabberwocky[] =
        "'Twas brillig, and the slithy toves\n"
        "Did gyre and gimble in the wabe:\n"
        "All mimsy were the borogoves,\n"
        "And the mome raths outgrabe.\n"

        "Beware the Jabberwock, my son!\n"
        "The jaws that bite, the claws that catch!\n"
        "Beware the Jubjub bird, and shun\n"
        "The frumious Bandersnatch!\n"

        "He took his vorpal sword in hand:\n"
        "Long time the manxome foe he sought -\n"
        "So rested he by the Tumturn tree,\n"
        "And stood awhile in thought.\n"

        "And, as in uffish thought he stood;\n"
        "The Jabberwock, with eyes of flame,\n"
        "Came whiffling through the tulgey wood,\n"
        "And burbled as it came!\n"

        "One, two! One, two! And through and through\n"
        "The vorpal blade went snicker-snackl\n"
        "He left it dead, and with its head\n"
        "He went galumphing back.\n"

        "And hast thou slain the Jabberwock?\n"
        "Come to my arms, my beamish boy!\n"
        "O frabjous day! Callooh! Callay!\n"
        "He chortled in his joy.\n"

        "'Twas brillig, and the slithy toves\n"
        "Did gyre and gimble in the wabe:\n"
        "All mimsy were the borogoves,\n"
        "And the mome raths outgrabe.";

/* String类的基础测试 */

void String_Base_Test()
{
    int i;
    double time;
    int loop_times = 9999999;
    jimi::stop_watch sw;

    printf("==============================================================================\n\n");
    printf("  String_Base_Test()\n\n");
    printf("==============================================================================\n\n");

#if 1
    jimi::String str1 = "abcdefg";
    jimi::String str2 = "hijklmnop";
    printf("str1.c_str() = %s\n", str1.c_str());
    printf("str2.c_str() = %s\n\n", str2.c_str());
    printf("? (str1 == str2) = %d\n", (str1 == str2));
    printf("? (str1 == str1) = %d\n\n", (str1 == str1));

    str1.swap(str2);

    printf("str1.swap(str2);\n\n");
    printf("str1.c_str() = %s\n", str1.c_str());
    printf("str2.c_str() = %s\n\n", str2.c_str());

    printf("sizeof(str1) = %d bytes\n\n", sizeof(str1));

    str1 = str2;
    printf("str1 = str2;\n");
    printf("str1.c_str() = %s\n\n", str1.c_str());

    str1 = "i21938219";
    printf("str1 = \"i21938219\";\n");
    printf("str1.c_str() = %s\n", str1.c_str());
    printf("\n");

    str2.append("00abcd");
    str2.append(str1);

    printf("str2.c_str() = %s\n", str2.c_str());
    printf("\n");

    jimi::String str3('a');
    printf("str3.c_str() = %s\n", str3.c_str());
    printf("\n");

    jimi::String str4;
    str4 = jabberwocky;
    printf("str4.c_str() = \n%s\n\n", str4.c_str());
    printf("str4.size()  = %d bytes\n", str4.size());
    printf("\n");

    jimi::String str5;
    //str5.append_format_c("%d %x %f %u %c %b", 9999, 8888, 10.9999, 10000000, 33, true);
    str5.append_format_c("%d %s %d %s", 111, "222erer", 33333, "ffffff44");
    printf("str5.c_str() = \n%s\n\n", str5.c_str());
    printf("str5.size()  = %d bytes\n", str5.size());
    printf("\n");

    jimi::String str6;
    //str6.format_c("{0}, {1}, {2}, {{3}, {3}", "%d %s %d %s", 111, "222erer", 33333, "ffffff44");
    str6.format("{0}, {1}, {2}, {{3}, {3}", 111, "222erer", 33333, "ffffff44");
    printf("str6.c_str() = \n%s\n\n", str6.c_str());
    printf("str6.size()  = %d bytes\n", str6.size());
    printf("\n");
#endif

#if 0
    int num1, num2;
    std::cout << "请输入两个数字: " << std::endl;
    std::cin  >> num1;
    std::cin  >> num2;
    std::cout << std::endl;

    char buf1[256], buf2[256];
    std::string in_str1, in_str2;

    std::cout << "请输入两个字符串(长度不能大于256): " << std::endl;
    std::cin  >> in_str1;
    std::cin  >> in_str2;
    std::cout << std::endl;
    std::cout << "done." << std::endl;

    fflush(stdout);

    jm_strcpy(buf1, jm_countof(buf1), in_str1.c_str());
    jm_strcpy(buf2, jm_countof(buf2), in_str2.c_str());    
#else
    int num1, num2;
    num1 = 222;
    num2 = 33333;

    char buf1[256], buf2[256];
    jm_strcpy(buf1, jm_countof(buf1), "222erer");
    jm_strcpy(buf2, jm_countof(buf2), "ffffff44");
#endif

#ifndef _DEBUG
    loop_times = 9999999;
#else
    loop_times = 9999;
#endif

    printf("==============================================================================\n\n");
    printf("  String_Base_Test()\n\n");
    printf("==============================================================================\n\n");

    /////////////////////////////////////////////////////////////////////////////////////////////////

    {
        int delta;
        jimi::String strTest((size_t)999999999);
#if 1
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            //strTest.format("{0}, {1}, {2}, {{3}, {3}", (unsigned int)111, "222erer", (unsigned long)33333, "ffffff44");
            strTest.format("{0}, {1}, {2}, {{3}, {3}", (unsigned int)num1, buf1, (unsigned long)num2, buf2);
        }
        sw.stop();
        time = sw.getMillisec();

        printf("==============================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        str.format(\"{0}, {1}, {2}, {{3}, {3}\",\n"
               "                    111, \"222erer\", 33333, \"ffffff44\");\n");
        printf("    }\n\n");
        printf("==============================================================================\n\n");

        jimi::String strTest2((size_t)128);
        //strTest2.format("{0}, {1}, {2}, {{3}, {3}", 111, "222erer", 33333, "ffffff44");
        strTest2.format("{0}, {1}, {2}, {{3}, {3}", (unsigned int)num1, buf1, (unsigned long)num2, buf2);
        delta = strTest2.size();
#elif 0
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            strTest.format_c("{0}, {1}, {2}, {{3}, {3}", "%d %s %d %s", 111, "222erer", 33333, "ffffff44");
        }
        sw.stop();
        time = sw.getMillisec();

        printf("==============================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        str.format_c(\"{0}, {1}, {2}, {{3}, {3}\", \"%%d %%s %%d %%s\",\n"
               "                      111, \"222erer\", 33333, \"ffffff44\");\n");
        printf("    }\n\n");
        printf("==============================================================================\n\n");

        jimi::String strTest2((size_t)128);
        strTest2.format_c("{0}, {1}, {2}, {{3}, {3}", "%d %s %d %s", 111, "222erer", 33333, "ffffff44");
        delta = strTest2.size();
#else
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            strTest.append_format_c("%d, %s, %d, {3}, %s", 111, "222erer", 33333, "ffffff44");
        }
        sw.stop();
        time = sw.getMillisec();

        printf("==============================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        str.append_format_c(\"%%d, %%s, %%d, {3}, %%s\",\n"
               "                            111, \"222erer\", 33333, \"ffffff44\");\n");
        printf("    }\n\n");
        printf("==============================================================================\n\n");

        jimi::String strTest2((size_t)128);
        strTest2.append_format_c("%d, %s, %d, {3}, %s", 111, "222erer", 33333, "ffffff44");
        delta = strTest2.size();
#endif

        printf("str.c_str() = %s\n\n", strTest2.c_str());

        printf("time = %0.3f ms, delta = %d.\n\n", time, delta);
        printf("strTest.size()  = %d bytes\n", strTest.size());
        printf("\n");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////

    {
        int delta;
        jimi::String strTest((size_t)999999999);
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            strTest.append_format((unsigned int)111, ", ", "222erer", ", ", (unsigned long)33333, ", ", "{3}, ", "ffffff44");
        }
        sw.stop();
        time = sw.getMillisec();

        printf("==============================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        str.append_format(111, \", \", \"222erer\", \", \",\n"
               "                          33333, \", \", \"{3}, \", \"ffffff44\");\n");
        printf("    }\n\n");
        printf("==============================================================================\n\n");

        jimi::String strTest2((size_t)128);
        strTest2.append_format((unsigned int)111, ", ", "222erer", ", ", (unsigned long)33333, ", ", "{3}, ", "ffffff44");
        delta = strTest2.size();

        printf("str.c_str() = %s\n\n", strTest2.c_str());

        printf("time = %0.3f ms, delta = %d.\n\n", time, delta);
        printf("strTest.size()  = %d bytes\n", strTest.size());
        printf("\n");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////

    {
        int delta;
        jimi::String strTest((size_t)999999999);
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            strTest.append_format((unsigned int)num1, ", ", buf1, ", ", (unsigned long)num2, ", ", "{3}, ", buf2);
        }
        sw.stop();
        time = sw.getMillisec();

        printf("==============================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        str.append_format(num1, \", \", buf1, \", \",\n"
               "                          num2, \", \", \"{3}, \", buf2);\n");
        printf("    }\n\n");
        printf("==============================================================================\n\n");

        jimi::String strTest2((size_t)128);
        strTest2.append_format((unsigned int)num1, ", ", buf1, ", ", (unsigned long)num2, ", ", "{3}, ", buf2);
        delta = strTest2.size();

        std::string strStd2 = strTest2.toStdString();

        printf("str.c_str() = %s\n\n", strTest2.c_str());

        printf("time = %0.3f ms, delta = %d.\n\n", time, delta);
        printf("strTest.size()  = %d bytes\n", strTest.size());
        printf("\n");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(JIMI_HAS_CXX11_VARIADIC_TEMPLATES) && (JIMI_HAS_CXX11_VARIADIC_TEMPLATES != 0)
    {
        int delta;
        //jimi::String strTest((size_t)1024);
        jimi::String strTest;
        jimi::formatter<> formator;
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            strTest = formator.append((unsigned int)111, ", ", "222erer", ", ", (unsigned long)33333, ", ", "{3}, ", "ffffff44");
        }
        sw.stop();
        time = sw.getMillisec();

        printf("==============================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        str = formator.append(111, \", \", \"222erer\", \", \",\n"
               "                              33333, \", \", \"{3}, \", \"ffffff44\");\n");
        printf("    }\n\n");
        printf("==============================================================================\n\n");

        jimi::String strTest2;
        strTest2 = formator.append((unsigned int)111, ", ", "222erer", ", ", (unsigned long)33333, ", ", "{3}, ", "ffffff44");
        delta = strTest2.size();

        printf("str.c_str() = %s\n\n", strTest2.c_str());

        printf("time = %0.3f ms, delta = %d.\n\n", time, delta);
        printf("strTest.size()  = %d bytes\n", strTest.size());
        printf("\n");
        printf("d\n");
    }
#endif  /* JIMI_HAS_CXX11_VARIADIC_TEMPLATES */

    /////////////////////////////////////////////////////////////////////////////////////////////////

#if 1
    {
        int delta;
        //jimi::String strTest((size_t)1024);
        jimi::String strTest;
        jimi::formatter<> formator;
        jimi::format_detail<> &fmt_detail  = jimi::format_detail<>().setFloat(jimi::detail::AlignRight, jimi::detail::FillNone, 3, 3)
                                        .setDoublePrecision(6).setDouble(0, 0, 3, 0);
        jimi::format_detail<> &fmt_detail2 = jimi::format_detail<>().setFloat("%0.3f").setDouble("%0.5f").setInt32("%-08d")
                                         .setInt64("%+020d").setString("%-30s");
                                         
        fmt_detail.doubles.align = jimi::detail::AlignRight;
        fmt_detail.doubles.fill  = jimi::detail::FillSpace;
        fmt_detail.doubles.width = 3;
        fmt_detail.doubles.precision = 6;
        fmt_detail.floats.setDetail('-', '0', 3, 6);

        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            //delta = formator.append_to(strTest, (unsigned int)111, ", ", "222erer", ", ", (unsigned long)33333, ", ", "{3}, ", "ffffff44");
            delta = formator.append_to(strTest, (unsigned int)num1, ", ", buf1, ", ", (unsigned long)num2, ", ", "{3}, ", buf2);
        }
        sw.stop();
        time = sw.getMillisec();

        printf("==============================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        n = formator.append_to(str, 111, \", \", \"222erer\", \", \",\n"
               "                               33333, \", \", \"{3}, \", \"ffffff44\");\n");
        printf("    }\n\n");
        printf("==============================================================================\n\n");

        jimi::String strTest2;
        delta = formator.append_to(strTest2, (unsigned int)111, ", ", "222erer", ", ", (unsigned long)33333, ", ", "{3}, ", "ffffff44");
        delta = strTest2.size();

        printf("str.c_str() = %s\n\n", strTest2.c_str());

        printf("time = %0.3f ms, delta = %d.\n\n", time, delta);
        printf("strTest.size()  = %d bytes\n", strTest.size());
        printf("\n");
    }
#endif

#if 1
    {
        int delta;
        //jimi::String strTest((size_t)1024);
        jimi::String strTest;
        jimi::formatter<> formator;
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            //strTest = formator.csharp_format_old("{0}, {1}, {2}, {{3}, {3}", (unsigned int)111, "222erer", (unsigned long)33333, "ffffff44");
            strTest = formator.csharp_format_old("{0}, {1}, {2}, {{3}, {3}", (unsigned int)num1, buf1, (unsigned long)num2, buf2);
        }
        sw.stop();
        time = sw.getMillisec();

        printf("==============================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        str = formator.csharp_format_old(\"{0}, {1}, {2}, {{3}, {3}\",\n"
               "                                         111, \"222erer\", 33333, \"ffffff44\");\n");
        printf("    }\n\n");
        printf("==============================================================================\n\n");

        jimi::String strTest2;
        //strTest2 = formator.csharp_format_old("{0}, {1}, {2}, {{3}, {3}", (unsigned int)111, "222erer", (unsigned long)33333, "ffffff44");
        strTest2 = formator.csharp_format_old("{0}, {1}, {2}, {{3}, {3}", (unsigned int)num1, buf1, (unsigned long)num2, buf2);
        delta = strTest2.size();

        printf("str.c_str() = %s\n\n", strTest2.c_str());

        printf("time = %0.3f ms, delta = %d.\n\n", time, delta);
        printf("strTest.size()  = %d bytes\n", strTest.size());
        printf("\n");
    }
#endif

#if 1
    {
        int delta;
        //jimi::String strTest((size_t)1024);
        jimi::String strTest;
        jimi::formatter<> formator;
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            //delta = formator.csharp_format_to_old(strTest, "{0}, {1}, {2}, {{3}, {3}", (unsigned int)111, "222erer", (unsigned long)33333, "ffffff44");
            delta = formator.csharp_format_to_old(strTest, "{0}, {1}, {2}, {{3}, {3}", (unsigned int)num1, buf1, (unsigned long)num2, buf2);
        }
        sw.stop();
        time = sw.getMillisec();

        printf("==============================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        n = formator.csharp_format_to_old(str, \"{0}, {1}, {2}, {{3}, {3}\",\n"
               "                                          111, \"222erer\", 33333, \"ffffff44\");\n");
        printf("    }\n\n");
        printf("==============================================================================\n\n");

        jimi::String strTest2;
        //delta = formator.csharp_format_to_old(strTest2, "{0}, {1}, {2}, {{3}, {3}", (unsigned int)111, "222erer", (unsigned long)33333, "ffffff44");
        delta = formator.csharp_format_to_old(strTest2, "{0}, {1}, {2}, {{3}, {3}", (unsigned int)num1, buf1, (unsigned long)num2, buf2);
        delta = strTest2.size();

        printf("str.c_str() = %s\n\n", strTest2.c_str());

        printf("time = %0.3f ms, delta = %d.\n\n", time, delta);
        printf("strTest.size()  = %d bytes\n", strTest.size());
        printf("\n");
    }
#endif

#if 1
    {
        int delta;
        jimi::String strTest;
        jimi::formatter<> formator;
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            //strTest = formator.csharp_format("{0}, {1}, {2}, {{3}, {3}", (unsigned int)111, "222erer", (unsigned long)33333, "ffffff44");
            strTest = formator.csharp_format("{0}, {1}, {2}, {{3}, {3}", (unsigned int)num1, buf1, (unsigned long)num2, buf2);
        }
        sw.stop();
        time = sw.getMillisec();

        printf("==============================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        str = formator.csharp_format(\"{0}, {1}, {2}, {{3}, {3}\",\n"
               "                                     111, \"222erer\", 33333, \"ffffff44\");\n");
        printf("    }\n\n");
        printf("==============================================================================\n\n");

        jimi::String strTest2;
        //strTest2 = formator.csharp_format("{0}, {1}, {2}, {{3}, {3}", (unsigned int)111, "222erer", (unsigned long)33333, "ffffff44");
        strTest2 = formator.csharp_format("{0}, {1}, {2}, {{3}, {3}", (unsigned int)num1, buf1, (unsigned long)num2, buf2);
        delta = strTest2.size();

        printf("str.c_str() = %s\n\n", strTest2.c_str());

        printf("time = %0.3f ms, delta = %d.\n\n", time, delta);
        printf("strTest.size()  = %d bytes\n", strTest.size());
        printf("\n");
    }
#endif

#if 1
    {
        int delta;
        jimi::String strTest;
        jimi::formatter<> formator;
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            //delta = formator.csharp_format_to(strTest, "{0}, {1}, {2}, {{3}, {3}", (unsigned int)111, "222erer", (unsigned long)33333, "ffffff44");
            delta = formator.csharp_format_to(strTest, "{0}, {1}, {2}, {{3}, {3}", (unsigned int)num1, buf1, (unsigned long)num2, buf2);
        }
        sw.stop();
        time = sw.getMillisec();

        printf("==============================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        n = formator.csharp_format_to(str, \"{0}, {1}, {2}, {{3}, {3}\",\n"
               "                                      111, \"222erer\", 33333, \"ffffff44\");\n");
        printf("    }\n\n");
        printf("==============================================================================\n\n");

        jimi::String strTest2;
        //delta = formator.csharp_format_to(strTest2, "{0}, {1}, {2}, {{3}, {3}", (unsigned int)111, "222erer", (unsigned long)33333, "ffffff44");
        delta = formator.csharp_format_to(strTest2, "{0}, {1}, {2}, {{3}, {3}", (unsigned int)num1, buf1, (unsigned long)num2, buf2);
        delta = strTest2.size();

        printf("str.c_str() = %s\n\n", strTest2.c_str());

        printf("time = %0.3f ms, delta = %d.\n\n", time, delta);
        printf("strTest.size()  = %d bytes\n", strTest.size());
        printf("\n");
    }
#endif

#if 1
    {
        int delta;
        jimi::String strTest;
        jimi::formatter<> formator;
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            //delta = formator.csharp_format_to_new(strTest, "{0}, {1}, {2}, {{3}, {3}", (unsigned int)111, "222erer", (unsigned long)33333, "ffffff44");
            delta = formator.csharp_format_to_new(strTest, "{0}, {1}, {2}, {{3}, {3}", (unsigned int)num1, buf1, (unsigned long)num2, buf2);
        }
        sw.stop();
        time = sw.getMillisec();

        printf("==============================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        n = formator.csharp_format_to_new(str, \"{0}, {1}, {2}, {{3}, {3}\",\n"
               "                                          111, \"222erer\", 33333, \"ffffff44\");\n");
        printf("    }\n\n");
        printf("==============================================================================\n\n");

        jimi::String strTest2;
        //delta = formator.csharp_format_to_new(strTest2, "{0}, {1}, {2}, {{3}, {3}", (unsigned int)111, "222erer", (unsigned long)33333, "ffffff44");
        delta = formator.csharp_format_to_new(strTest2, "{0}, {1}, {2}, {{3}, {3}", (unsigned int)num1, buf1, (unsigned long)num2, buf2);
        delta = strTest2.size();

        printf("str.c_str() = %s\n\n", strTest2.c_str());

        printf("time = %0.3f ms, delta = %d.\n\n", time, delta);
        printf("strTest.size()  = %d bytes\n", strTest.size());
        printf("\n");
    }
#endif

#if 1
    {
        int delta;
        jimi::String strTest;
        jimi::formatter<> formator;
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            delta = formator.format_fast_to(strTest, "?, ?, ?, ??3}, ?", (unsigned int)111, "222erer", (unsigned long)33333, "ffffff44");
            //delta = formator.format_fast_to(strTest, "?, ?, ?, ??3}, ?", (unsigned int)num1, buf1, (unsigned long)num2, buf2);
        }
        sw.stop();
        time = sw.getMillisec();

        printf("==============================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        n = formator.format_fast_to(str, \"?, ?, ?, ??3}, ?\",\n"
               "                                    111, \"222erer\", 33333, \"ffffff44\");\n");
        printf("    }\n\n");
        printf("==============================================================================\n\n");

        jimi::String strTest2;
        delta = formator.format_fast_to(strTest2, "?, ?, ?, ??3}, ?", (unsigned int)111, "222erer", (unsigned long)33333, "ffffff44");
        //delta = formator.format_fast_to(strTest2, "?, ?, ?, ??3}, ?", (unsigned int)num1, buf1, (unsigned long)num2, buf2);
        delta = strTest2.size();

        printf("str.c_str() = %s\n\n", strTest2.c_str());

        printf("time = %0.3f ms, delta = %d.\n\n", time, delta);
        printf("strTest.size()  = %d bytes\n", strTest.size());
        printf("\n");
    }
#endif

#if 1
    {
        int delta;
        jimi::String strTest;
        jimi::formatter<> formator;
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            delta = formator.format_fast_to_new(strTest, "?, ?, ?, ??3}, ?", (unsigned int)111, "222erer", (unsigned long)33333, "ffffff44");
            //delta = formator.format_fast_to_new(strTest, "?, ?, ?, ??3}, ?", (unsigned int)num1, buf1, (unsigned long)num2, buf2);
        }
        sw.stop();
        time = sw.getMillisec();

        printf("==============================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        n = formator.format_fast_to_new(str, \"?, ?, ?, ??3}, ?\",\n"
               "                                        111, \"222erer\", 33333, \"ffffff44\");\n");
        printf("    }\n\n");
        printf("==============================================================================\n\n");

        jimi::String strTest2;
        delta = formator.format_fast_to_new(strTest2, "?, ?, ?, ??3}, ?", (unsigned int)111, "222erer", (unsigned long)33333, "ffffff44");
        //delta = formator.format_fast_to_new(strTest2, "?, ?, ?, ??3}, ?", (unsigned int)num1, buf1, (unsigned long)num2, buf2);
        delta = strTest2.size();

        printf("str.c_str() = %s\n\n", strTest2.c_str());

        printf("time = %0.3f ms, delta = %d.\n\n", time, delta);
        printf("strTest.size()  = %d bytes\n", strTest.size());
        printf("\n");
    }
#endif

#if 1
    {
        int delta;
        //jimi::String strTest((size_t)1024);
        jimi::String strTest;
        jimi::formatter<> formator;
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            delta = formator.format_fast2_to(strTest, "?, ?, ?, ??3}, ?", (unsigned int)111, "222erer", (unsigned long)33333, "ffffff44");
            //delta = formator.format_fast2_to(strTest, "?, ?, ?, ??3}, ?", (unsigned int)num1, buf1, (unsigned long)num2, buf2);
        }
        sw.stop();
        time = sw.getMillisec();

        printf("==============================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        n = formator.format_fast2_to(str, \"?, ?, ?, ??3}, ?\",\n"
               "                                     111, \"222erer\", 33333, \"ffffff44\");\n");
        printf("    }\n\n");
        printf("==============================================================================\n\n");

        jimi::String strTest2;
        delta = formator.format_fast2_to(strTest2, "?, ?, ?, ??3}, ?", (unsigned int)111, "222erer", (unsigned long)33333, "ffffff44");
        //delta = formator.format_fast2_to(strTest2, "?, ?, ?, ??3}, ?", (unsigned int)num1, buf1, (unsigned long)num2, buf2);
        delta = strTest2.size();

        printf("str.c_str() = %s\n\n", strTest2.c_str());

        printf("time = %0.3f ms, delta = %d.\n\n", time, delta);
        printf("strTest.size()  = %d bytes\n", strTest.size());
        printf("\n");
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////

#if 0
    {
        int delta;
        std::string str_cpp11;
        str_cpp11.reserve(999999999);

        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            //str_cpp11 += 
                qicosmos::format("{0}, {1}, {2}, $3}, {3}", 111, "222erer", 33333, "ffffff44");
        }
        sw.stop();
        time = sw.getMillisec();

        printf("==============================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        str = format(\"{0}, {1}, {2}, $3}, {3}\",\n"
               "                      111, \"222erer\", 33333, \"ffffff44\");\n");
        printf("    }\n\n");
        printf("==============================================================================\n\n");

        std::string str_cpp11b;
        str_cpp11b = qicosmos::format("{0}, {1}, {2}, $3}, {3}", 111, "222erer", 33333, "ffffff44");
        delta = str_cpp11b.size();

        printf("str_cpp11.c_str() = %s\n\n", str_cpp11b.c_str());

        printf("time = %0.3f ms, delta = %d.\n\n", time, delta);
        printf("str_cpp11.size()  = %d bytes\n", str_cpp11.size());
        printf("\n");
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////

#if 0
    {
        int delta;
        std::string str_cfmt;
        str_cfmt.reserve(999999999);
        char buf[256];

        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            sprintf(buf, "%d, %s, %d, {3}, %s", 111, "222erer", 33333, "ffffff44");
            //str_cfmt += buf;
        }
        sw.stop();
        time = sw.getMillisec();

        printf("==============================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        str = sprintf(buf, \"%%d, %%s, %%d, {3}, %%s\",\n"
               "                      111, \"222erer\", 33333, \"ffffff44\");\n");
        printf("    }\n\n");
        printf("==============================================================================\n\n");

        std::string str_cfmt2;
        sprintf(buf, "%d, %s, %d, {3}, %s", 111, "222erer", 33333, "ffffff44");
        str_cfmt2 = buf;
        delta = str_cfmt2.size();

        printf("str_cpp11.c_str() = %s\n\n", str_cfmt2.c_str());

        printf("time = %0.3f ms, delta = %d.\n\n", time, delta);
        printf("str_cpp11.size()  = %d bytes\n", str_cfmt.size());
        printf("\n");
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////
}

void String_Format_Test()
{
    int i;
    double time;
    int loop_times = 9999999;
    jimi::stop_watch sw;

    printf("******************************************************************************\n\n");
    printf("String_Format_Test()\n\n");
    printf("******************************************************************************\n\n");

#ifndef _DEBUG
    loop_times = 999999;
#else
    loop_times = 9999;
#endif

#if 1
    {
        int delta;
        jimi::String strTest;
        jimi::formatter<> formator;
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            strTest = formator.csharp_format("{99} {98} {97} {96} {95}",
                                             1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                             1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                             1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                             1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                             1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                             1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                             1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                             1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                             1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                             1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
        }
        sw.stop();
        time = sw.getMillisec();

        printf("==============================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        str = formator.csharp_format(\"{99} {98} {97} {96} {95}\",\n"
               "                                     111, \", \", \"222erer\", \", \",\n"
               "                                     33333, \", \", \"{3}, \", \"ffffff44\");\n");
        printf("    }\n\n");
        printf("==============================================================================\n\n");

        jimi::String strTest2;
        strTest2 = formator.csharp_format("{99} {98} {97} {96} {95}",
                                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
        delta = strTest2.size();

        printf("str.c_str() = %s\n\n", strTest2.c_str());

        printf("time = %0.3f ms, delta = %d.\n\n", time, delta);
        printf("strTest.size()  = %d bytes\n", strTest.size());
        printf("\n");
    }
#endif

#if 1
    {
        int delta;
        jimi::String strTest;
        jimi::formatter<> formator;
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            //strTest.clear();
            delta = formator.csharp_format_to(strTest,
                                              //"{1} {1} {1} {1} {1}",
                                              "{99} {98} {97} {96} {95}",
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
        }
        sw.stop();
        time = sw.getMillisec();

        printf("==============================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        n = formator.csharp_format_to(str, \"{99} {98} {97} {96} {95}\",\n"
               "                                      111, \", \", \"222erer\", \", \",\n"
               "                                      33333, \", \", \"{3}, \", \"ffffff44\");\n");
        printf("    }\n\n");
        printf("==============================================================================\n\n");

        jimi::String strTest2;
        delta = formator.csharp_format_to(strTest2,
                                          //"{1} {1} {1} {1} {1}",
                                          "{99} {98} {97} {96} {95}",
                                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
        delta = strTest2.size();

        printf("str.c_str() = %s\n\n", strTest2.c_str());

        printf("time = %0.3f ms, delta = %d.\n\n", time, delta);
        printf("strTest.size()  = %d bytes\n", strTest.size());
        printf("\n");
    }
#endif

#if 1
    {
        int delta;
        jimi::String strTest;
        jimi::formatter<> formator;
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            //strTest.clear();
            delta = formator.csharp_format_to_new(strTest,
                                                  //"{1} {1} {1} {1} {1}",
                                                  "{99} {98} {97} {96} {95}",
                                                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
        }
        sw.stop();
        time = sw.getMillisec();

        printf("==============================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        n = formator.csharp_format_to_new(str, \"{99} {98} {97} {96} {95}\",\n"
               "                                          111, \", \", \"222erer\", \", \",\n"
               "                                          33333, \", \", \"{3}, \", \"ffffff44\");\n");
        printf("    }\n\n");
        printf("==============================================================================\n\n");

        jimi::String strTest2;
        delta = formator.csharp_format_to_new(strTest2,
                                              //"{1} {1} {1} {1} {1}",
                                              "{99} {98} {97} {96} {95}",
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
        delta = strTest2.size();

        printf("str.c_str() = %s\n\n", strTest2.c_str());

        printf("time = %0.3f ms, delta = %d.\n\n", time, delta);
        printf("strTest.size()  = %d bytes\n", strTest.size());
        printf("\n");
    }
#endif

#if 1
    {
        int a, b, c, d, e, f, g, h, i, j;
        int k, l, m, n, o, p, q, r, s, t;
        int u, v, w, x, y, z;
        int delta;
        jimi::String strTest;
        jimi::formatter<> formator;
        // 从 k 到 z 是故意不初始化的, 看编译器会优化成什么样子
        a = b = c = d = e = 1;
        f = g = h = i = j = 3;
        k = l = m = n = o = 5;
        p = q = r = s = t = 7;
        u = v = w = x = y = z = 9;
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            //strTest.clear();
            delta = formator.csharp_format_to_new(strTest,
                                                  //"{1} {1} {1} {1} {1}",
                                                  "{99} {98} {97} {96} {95}",
                                                  a, b, c, d, e, f, g, h, i, j,
                                                  k, l, m, n, o, p, q, r, s, t,
                                                  u, v, w, x, y, z, a, b, c, d,
                                                  a, b, c, d, e, f, g, h, i, j,
                                                  k, l, m, n, o, p, q, r, s, t,
                                                  u, v, w, x, y, z, a, b, c, d,
                                                  a, b, c, d, e, f, g, h, i, j,
                                                  k, l, m, n, o, p, q, r, s, t,
                                                  u, v, w, x, y, z, a, b, c, d,
                                                  a, b, c, d, e, f, g, h, i, j,
                                                  2, 2, 2, 2, 2, 2, 2, 2, 2, 2);
        }
        sw.stop();
        time = sw.getMillisec();

        printf("==============================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        n = formator.csharp_format_to_new(str, \"{99} {98} {97} {96} {95}\",\n"
               "                                          111, \", \", \"222erer\", \", \",\n"
               "                                          33333, \", \", \"{3}, \", \"ffffff44\");\n");
        printf("    }\n\n");
        printf("==============================================================================\n\n");

        jimi::String strTest2;
        delta = formator.csharp_format_to_new(strTest2,
                                              //"{1} {1} {1} {1} {1}",
                                              "{99} {98} {97} {96} {95}",
                                              a, b, c, d, e, f, g, h, i, j,
                                              k, l, m, n, o, p, q, r, s, t,
                                              u, v, w, x, y, z, a, b, c, d,
                                              a, b, c, d, e, f, g, h, i, j,
                                              k, l, m, n, o, p, q, r, s, t,
                                              u, v, w, x, y, z, a, b, c, d,
                                              a, b, c, d, e, f, g, h, i, j,
                                              k, l, m, n, o, p, q, r, s, t,
                                              u, v, w, x, y, z, a, b, c, d,
                                              a, b, c, d, e, f, g, h, i, j,
                                              2, 2, 2, 2, 2, 2, 2, 2, 2, 2);
        delta = strTest2.size();

        printf("str.c_str() = %s\n\n", strTest2.c_str());

        printf("time = %0.3f ms, delta = %d.\n\n", time, delta);
        printf("strTest.size()  = %d bytes\n", strTest.size());
        printf("\n");
    }
#endif

#if 1
    {
        int delta;
        jimi::String strTest;
        jimi::formatter<> formator;
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            //strTest.clear();
            delta = formator.csharp_format_to_old(strTest,
                                                  //"{1} {1} {1} {1} {1}",
                                                  "{99} {98} {97} {96} {95}",
                                                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                  1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
        }
        sw.stop();
        time = sw.getMillisec();

        printf("==============================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        n = formator.csharp_format_to_old(str, \"{99} {98} {97} {96} {95}\",\n"
               "                                          111, \", \", \"222erer\", \", \",\n"
               "                                          33333, \", \", \"{3}, \", \"ffffff44\");\n");
        printf("    }\n\n");
        printf("==============================================================================\n\n");

        jimi::String strTest2;
        delta = formator.csharp_format_to_old(strTest2,
                                              //"{1} {1} {1} {1} {1}",
                                              "{99} {98} {97} {96} {95}",
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
        delta = strTest2.size();

        printf("str.c_str() = %s\n\n", strTest2.c_str());

        printf("time = %0.3f ms, delta = %d.\n\n", time, delta);
        printf("strTest.size()  = %d bytes\n", strTest.size());
        printf("\n");
    }
#endif

#if 1
    {
        int delta;
        jimi::String strTest;
        jimi::formatter<> formator;
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            //strTest.clear();
            delta = formator.format_fast_to_new(strTest,
                                                //"{1} {1} {1} {1} {1}",
                                                "? ? ? ? ?",
                                                1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                                1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
        }
        sw.stop();
        time = sw.getMillisec();

        printf("==============================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        n = formator.format_fast_to_new(str, \"? ? ? ? ?\",\n"
               "                                        111, \", \", \"222erer\", \", \",\n"
               "                                        33333, \", \", \"{3}, \", \"ffffff44\");\n");
        printf("    }\n\n");
        printf("==============================================================================\n\n");

        jimi::String strTest2;
        delta = formator.format_fast_to_new(strTest2,
                                            //"{1} {1} {1} {1} {1}",
                                            "? ? ? ? ?",
                                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
        delta = strTest2.size();

        printf("str.c_str() = %s\n\n", strTest2.c_str());

        printf("time = %0.3f ms, delta = %d.\n\n", time, delta);
        printf("strTest.size()  = %d bytes\n", strTest.size());
        printf("\n");
    }
#endif
}

#define USE_BUFFER_VERIFY   0

void Memcpy_Test()
{
#ifndef _DEBUG
    const int LOOP_TIMES = 16;
#else
    const int LOOP_TIMES = 4;
#endif
    const unsigned int BUFFER_SIZE = 64 * 1024 * 1024;
    unsigned int buf_size;
    void *buffer1, *buffer2, *buffer3, *buffer4;
    void *buffer5, *buffer6, *buffer7, *buffer8;
    double time1, time2, time3, time4;
    double time5, time6, time7, time8;
    int i, j, loop_times;
    jimi::stop_watch sw;
#if USE_BUFFER_VERIFY
    bool verify_ok1, verify_ok2;
#endif

    char bufsize_text[256];
    unsigned int bufsize_list[] = {
        // 16, 32,
        64, 128, 256, 512,
        //
        1024 * 1, 1024 * 2, 1024 * 4, 1024 * 8, 1024 * 16,
        1024 * 32, 1024 * 64, 1024 * 128, 1024 * 256,
        //
        1048576 * 1, 1048576 * 2, 1048576 * 4, 1048576 * 8,
        1048576 * 16, 1048576 * 32, 1048576 * 64
    };

    buffer1 = ::_aligned_malloc(BUFFER_SIZE, 64);
    buffer2 = ::_aligned_malloc(BUFFER_SIZE, 64);
    buffer3 = ::_aligned_malloc(BUFFER_SIZE, 64);
    buffer4 = ::_aligned_malloc(BUFFER_SIZE, 64);

    buffer5 = ::_aligned_malloc(BUFFER_SIZE, 64);
    buffer6 = ::_aligned_malloc(BUFFER_SIZE, 64);
    buffer7 = ::_aligned_malloc(BUFFER_SIZE, 64);
    buffer8 = ::_aligned_malloc(BUFFER_SIZE, 64);

#if 1
    for (j = 0; j < jm_countof(bufsize_list); ++j) {
        buf_size = bufsize_list[j];

        if (buf_size <= 1024)
            loop_times = (8 * 1024 * 1024 / buf_size) * LOOP_TIMES;
        else if (buf_size <= 64 * 1024)
            loop_times = (4 * 1024 * 1024 / buf_size) * LOOP_TIMES;
        else
            loop_times = LOOP_TIMES;

        sw.restart();
        ::memcpy(buffer1, buffer2, buf_size);
        sw.stop();
        time1 = sw.getMillisec();

        sw.restart();
        for (i = 0; i < loop_times; ++i)
            ::memcpy(buffer1, buffer2, buf_size);
        sw.stop();
        time5 = sw.getMillisec();

        sw.restart();
        A_memcpy(buffer3, buffer4, buf_size);
        sw.stop();
        time2 = sw.getMillisec();

        sw.restart();
        for (i = 0; i < loop_times; ++i)
            A_memcpy(buffer3, buffer4, buf_size);
        sw.stop();
        time6 = sw.getMillisec();

#if USE_BUFFER_VERIFY
        buffer_set_val(buffer1, 0, buf_size);
        buffer_set_val(buffer2, 1, buf_size);
#endif
        sw.restart();
        fast_memcpy(buffer5, buffer6, buf_size);
        sw.stop();
        time3 = sw.getMillisec();

        sw.restart();
        for (i = 0; i < loop_times; ++i)
            fast_memcpy(buffer5, buffer6, buf_size);
        sw.stop();
        time7 = sw.getMillisec();

#if USE_BUFFER_VERIFY
        verify_ok1 = verify_buffer_val(buffer5, buf_size, 1);

        buffer_set_val(buffer3, 0, buf_size);
        buffer_set_val(buffer4, 1, buf_size);
#endif
        sw.restart();
        memcpy_mmx4(buffer7, buffer8, buf_size);
        sw.stop();
        time4 = sw.getMillisec();

        sw.restart();
        for (i = 0; i < loop_times; ++i)
            memcpy_mmx4(buffer7, buffer8, buf_size);
        sw.stop();
        time8 = sw.getMillisec();

#if USE_BUFFER_VERIFY
        verify_ok2 = verify_buffer_val(buffer7, buf_size, 1);
#endif

        bufsize_text[0] = '\0';
        get_bytes_display(bufsize_text, jm_countof(bufsize_text), buf_size);

        printf("%-16s size = %5s, time = %0.5f ms\n", "::memcpy()",     bufsize_text, time1);
        printf("%-16s size = %5s, time = %0.5f ms\n", "A_memcpy()",     bufsize_text, time2);
        printf("%-16s size = %5s, time = %0.5f ms\n", "fast_memcpy()",  bufsize_text, time3);
        printf("%-16s size = %5s, time = %0.5f ms\n", "memcpy_mmx4()",  bufsize_text, time4);
#if USE_BUFFER_VERIFY
        printf("\n");
        printf("verify_ok1 = %d, verify_ok2 = %d\n", verify_ok1 , verify_ok2);
#endif

        printf("\n");

        printf("%-16s size = %5s, time = %0.5f ms\n", "::memcpy()",     bufsize_text, time5 / (double)loop_times);
        printf("%-16s size = %5s, time = %0.5f ms\n", "A_memcpy()",     bufsize_text, time6 / (double)loop_times);
        printf("%-16s size = %5s, time = %0.5f ms\n", "fast_memcpy()",  bufsize_text, time7 / (double)loop_times);
        printf("%-16s size = %5s, time = %0.5f ms\n", "memcpy_mmx4()",  bufsize_text, time8 / (double)loop_times);

        printf("\n");
    }
#else

    buf_size = 16 * 1024;

    sw.restart();
    fast_memcpy(buffer1, buffer2, buf_size);
    sw.stop();
    time1 = sw.getMillisec();

    sw.restart();
    memcpy_mmx4(buffer3, buffer4, buf_size);
    sw.stop();
    time2 = sw.getMillisec();

    sw.restart();
    A_memcpy(buffer5, buffer6, buf_size);
    sw.stop();
    time3 = sw.getMillisec();

    sw.restart();
    ::memcpy(buffer7, buffer8, buf_size);
    sw.stop();
    time4 = sw.getMillisec();

    printf("%-16s size = %d K, time = %0.5f ms\n", "fast_memcpy()",   buf_size / 1024, time1);
    printf("%-16s size = %d K, time = %0.5f ms\n", "memcpy_mmx4()",   buf_size / 1024, time2);
    printf("%-16s size = %d K, time = %0.5f ms\n", "A_memcpy()",      buf_size / 1024, time3);
    printf("%-16s size = %d K, time = %0.5f ms\n", "::memcpy()",      buf_size / 1024, time4);

    printf("\n");

    buf_size = 64 * 1024;

    sw.restart();
    fast_memcpy(buffer1, buffer2, buf_size);
    sw.stop();
    time1 = sw.getMillisec();

    sw.restart();
    memcpy_mmx4(buffer3, buffer4, buf_size);
    sw.stop();
    time2 = sw.getMillisec();

    sw.restart();
    A_memcpy(buffer5, buffer6, buf_size);
    sw.stop();
    time3 = sw.getMillisec();

    sw.restart();
    ::memcpy(buffer7, buffer8, buf_size);
    sw.stop();
    time4 = sw.getMillisec();

    printf("%-16s size = %d K, time = %0.5f ms\n", "fast_memcpy()",   buf_size / 1024, time1);
    printf("%-16s size = %d K, time = %0.5f ms\n", "memcpy_mmx4()",   buf_size / 1024, time2);
    printf("%-16s size = %d K, time = %0.5f ms\n", "A_memcpy()",      buf_size / 1024, time3);
    printf("%-16s size = %d K, time = %0.5f ms\n", "::memcpy()",      buf_size / 1024, time4);

    printf("\n");

    buf_size = 16 * 1024 * 1024;

    sw.restart();
    fast_memcpy(buffer1, buffer2, buf_size);
    sw.stop();
    time1 = sw.getMillisec();

    sw.restart();
    memcpy_mmx4(buffer3, buffer4, buf_size);
    sw.stop();
    time2 = sw.getMillisec();

    sw.restart();
    A_memcpy(buffer5, buffer6, buf_size);
    sw.stop();
    time3 = sw.getMillisec();

    sw.restart();
    ::memcpy(buffer7, buffer8, buf_size);
    sw.stop();
    time4 = sw.getMillisec();

    printf("%-16s size = %d M, time = %0.5f ms\n", "fast_memcpy()",   buf_size / (1024 * 1024), time1);
    printf("%-16s size = %d M, time = %0.5f ms\n", "memcpy_mmx4()",   buf_size / (1024 * 1024), time2);
    printf("%-16s size = %d M, time = %0.5f ms\n", "A_memcpy()",      buf_size / (1024 * 1024), time3);
    printf("%-16s size = %d M, time = %0.5f ms\n", "::memcpy()",      buf_size / (1024 * 1024), time4);

    printf("\n");

    buf_size = BUFFER_SIZE;

    sw.restart();
    fast_memcpy(buffer1, buffer2, buf_size);
    sw.stop();
    time1 = sw.getMillisec();

    sw.restart();
    memcpy_mmx4(buffer3, buffer4, buf_size);
    sw.stop();
    time2 = sw.getMillisec();

    sw.restart();
    A_memcpy(buffer5, buffer6, buf_size);
    sw.stop();
    time3 = sw.getMillisec();

    sw.restart();
    ::memcpy(buffer7, buffer8, buf_size);
    sw.stop();
    time4 = sw.getMillisec();

    printf("%-16s size = %d M, time = %0.5f ms\n", "fast_memcpy()",   buf_size / (1024 * 1024), time1);
    printf("%-16s size = %d M, time = %0.5f ms\n", "memcpy_mmx4()",   buf_size / (1024 * 1024), time2);
    printf("%-16s size = %d M, time = %0.5f ms\n", "A_memcpy()",      buf_size / (1024 * 1024), time3);
    printf("%-16s size = %d M, time = %0.5f ms\n", "::memcpy()",      buf_size / (1024 * 1024), time4);

    printf("\n");
#endif

    if (buffer1) _aligned_free(buffer1);
    if (buffer2) _aligned_free(buffer2);
    if (buffer3) _aligned_free(buffer3);
    if (buffer4) _aligned_free(buffer4);

    if (buffer5) _aligned_free(buffer5);
    if (buffer6) _aligned_free(buffer6);
    if (buffer7) _aligned_free(buffer7);
    if (buffer8) _aligned_free(buffer8);
}

/* char_traits<T>相关字符串函数的测试 */

void Char_Traits_Test()
{
#ifndef _DEBUG
    const int LOOP_TIMES = 1000000;
#else
    const int LOOP_TIMES = 50000;
#endif
    double time1, time2, time3;
    jimi::stop_watch sw;
    int i, j;

    char *pstr1, *pstr2, *pstr3, *pstr4, *pstr;
    int str_len;

    pstr1 = (char *)::malloc(256 * sizeof(char));
    pstr2 = (char *)::malloc(256 * sizeof(char));
    pstr3 = (char *)::malloc(256 * sizeof(char));
    pstr4 = (char *)::malloc(256 * sizeof(char));

    pstr = ::jm_strcpy(pstr1 , 256, "test char * : pstr1");
    pstr = ::jm_strcpy(pstr2 , 256, "test char * : pstr2");
    pstr = ::jm_strcpy(pstr3 , 256, "");
    pstr = ::jm_strcpy(pstr4 , 256, "");

    str_len = ::jm_strlen(pstr1);

    for (j = 0; j < 4; ++j) {
        if (j == 1) {
            pstr1 = ::jm_strcat(pstr1 , 256, "test char * : pstr1 ");
            pstr1 = ::jm_strcat(pstr1 , 256, "test char * : pstr1 ");
            str_len = ::jm_strlen(pstr1);
        }
        else if (j > 1) {
            pstr1 = ::jm_strcat(pstr1 , 256, "test char * : pstr1 ");
            pstr1 = ::jm_strcat(pstr1 , 256, "test char * : pstr1 ");
            pstr1 = ::jm_strcat(pstr1 , 256, "test char * : pstr1 ");
            pstr1 = ::jm_strcat(pstr1 , 256, "test char * : pstr1 ");
            str_len = ::jm_strlen(pstr1);
        }
        sw.restart();
        for (i = 0; i < LOOP_TIMES; ++i) {
            pstr = ::strncpy(pstr2, pstr1, str_len);
            pstr2[str_len] = '\0';
        }
        sw.stop();
        time1 = sw.getMillisec();

        sw.restart();
        for (i = 0; i < LOOP_TIMES; ++i) {
            pstr = jimi::char_traits<char>::strncpy_unsafe(pstr3, pstr1, str_len);
            pstr3[str_len] = '\0';
        }
        sw.stop();
        time2 = sw.getMillisec();

        sw.restart();
        for (i = 0; i < LOOP_TIMES; ++i) {
            pstr = jimi::char_traits<char>::strncpy_unsafe2(pstr4, pstr1, str_len);
            pstr4[str_len] = '\0';
        }
        sw.stop();
        time3 = sw.getMillisec();

        printf("pstr1 = %s\n", pstr1);
        //printf("pstr2 = %s\n", pstr2);
        //printf("pstr3 = %s\n", pstr3);
        //printf("pstr3 = %s\n", pstr4);

        printf("\n");

        printf("::strlen(pstr1)                       = %d bytes\n", str_len);
        printf("::strncpy(),                     time = %0.3f ms\n", time1);
        printf("char_traits<char>::strncpy_u(),  time = %0.3f ms\n", time2);
        printf("char_traits<char>::strncpy_u2(), time = %0.5f ms\n", time3);

        printf("\n");
    }

    if (pstr1) free(pstr1);
    if (pstr2) free(pstr2);
    if (pstr3) free(pstr3);
    if (pstr4) free(pstr4);
}

void win_iconv_ansi2unicode_test(const char *szTest, bool echo_result)
{
    jimi::stop_watch sw;
#ifdef _DEBUG
    const int loop_times = 2000;
#else
    const int loop_times = 200000;
#endif
    int i;
    wchar_t *unicode_str, *unicode_str2;
    char *ansi_str, *utf8_str;
    int ansi_size, utf8_size, unicode_size, unicode_size2;
    double time1, time2, time3, time4;

    setlocale(LC_ALL, "chs");

    printf("repeat times: %d 次\n\n", loop_times);

    std::string source = szTest;
    std::string to_encoding, from_encoding;
    std::wstring w_string;
    to_encoding = "utf8";
    from_encoding = "gbk";

    utf8_size = iconv_ansi_to_utf8_auto(source.c_str(), source.size(), &utf8_str);
    if (utf8_str != NULL && utf8_size >= 0) {
        utf8_str[utf8_size] = '\0';
    }

#if defined(JIMI_HAS_BOOST_LOCALE) && (JIMI_HAS_BOOST_LOCALE != 0)

    if (echo_result)
        printf("boost::locale::conv::to_utf()\n\n");

    w_string = boost::locale::conv::to_utf<wchar_t>(source, "gbk");
    sw.restart();
    for (i = 0; i < loop_times; ++i) {
        //std::string ssss = boost::locale::conv::between(source, to_encoding, from_encoding);
        //utf8_string = boost::locale::conv::to_utf<char>(source, "gbk");
        //w_string = boost::locale::conv::utf_to_utf<wchar_t>(utf8_string);
        w_string = boost::locale::conv::to_utf<wchar_t>(source, "gbk");
    }
    sw.stop();
    time1 = sw.getElapsedTime();

    if (echo_result) {
        printf("ansi str = \n%s\n\n", source.c_str());
        wprintf(L"unicode str = \n%s\n\n", w_string.c_str());
        printf("\n");
    }

#else
    time1 = 1.0;
#endif  /* JIMI_HAS_BOOST_LOCALE */

    ansi_size = jm_strlen(szTest) + 1;
    ansi_str = (char *)malloc(ansi_size * sizeof(char));
    jm_strcpy(ansi_str, ansi_size, szTest);
    ansi_size = jm_strlen(ansi_str);
    unicode_size = iconv_ansi_to_unicode_auto(ansi_str, ansi_size, &unicode_str);
#if 0
    if (unicode_str)
        iconv_free(unicode_str);
#endif

    if (echo_result)
        printf("iconv_ansi_to_unicode()\n\n");

    sw.restart();
    for (i = 0; i < loop_times; ++i) {
        //unicode_size = iconv_ansi_to_unicode_auto(ansi_str, ansi_size, &unicode_str);
        unicode_size = iconv_ansi_to_unicode(ansi_str, ansi_size, unicode_str, unicode_size + 1);
        //unicode_size = iconv_utf8_to_unicode_auto(utf8_str, utf8_size, &unicode_str);
        /*
        if (unicode_size >= 0) {
            unicode_str[unicode_size] = '\0';
        }
        //*/
    }
    sw.stop();
    time2 = sw.getElapsedTime();

    if (echo_result) {
        printf("ansi str = \n%s\n\n", ansi_str);
        wprintf(L"unicode str = \n%s\n\n", unicode_str);
        printf("\n");
    }

    ansi_size = jm_strlen(szTest) + 1;
    jm_strcpy(ansi_str, ansi_size, szTest);
    ansi_size = jm_strlen(ansi_str);
    unicode_size2 = iconv_ansi_to_unicode_auto(ansi_str, ansi_size, &unicode_str2);

    if (echo_result)
        printf("mbstowcs()\n\n");

    sw.restart();
    for (i = 0; i < loop_times; ++i) {
        unicode_size2 = mbstowcs(unicode_str2, ansi_str, ansi_size);
    }
    sw.stop();
    time3 = sw.getElapsedTime();

    if (echo_result) {
        printf("ansi str = \n%s\n\n", ansi_str);
        wprintf(L"unicode str = \n%s\n\n", unicode_str2);
        printf("\n");
    }

    if (echo_result)
        printf("mbstowcs_s()\n\n");

    errno_t err;
    sw.restart();
    for (i = 0; i < loop_times; ++i) {
        err = mbstowcs_s((size_t *)&unicode_size2, unicode_str2, unicode_size + 1, ansi_str, ansi_size * sizeof(char));
    }
    sw.stop();
    time4 = sw.getElapsedTime();

    if (echo_result) {
        printf("ansi str = \n%s\n\n", ansi_str);
        wprintf(L"unicode str = \n%s\n\n", unicode_str2);
        printf("\n");
    }

    printf("test ansi ==> unicode,          size = %d bytes\n\n", jm_strlen(szTest));

    printf("boost::locale::conv::to_utf()   time = %0.3f ms\n", time1);
    printf("iconv_ansi_to_unicode()         time = %0.3f ms\n", time2);
    printf("mbstowcs()                      time = %0.3f ms\n", time3);
    printf("mbstowcs_s()                    time = %0.3f ms\n", time4);
    printf("\n");

    if (ansi_str)
        free(ansi_str);

    if (utf8_str)
        iconv_free(utf8_str);

    if (unicode_str)
        iconv_free(unicode_str);

    if (unicode_str2)
        iconv_free(unicode_str2);
}

void win_iconv_unicode2ansi_test(const wchar_t *szTest, bool echo_result)
{
    jimi::stop_watch sw;
#ifdef _DEBUG
    const int loop_times = 2000;
#else
    const int loop_times = 200000;
#endif
    int i;
    wchar_t *unicode_str;
    char *ansi_str, *utf8_str, *ansi_str2;
    int ansi_size, utf8_size, unicode_size, ansi_size2;
    double time1, time2, time3, time4;

    setlocale(LC_ALL, "chs");

    printf("repeat times: %d 次\n\n", loop_times);

    std::wstring source = szTest;
    std::string to_encoding, from_encoding;
    std::string a_string;
    to_encoding = "utf8";
    from_encoding = "gbk";

    utf8_size = iconv_unicode_to_utf8_auto(source.c_str(), source.size(), &utf8_str);
    if (utf8_str != NULL && utf8_size >= 0) {
        utf8_str[utf8_size] = '\0';
    }

#if defined(JIMI_HAS_BOOST_LOCALE) && (JIMI_HAS_BOOST_LOCALE != 0)

    if (echo_result)
        printf("boost::locale::conv::from_utf()\n\n");

    a_string = boost::locale::conv::from_utf<wchar_t>(source, from_encoding);
    sw.restart();
    for (i = 0; i < loop_times; ++i) {
        //std::string ssss = boost::locale::conv::between(source, to_encoding, from_encoding);
        //utf8_string = boost::locale::conv::to_utf<char>(source, "gbk");
        //w_string = boost::locale::conv::utf_to_utf<wchar_t>(utf8_string);
        a_string = boost::locale::conv::from_utf<wchar_t>(source, from_encoding);
    }
    sw.stop();
    time1 = sw.getElapsedTime();

    if (echo_result) {
        wprintf(L"unicode str = \n%s\n\n", source.c_str());
        printf("ansi str = \n%s\n\n", a_string.c_str());
        printf("\n");
    }

#else
    time1 = 1.0;
#endif  /* JIMI_HAS_BOOST_LOCALE */

    unicode_size = jm_wcslen(szTest) + 1;
    unicode_str = (wchar_t *)malloc(unicode_size * sizeof(wchar_t));
    jm_wcscpy(unicode_str, unicode_size, szTest);
    unicode_size = jm_wcslen(unicode_str);
    ansi_size = iconv_unicode_to_ansi_auto(unicode_str, unicode_size, &ansi_str);
#if 0
    if (unicode_str)
        iconv_free(unicode_str);
#endif

    if (echo_result)
        printf("iconv_unicode_to_ansi()\n\n");

    sw.restart();
    for (i = 0; i < loop_times; ++i) {
        ansi_size2 = iconv_unicode_to_ansi(unicode_str, unicode_size, ansi_str, ansi_size + 1);
    }
    sw.stop();
    time2 = sw.getElapsedTime();

    if (echo_result) {
        wprintf(L"unicode str = \n%s\n\n", unicode_str);
        printf("ansi str = \n%s\n\n", ansi_str);
        printf("\n");
    }

    if (unicode_str)
        free(unicode_str);

    unicode_size = jm_wcslen(szTest) + 1;
    unicode_str = (wchar_t *)malloc(unicode_size * sizeof(wchar_t));
    jm_wcscpy(unicode_str, unicode_size, szTest);
    unicode_size = jm_wcslen(unicode_str);
    ansi_size2 = iconv_unicode_to_ansi_auto(unicode_str, unicode_size, &ansi_str2);

    if (echo_result)
        printf("wcstombs()\n\n");

    sw.restart();
    for (i = 0; i < loop_times; ++i) {
        ansi_size2 = wcstombs(ansi_str2, unicode_str, unicode_size);
    }
    sw.stop();
    time3 = sw.getElapsedTime();

    if (echo_result) {
        wprintf(L"unicode str = \n%s\n\n", unicode_str);
        printf("ansi str = \n%s\n\n", ansi_str2);
        printf("\n");
    }

    if (echo_result)
        printf("wcstombs_s()\n\n");

#if 0
    if (ansi_str2)
        iconv_free(ansi_str2);

    ansi_size = ansi_size + 32;
    //ansi_str2 = (char *)malloc(ansi_size * sizeof(char));
    ansi_str2 = iconv_malloc(ansi_size, char);
    ansi_size--;
#endif

    //setlocale(LC_CTYPE, 0);

    errno_t err;
    sw.restart();
    for (i = 0; i < loop_times; ++i) {
        err = wcstombs_s((size_t *)&ansi_size2, ansi_str2, ansi_size + 1, unicode_str, unicode_size * sizeof(wchar_t));
    }
    sw.stop();
    time4 = sw.getElapsedTime();

    if (echo_result) {
        wprintf(L"unicode str = \n%s\n\n", unicode_str);
        printf("ansi str = \n%s\n\n", ansi_str2);
        printf("\n");
    }

    printf("test unicode ==> ansi,          size = %d words\n\n", jm_wcslen(szTest));

    printf("boost::locale::conv::from_utf() time = %0.3f ms\n", time1);
    printf("iconv_unicode_to_ansi()         time = %0.3f ms\n", time2);
    printf("wcstombs()                      time = %0.3f ms\n", time3);
    printf("wcstombs_s()                    time = %0.3f ms\n", time4);
    printf("\n");

    if (ansi_str)
        iconv_free(ansi_str);

    if (ansi_str2)
        iconv_free(ansi_str2);

    if (utf8_str)
        iconv_free(utf8_str);

    if (unicode_str)
        free(unicode_str);
}

bool win_iconv_test()
{
    bool echo_result = false;

#if !defined(JIMI_MSVC_CLANG) || (JIMI_MSVC_CLANG == 0)

    char *szAnsi_Test1 = "23123213我们都是中国人, 我们都是地球人fkjadhhfkljdhfkljdhfkldhklfhdkjafhkldjhfkjadhfkjhdakjfhdkajhfkldash"
        "英雄所见略同, 不经历风雨怎么见彩虹???jfkljdl4348972949384iyjdhfjdgjfg3h4eih3k19283变形金刚4, 变形金刚四⑦⑦";
    wchar_t *szUtf16_Test1 = L"23123213我们都是中国人, 我们都是地球人fkjadhhfkljdhfkljdhfkldhklfhdkjafhkldjhfkjadhfkjhdakjfhdkajhfkldash"
        L"英雄所见略同, 不经历风雨怎么见彩虹???jfkljdl4348972949384iyjdhfjdgjfg3h4eih3k19283变形金刚4, 变形金刚四⑦⑦";

    printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\n");

    win_iconv_ansi2unicode_test(szAnsi_Test1, echo_result);
    //Console.ReadKey();

    win_iconv_unicode2ansi_test(szUtf16_Test1, echo_result);
    //Console.ReadKey();

    char *szAnsi_Test2 = "我们都是中国人, 我们都是地球人";
    wchar_t *szUtf16_Test2 = L"我们都是中国人, 我们都是地球人";

    printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\n");

    win_iconv_ansi2unicode_test(szAnsi_Test2, echo_result);
    //Console.ReadKey();

    win_iconv_unicode2ansi_test(szUtf16_Test2, echo_result);

    printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\n");

#endif
    return true;
}

void Emplace_Test()
{
#if defined(JIMI_HAS_CXX11_VARIADIC_TEMPLATES) && (JIMI_HAS_CXX11_VARIADIC_TEMPLATES != 0)

    std::map<std::string, std::string> m;
 
    // uses pair's move constructor (overload 8)
    m.emplace(std::make_pair(std::string("a"), std::string("a")));
 
    // uses pair's converting move constructor (overload 5)
    m.emplace(std::make_pair("b", "abcd"));
 
    // uses pair's template constructor (overload 3)
    //m.emplace(std::pair<std::string, std::string>("d", "ddd"));
    m.emplace("d", "ddd");
 
    // uses pair's piecewise constructor (overload 6)
    m.emplace(std::piecewise_construct,
              std::forward_as_tuple("c"),
              std::forward_as_tuple(10, 'c'));
 
    for (const auto &p : m) {
        std::cout << p.first << " => " << p.second << '\n';
    }
    std::cout << std::endl;

#endif  /* JIMI_HAS_CXX11_VARIADIC_TEMPLATES */
}

void malloc_addr_test()
{
    int i;
    void *p;
    size_t size;
    void *list[256];
    srand((unsigned)time(NULL));
    for (i = 0; i < 256; ++i) {
        size = rand();
        p = ::malloc(size);
        list[i] = p;
        printf("p = 0x%08X, size = %d\n", p, size);
    }

    for (i = 0; i < 256; ++i) {
        if (list[i]) {
            free(list[i]);
            list[i] = NULL;
        }
    }

    printf("\n");
}

float __binary32_add_binary64_binary64(double a, double b)
{
    return (float)(a + b);
}

void IEEE754_Double_Test()
{
    double a64, b64;
    float c32;
    a64 = 1.000000059604644775390625;
    b64 = 1.1102230246251565404236316680908203125e-16;
    c32 = __binary32_add_binary64_binary64(a64, b64);
    printf("The addition result using the libary: %8.8f\n", c32);
    c32 = (float)(a64 + b64);
    printf("The addition result without the libary: %8.8f\n", c32);
    printf("\n");

    jimi::Console.ReadKey(true, false);
}

class resA
{
public:
    resA()  { std::cout << "resA::ctor()"  << std::endl; }
    ~resA() { std::cout << "resA::~dtor()" << std::endl; }
};

class resB
{
public:
    resB()  { std::cout << "resB::ctor()"  << std::endl; }
    ~resB() { std::cout << "resB::~dtor()" << std::endl; }
};

class AA
{
public:
    AA() : resA(), valueA(1) { std::cout << "AA::ctor()" << std::endl; }
    virtual ~AA() {
        std::cout << "AA::~dtor()" << std::endl;
    }

    void dump() {
        ReleaseUtils::dump(this, sizeof(*this));
    }

public:
    resA resA;
    int valueA;
};

class BB : public AA
{
public:
    BB() : resB(), valueB(2) { std::cout << "BB::ctor()" << std::endl; }
    virtual ~BB() {
        std::cout << "BB::~dtor()" << std::endl;
    }

    void dump() {
        ReleaseUtils::dump(this, sizeof(*this));
    }

public:
    resB resB;
    int valueB;
};

template<class T>
class A
{
public:
    A()  { std::cout << "A::ctor()" << std::endl; }
    ~A() {
        std::cout << "A::~dtor()" << std::endl;
        Destroy(true);
    }
    void Destroy(bool isDestructor = false) {
        T* pThis = static_cast<T *>(this);
        if (pThis && !pThis->hasDisposed()) {
            pThis->setDisposed();
            if (isDestructor)
                pThis->~T();
            else
                delete pThis;
        }
    }
private:
    resA resA;
};

class B : public A<B>
{
public:
    B() : hasDisposed_(false) { std::cout << "B::ctor()" << std::endl; }
    ~B() { std::cout << "B::~dtor()" << std::endl; }
    bool hasDisposed() { return hasDisposed_; };
    void setDisposed() { hasDisposed_ = true; };

private:
    bool hasDisposed_;
    resB resB;
};

void template_inherit_test()
{
    A<B>* a = new B();
    char *p = (char *)a;
    //*(p + sizeof(A<B>) + 3) = 0;
    a->Destroy();
    //delete a;
}

#if !defined(_MSC_VER) || (_MSC_VER >= 1600)

class Widget
{
public:
    Widget(void) : data_(0)
                    { std::cout << "ctor" << std::endl; }
    Widget(unsigned int data) : data_(data)
                    { std::cout << "ctor" << std::endl; }
    Widget(const Widget &src) : data_(src.getData())
                    { std::cout << "copy assignment" << std::endl; }
    ~Widget()       { std::cout << "~dtor, data = " << getData() << std::endl; }

    unsigned int getData() const { return data_; }
    void setData(unsigned int data) { data_ = data; }
    
    void f()        { std::cout << "Widget::f(),  data = " << getData() << std::endl; }
    void cf() const { std::cout << "Widget::cf(), data = " << getData() << std::endl; }

private:
    unsigned int data_;
};

Widget func(const Widget & w)
{
    std::cout << "func(): data = " << w.getData() << std::endl;
    return w;
}

Widget && func_rvalue(const Widget & w)
{
    std::cout << "func_rvalue(): data = " << w.getData() << std::endl;
    return std::move(const_cast<Widget &>(w));
    //return std::forward<Widget>(const_cast<Widget &>(w));
}

template <class T, size_t N>
class ObjectArray
{
public:
    typedef T (&ArrayType)[N];

    ObjectArray()  {}
    ~ObjectArray() {}

    ArrayType getArray() { return array; }

private:
    T array[N];
};

typedef Widget (&WidgetArray8)[8];

WidgetArray8 getWidgetArray(const WidgetArray8 &array)
{
    return array;
}

void auto_rvalue_test()
{
    std::cout << "----------- start ------------" << std::endl;

    //auto && p = func_rvalue(Widget(100));
    auto && p = func(Widget(100));
    //Widget p = func(Widget(100));
    p.setData(199);
    p.f();
    p.cf();

    Widget widgetArray[8] = { 0 };
    WidgetArray8 array8 = getWidgetArray(widgetArray);

    typedef ObjectArray<Widget, 8>::ArrayType ObjectArray8;
    ObjectArray<Widget, 8> objArray;
    ObjectArray8 array8b = objArray.getArray();

    std::array<Widget, 8> a;

    std::cout << "------------  end  -----------" << std::endl;
}

#endif  /* !defined(_MSC_VER) || (_MSC_VER >= 1600) */

/* !!模板成员函数不能定义为虚函数 */

template <typename T, typename _CharType>
class WidgetBase
{
public:
    WidgetBase()  { printf("WidgetBase::ctor()\n");  }
    ~WidgetBase() { printf("WidgetBase::~dtor()\n"); }

    virtual void print(const char *message) = 0;

#if 0
    /* 这样不行 */
    template <typename _CharType>
    virtual void print_info(const _CharType * info) {
        printf("WidgetBase::print_info(): info = %s\n", info);
    }
#else
    /* 这样就可以了 */

    // 这么定义是可以的
    virtual void print_info(const _CharType * info) {
        printf("WidgetBase::print_info(): info = %S\n", info);
    };
    // 下面这么定义是不行的, 必须实例化
    //virtual void print_info(const _CharType * info) = 0;
    /*
    // 这么定义是可以的
    virtual void print_info(const _CharType * info) {
        printf("WidgetBase::print_info(): info = %s\n", info);
    }
    //*/
#endif
};

class WidgetSample : public WidgetBase<WidgetSample, wchar_t>
{
public:
    WidgetSample()  { printf("WidgetSample::ctor()\n");  }
    ~WidgetSample() { printf("WidgetSample::~dtor()\n"); }

    virtual void print(const char *message) {
        printf("WidgetSample::print(): message = %s\n", message);
    };

#if 1
#if 0
    /* 这样不行 */
    template <typename _CharType>
    virtual void print_info(const _CharType * info) {
        printf("WidgetSample::print_info(): info = %s\n", info);
    }
#else
    /* 这样就可以了 */
    virtual void print_info(const char * info) {
        printf("WidgetSample::print_info(): info = %s\n", info);
    }

    virtual void print_info(const wchar_t * info) {
        printf("WidgetSample::print_info(): info = %S\n", info);
    }
#endif
#endif
};

void WidgetSampleTest()
{
    WidgetSample widgetSample;
    widgetSample.print("This is WidgetSample test.");
    widgetSample.print_info("This is WidgetSample char test.");
    widgetSample.print_info(L"This is WidgetSample wchar_t test.");
}

#include "jimic/math/log10.h"

void Log10_Test1(double val)
{
    int exp10;

    printf("==============================================================================\n\n");
    printf("  Log10_Test1()\n\n");
    printf("==============================================================================\n\n");

    //val = 1E+308;
    printf("val = %0.12E\n\n", val);

    exp10 = ieee754_log10_crt_1(val);
    printf("log10_crt_1(val)        = %d.\n", exp10);

    exp10 = ieee754_log10_crt_2(val);
    printf("log10_crt_2(val)        = %d.\n", exp10);

    exp10 = jmc_log10_fast_v0(val);
    printf("jmc_log10_fast_v0(val)  = %d.\n", exp10);

    exp10 = jmc_log10_fast_v1(val);
    printf("jmc_log10_fast_v1(val)  = %d.\n", exp10);

    exp10 = jmc_log10_fast_v2(val);
    printf("jmc_log10_fast_v2(val)  = %d.\n", exp10);

    exp10 = jmc_log10_fast(val);
    printf("jmc_log10_fast(val)     = %d.\n", exp10);

    printf("\n");
}

void Log10_Test2()
{
#ifndef _DEBUG
    const int LOOP_TIMES = 1000000;
#else
    const int LOOP_TIMES = 2000;
#endif
    double time1, time2, time3, time4, time5, time6;
    double val;
    int exp10;
    int i, loop_times = 0;
    jimi::stop_watch sw;

    printf("==============================================================================\n\n");
    printf("  Log10_Test2()\n\n");
    printf("==============================================================================\n\n");

    val = 1000000000.0;
    exp10 = 0;

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        exp10 += ieee754_log10_crt_1(val);
        val += 100.0;
    }
    sw.stop();
    time1 = sw.getMillisec();
    printf("ieee754_log10_crt_1():  time = %0.6f ms, exp10 = %d.\n\n", time1, exp10);

    val = 1000000000.0;
    exp10 = 0;

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        exp10 += ieee754_log10_crt_2(val);
        val += 100.0;
    }
    sw.stop();
    time2 = sw.getMillisec();
    printf("ieee754_log10_crt_2():  time = %0.6f ms, exp10 = %d.\n\n", time2, exp10);

    // ieee754_int_log10_x87()
    val = 1000000000.0;
    exp10 = 0;

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        exp10 += ieee754_int_log10_x87(val);
        val += 100.0;
    }
    sw.stop();
    time2 = sw.getMillisec();
    printf("ieee754_log10_x87():    time = %0.6f ms, exp10 = %d.\n\n", time2, exp10);

    val = 1000000000.0;
    exp10 = 0;

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        exp10 += jmc_log10_fast_v0(val);
        val += 100.0;
    }
    sw.stop();
    time3 = sw.getMillisec();
    printf("jmc_log10_fast_v0():    time = %0.6f ms, exp10 = %d.\n\n", time3, exp10);

    val = 1000000000.0;
    exp10 = 0;

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        exp10 += jmc_log10_fast_v1(val);
        val += 100.0;
    }
    sw.stop();
    time4 = sw.getMillisec();
    printf("jmc_log10_fast_v1():    time = %0.6f ms, exp10 = %d.\n\n", time4, exp10);

    val = 1000000000.0;
    exp10 = 0;

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        exp10 += jmc_log10_fast_v2(val);
        val += 100.0;
    }
    sw.stop();
    time5 = sw.getMillisec();
    printf("jmc_log10_fast_v2():    time = %0.6f ms, exp10 = %d.\n\n", time5, exp10);

    val = 1000000000.0;
    exp10 = 0;

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        exp10 += jmc_log10_fast_v3(val);
        val += 100.0;
    }
    sw.stop();
    time6 = sw.getMillisec();
    printf("jmc_log10_fast_v3():    time = %0.6f ms, exp10 = %d.\n\n", time6, exp10);

    val = 1000000000.0;
    exp10 = 0;

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        exp10 += jmc_log10_fast(val);
        val += 100.0;
    }
    sw.stop();
    time6 = sw.getMillisec();
    printf("jmc_log10_fast():       time = %0.6f ms, exp10 = %d.\n\n", time6, exp10);

    val = 1000000000.0;
    exp10 = 0;

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        exp10 += jmc_log10_fast64(val);
        val += 100.0;
    }
    sw.stop();
    time6 = sw.getMillisec();
    printf("jmc_log10_fast64():     time = %0.6f ms, exp10 = %d.\n\n", time6, exp10);

    printf("\n");
}

void Log10_Test3()
{
    const int LOOP_TIMES = 128;
    double time1;
    double val;
    int exp10, sum;
    int i, loop_times = 0;
    jimi::stop_watch sw;

    printf("==============================================================================\n\n");
    printf("  Log10_Test3()\n\n");
    printf("==============================================================================\n\n");

    val = 1.0;
    sum = 0;

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        exp10 = jmc_log10_fast(val);
        printf("jmc_log10_fast(): val = %4d, exp10 = %4d.\n", (int)val, exp10);
        sum += exp10;
        val += 1.0;
    }
    sw.stop();
    time1 = sw.getMillisec();
    printf("jmc_log10_fast(): time = %0.6f ms, sum = %d.\n\n", time1, sum);

    printf("\n");
}

#include "jimic/math/int_log10.h"

void Int_Log10_Test()
{
#ifndef _DEBUG
    const int LOOP_TIMES = 1000000;
#else
    const int LOOP_TIMES = 2000;
#endif

#ifndef _WIN64
    double time1, time2, time3;
#else
    double time1, time2, time3, time4;
#endif
    unsigned int val;
    unsigned int exp10;
    int i, loop_times = 0;
    jimi::stop_watch sw;

    printf("==============================================================================\n\n");
    printf("  Int_Log10_Test()\n\n");
    printf("==============================================================================\n\n");

    exp10 = 0;

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        exp10 += jmc_uint_log10_sys(i);
    }
    sw.stop();
    time1 = sw.getMillisec();
    printf("jmc_uint_log10_sys():   time = %7.4f ms, exp10 = %d.\n\n", time1, exp10);

    exp10 = 0;

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        exp10 += jmc_uint_log10_comp(i);
    }
    sw.stop();
    time2 = sw.getMillisec();
    printf("jmc_uint_log10_comp():  time = %7.4f ms, exp10 = %d.\n\n", time2, exp10);

    exp10 = 0;

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        exp10 += jmc_uint_log10(i);
    }
    sw.stop();
    time3 = sw.getMillisec();
    printf("jmc_uint_log10():       time = %7.4f ms, exp10 = %d.\n\n", time3, exp10);

    exp10 = 0;

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        exp10 += jmc_uint_log10_v2(i);
    }
    sw.stop();
    time1 = sw.getMillisec();
    printf("jmc_uint_log10_v2():    time = %7.4f ms, exp10 = %d.\n\n", time1, exp10);

    exp10 = 0;

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        exp10 += jmc_uint_log10_v3(i);
    }
    sw.stop();
    time2 = sw.getMillisec();
    printf("jmc_uint_log10_v3():    time = %7.4f ms, exp10 = %d.\n\n", time2, exp10);

#ifdef _WIN64

    val = 1;
    exp10 = 0;

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        exp10 += jmc_uint64_log10(val);
        val += 1;
    }
    sw.stop();
    time4 = sw.getMillisec();
    printf("jmc_uint64_log10():     time = %7.4f ms, exp10 = %d.\n\n", time4, exp10);

#endif

    // special test (0-19)
    printf("special test (0-19):\n\n");
    
    exp10 = 0;

    sw.restart();
    for (i = 0; i < LOOP_TIMES / 20; ++i) {
        for (val = 0; val < 20; ++val) {
            exp10 += jmc_uint_log10(val);
        }
    }
    sw.stop();
    time1 = sw.getMillisec();
    printf("jmc_uint_log10():       time = %7.4f ms, exp10 = %d.\n\n", time1, exp10);

    exp10 = 0;

    sw.restart();
    for (i = 0; i < LOOP_TIMES / 20; ++i) {
        for (val = 0; val < 20; ++val) {
            exp10 += jmc_uint_log10_v2(val);
        }
    }
    sw.stop();
    time2 = sw.getMillisec();
    printf("jmc_uint_log10_v2():    time = %7.4f ms, exp10 = %d.\n\n", time2, exp10);

    exp10 = 0;

    sw.restart();
    for (i = 0; i < LOOP_TIMES / 20; ++i) {
        for (val = 0; val < 20; ++val) {
            exp10 += jmc_uint_log10_v3(val);
        }
    }
    sw.stop();
    time3 = sw.getMillisec();
    printf("jmc_uint_log10_v3():    time = %7.4f ms, exp10 = %d.\n\n", time3, exp10);

    // special test (0-99)
    printf("special test (0-99):\n\n");

    exp10 = 0;

    sw.restart();
    for (i = 0; i < LOOP_TIMES / 100; ++i) {
        for (val = 0; val < 100; ++val) {
            exp10 += jmc_uint_log10(val);
        }
    }
    sw.stop();
    time1 = sw.getMillisec();
    printf("jmc_uint_log10():       time = %7.4f ms, exp10 = %d.\n\n", time1, exp10);

    exp10 = 0;

    sw.restart();
    for (i = 0; i < LOOP_TIMES / 100; ++i) {
        for (val = 0; val < 100; ++val) {
            exp10 += jmc_uint_log10_v2(val);
        }
    }
    sw.stop();
    time2 = sw.getMillisec();
    printf("jmc_uint_log10_v2():    time = %7.4f ms, exp10 = %d.\n\n", time2, exp10);

    exp10 = 0;

    sw.restart();
    for (i = 0; i < LOOP_TIMES / 100; ++i) {
        for (val = 0; val < 100; ++val) {
            exp10 += jmc_uint_log10_v3(val);
        }
    }
    sw.stop();
    time3 = sw.getMillisec();
    printf("jmc_uint_log10_v3():    time = %7.4f ms, exp10 = %d.\n\n", time3, exp10);

    printf("\n");
}

void Int_Log10_TestNum(unsigned int val)
{
    unsigned int exp10;
    exp10 = jmc_uint_log10(val);
    printf("jmc_uint_log10(%u) \t= %u\n", val, exp10);
}

void Int_Log10_TestNums()
{
    ieee754_log10_x87(9.0);

    Int_Log10_TestNum(9);
    Int_Log10_TestNum(10);
    printf("\n");

    Int_Log10_TestNum(99);
    Int_Log10_TestNum(100);
    printf("\n");

    Int_Log10_TestNum(999);
    Int_Log10_TestNum(1000);
    printf("\n");

    Int_Log10_TestNum(99999999);
    Int_Log10_TestNum(100000000);
    printf("\n");

    Int_Log10_TestNum(999999999);
    Int_Log10_TestNum(1000000000);
    printf("\n");

    Int_Log10_TestNum(2147483648);
    Int_Log10_TestNum(4294967295);
    printf("\n");
}

#include "jimic/string/dtos.h"

void Double_And_Float_Test()
{
    char buf[128];
    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "double = %f\n", 12.345);
    printf("%s", buf);
    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "double = %8.3f\n", 12.345);
    printf("%s", buf);
    printf("\n");

    printf("--------------------------------------------\n\n");

    printf("printf():       int = %-012.3d,\n",  123456);
    printf("printf():       int = %-012.3d,\n", -123456);
    printf("printf():       int = %012.3d,\n",   123456);
    printf("printf():       int = %012.3d,\n",  -123456);
    printf("printf():       int = %-012.8d,\n",  123456);
    printf("printf():       int = %-012.8d,\n", -123456);
    printf("printf():       int = %012.8d,\n",   123456);
    printf("printf():       int = %012.8d,\n",  -123456);
    printf("\n");

    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "jmc_snprintf(): int = %-012.3d,\n",  123456);
    printf("%s", buf);
    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "jmc_snprintf(): int = %-012.3d,\n", -123456);
    printf("%s", buf);
    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "jmc_snprintf(): int = %012.3d,\n",   123456);
    printf("%s", buf);
    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "jmc_snprintf(): int = %012.3d,\n",  -123456);
    printf("%s", buf);
    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "jmc_snprintf(): int = %-012.8d,\n",  123456);
    printf("%s", buf);
    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "jmc_snprintf(): int = %-012.8d,\n", -123456);
    printf("%s", buf);
    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "jmc_snprintf(): int = %012.8d,\n",   123456);
    printf("%s", buf);
    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "jmc_snprintf(): int = %012.8d,\n",  -123456);
    printf("%s", buf);
    printf("\n");

    printf("--------------------------------------------\n\n");

    printf("printf():       int = %-12.3d,\n",  123456);
    printf("printf():       int = %-12.3d,\n", -123456);
    printf("printf():       int = %12.3d,\n",   123456);
    printf("printf():       int = %12.3d,\n",  -123456);
    printf("printf():       int = %-12.8d,\n",  123456);
    printf("printf():       int = %-12.8d,\n", -123456);
    printf("printf():       int = %12.8d,\n",   123456);
    printf("printf():       int = %12.8d,\n",  -123456);
    printf("\n");

    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "jmc_snprintf(): int = %-12.3d,\n",  123456);
    printf("%s", buf);
    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "jmc_snprintf(): int = %-12.3d,\n", -123456);
    printf("%s", buf);
    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "jmc_snprintf(): int = %12.3d,\n",   123456);
    printf("%s", buf);
    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "jmc_snprintf(): int = %12.3d,\n",  -123456);
    printf("%s", buf);
    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "jmc_snprintf(): int = %-12.8d,\n",  123456);
    printf("%s", buf);
    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "jmc_snprintf(): int = %-12.8d,\n", -123456);
    printf("%s", buf);
    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "jmc_snprintf(): int = %12.8d,\n",   123456);
    printf("%s", buf);
    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "jmc_snprintf(): int = %12.8d,\n",  -123456);
    printf("%s", buf);
    printf("\n");

    printf("--------------------------------------------\n\n");

#if defined(JIMI_HAS_CXX11_VARIADIC_TEMPLATES) && (JIMI_HAS_CXX11_VARIADIC_TEMPLATES != 0)

    #define NAN_DOUBLE          ((double)(INFINITY * 0.0F))
    #define DOUBLE_INFINITY     ((double)(_HUGE_ENUF * _HUGE_ENUF))

    printf("isnan(NAN)         = %d\n", isnan(NAN));
    printf("isnan(INFINITY)    = %d\n", isnan(INFINITY));
    printf("isnan(0.0)         = %d\n", isnan(0.0));
    printf("isnan(DBL_MIN/2.0) = %d\n", isnan(DBL_MIN/2.0));
    printf("isnan(1.0)         = %d\n", isnan(1.0));
    printf("isnan(NAN_DOUBLE)  = %d\n", isnan(NAN_DOUBLE));
    printf("\n");

    printf("isnan(NAN)         = %d\n", jmc_isnan_f(NAN));
    printf("isnan(INFINITY)    = %d\n", jmc_isnan_f(INFINITY));
    printf("isnan(0.0)         = %d\n", jmc_isnan_d(0.0));
    printf("isnan(DBL_MIN/2.0) = %d\n", jmc_isnan_d(DBL_MIN/2.0));
    printf("isnan(1.0)         = %d\n", jmc_isnan_d(1.0));
    printf("isnan(NAN_DOUBLE)  = %d\n", jmc_isnan_d(NAN_DOUBLE));
    printf("\n");

    printf("NAN         = %f\n", NAN);
    printf("NAN_DOUBLE  = %f\n", NAN_DOUBLE);
    printf("\n");

    printf("FP_NAN       = %d\n", FP_NAN);         // 0
    printf("FP_INFINITE  = %d\n", FP_INFINITE);    // 1
    printf("FP_ZERO      = %d\n", FP_ZERO);        // 2
    printf("FP_SUBNORMAL = %d\n", FP_SUBNORMAL);   // 3
    printf("FP_NORMAL    = %d\n", FP_NORMAL);      // 4
    printf("\n");
 
    printf("fpclassify(NAN)         = %d\n", fpclassify(NAN));           // 0
    printf("fpclassify(INFINITY)    = %d\n", fpclassify(INFINITY));      // 1
    printf("fpclassify(0.0)         = %d\n", fpclassify(0.0));           // 2
    printf("fpclassify(DBL_MIN/2.0) = %d\n", fpclassify(DBL_MIN/2.0));   // 3
    printf("fpclassify(1.0)         = %d\n", fpclassify(1.0));           // 4
    printf("\n");

    printf("jmc_ftest(NAN)          = %d\n", jmc_ftest(NAN));           // 0
    printf("jmc_ftest(INFINITY)     = %d\n", jmc_ftest(INFINITY));      // 1
    printf("jmc_dtest(NAN_DOUBLE)   = %d\n", jmc_dtest(NAN_DOUBLE));    // 0
    printf("jmc_dtest(0.0)          = %d\n", jmc_dtest(0.0));           // 2
    printf("jmc_dtest(DBL_MIN/2.0)  = %d\n", jmc_dtest(DBL_MIN/2.0));   // 3
    printf("jmc_dtest(1.0)          = %d\n", jmc_dtest(1.0));           // 4
    printf("\n");

    printf("printf(INFINITY)         = %f\n", INFINITY);
    printf("printf(-INFINITY)        = %f\n", -INFINITY);
    printf("printf(DOUBLE_INFINITY)  = %f\n", DOUBLE_INFINITY);
    printf("printf(-DOUBLE_INFINITY) = %f\n", -DOUBLE_INFINITY);
    //printf("printf(1.23456789E+200)  = %f\n", 1.2345678901234567890123456789E+200);
    printf("printf(1.23456789E+200)  = %f\n", 1.234567890123456789E+200);
    printf("\n");

#endif  /* JIMI_HAS_CXX11_VARIADIC_TEMPLATES */

    double d = 1.2345678901234567890123456789E+200;
    double dd = d;
    uint64_t u64 = (uint64_t)d;
    printf("printf(1.2345678901234567E+19) = %I64u\n", (uint64_t)1.234567890123456789E+19);
    jmc_dadjust_to_bin64((double *)&dd, jmc_dget_exponent((double *)&dd));
    printf("printf(1.23456789E+200)        = %0.20f\n", dd);

    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "jmc_snprintf(1.23456789E+200)  = %0.20f\n", d);
    printf("%s", buf);
    printf("\n");

    printf("printf(1.23456789E+0)          = %0.20f\n", (double)1.2345678901234567890123456789);

    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "jmc_snprintf(1.23456789E+0)    = %0.20f\n", (double)1.2345678901234567890123456789);
    printf("%s", buf);
    printf("\n");

    printf("printf(12.3456789E+0)          = %0.20f\n", (double)12.345678901234567890123456789);

    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "jmc_snprintf(12.3456789E+0)    = %0.20f\n", (double)12.345678901234567890123456789);
    printf("%s", buf);
    printf("\n");

    printf("printf(123.456789E+0)          = %0.20f\n", (double)123.45678901234567890123456789);

    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "jmc_snprintf(123.456789E+0)    = %0.20f\n", (double)123.45678901234567890123456789);
    printf("%s", buf);
    printf("\n");

    printf("printf(1234.56789E+0)          = %0.20f\n", (double)1234.5678901234567890123456789);

    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "jmc_snprintf(1234.56789E+0)    = %0.20f\n", (double)1234.5678901234567890123456789);
    printf("%s", buf);
    printf("\n");

    printf("printf(12345.6789E+0)          = %0.20f\n", (double)12345.678901234567890123456789);

    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "jmc_snprintf(12345.6789E+0)    = %0.20f\n", (double)12345.678901234567890123456789);
    printf("%s", buf);
    printf("\n");

#if defined(JIMI_HAS_CXX11_VARIADIC_TEMPLATES) && (JIMI_HAS_CXX11_VARIADIC_TEMPLATES != 0)

    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "jmc_snprintf(INFINITY)         = %f\n", INFINITY);
    printf("%s", buf);
    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "jmc_snprintf(-INFINITY)        = %f\n", -INFINITY);
    printf("%s", buf);
    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "jmc_snprintf(DOUBLE_INFINITY)  = %f\n", DOUBLE_INFINITY);
    printf("%s", buf);
    jmc_snprintf(buf, jm_countof(buf), jm_countof(buf) - 1, "jmc_snprintf(-DOUBLE_INFINITY) = %f\n", -DOUBLE_INFINITY);
    printf("%s", buf);
    printf("\n");

#endif  /* JIMI_HAS_CXX11_VARIADIC_TEMPLATES */
}

#include "jimi/lang/SmallString.h"

void SmallString_Test()
{
    jimi::SmallString<char, 120, 15> smallString1("small string test");
    jimi::SmallString<char, 122, 31> smallString2("small string test! small string test! small string test! "
                                                  "small string test! small string test! small string test! "
                                                  "small string test! small string test! small string test! "
                                                  "small string test! small string test! small string test! ");

    printf("smallString1<char , 128>.capacity()  = %u byte(s),\n", smallString1.capacity());
    printf("smallString1<char , 128>.alignment() = %u byte(s),\n", smallString1.alignment());
    printf("smallString1<char , 128>.data()      = 0x%p,\n", smallString1.data());
    printf("smallString1<char , 128>.size()      = %u byte(s),\n", smallString1.size());
    printf("smallString1<char , 128>.c_str()     = \"%s\".\n", smallString1.c_str());
    printf("\n");

    printf("smallString2<char , 128>.capacity()  = %u byte(s),\n", smallString2.capacity());
    printf("smallString2<char , 128>.alignment() = %u byte(s),\n", smallString2.alignment());
    printf("smallString2<char , 128>.data()      = 0x%p,\n", smallString2.data());
    printf("smallString2<char , 128>.size()      = %u byte(s),\n", smallString2.size());
    printf("smallString2<char , 128>.c_str()     = \"%s\".\n", smallString2.c_str());
    printf("\n");
}

#include "jimic/stdio/sprintf_lite.h"

void sprintf_lite_test()
{
    char outbuf[256];
    snprintf_lite(outbuf, jm_countof(outbuf), jm_countof(outbuf) - 1, "%d", 1234);
    printf("snprintf_lite(\"%%d\", %d) = %s\n", 1234, outbuf);

    snprintf_lite(outbuf, jm_countof(outbuf), jm_countof(outbuf) - 1, "%d", 23456);
    printf("snprintf_lite(\"%%d\", %d) = %s\n", 23456, outbuf);

    snprintf_lite(outbuf, jm_countof(outbuf), jm_countof(outbuf) - 1, "0x%X", 0x12ABCD);
    printf("snprintf_lite(\"0x%%X\", 0x%X) = %s\n", 0x12ABCD, outbuf);

    snprintf_lite(outbuf, jm_countof(outbuf), jm_countof(outbuf) - 1, "0%o", 023456);
    printf("snprintf_lite(\"0%%o\", 0%o) = %s\n", 023456, outbuf);

    snprintf_lite(outbuf, jm_countof(outbuf), jm_countof(outbuf) - 1, "#%t", 234567);
    printf("snprintf_lite(\"#%%t\", %d) = %s\n", 234567, outbuf);

    snprintf_lite(outbuf, jm_countof(outbuf), jm_countof(outbuf) - 1, "#%T", 1234567);
    printf("snprintf_lite(\"#%%T\", %d) = %s\n", 1234567, outbuf);

    snprintf_lite(outbuf, jm_countof(outbuf), jm_countof(outbuf) - 1, "#%K", 2345678);
    printf("snprintf_lite(\"#%%K\", %d) = %s\n", 2345678, outbuf);

    snprintf_lite(outbuf, jm_countof(outbuf), jm_countof(outbuf) - 1, "#%M", 3456789);
    printf("snprintf_lite(\"#%%M\", %d) = %s\n", 3456789, outbuf);
}

#include <memory>

template <typename T, typename RefType = std::shared_ptr<T> >
bool ReadData(DWORD dwBase, RefType &data)
{
    data = std::make_shared<T>(1);
    T *pData = data.get();
    size_t size = sizeof(T);

    return true;
}

template <typename T>
bool ReadData2(DWORD dwBase, std::shared_ptr<T> &data)
{
    data = std::make_shared<T>(1);
    T *pData = data.get();
    size_t size = sizeof(T);

    return true;
}

template <typename T = int>
bool ReadData3(DWORD dwBase, std::shared_ptr<T> &data)
{
    data = std::make_shared<T>(1);
    T *pData = data.get();
    size_t size = sizeof(T);

    return true;
}

template <typename T>
bool ReadData_Test(DWORD dwBase, std::shared_ptr<T> &data)
{
    data = std::make_shared<T>();
    T *pData = data.get();
    size_t size = sizeof(T);

    return true;
}

#include "jimic/libc/ieee754.h"

int UnitTest_Main(int argc, char *argv[])
{
    //using namespace jimi;
    //using namespace jimi::system;

    std::shared_ptr<int> data;
    ReadData<int>((DWORD)1, data);

    std::shared_ptr<int> data2;
    ReadData2<int>(1, data2);

    std::shared_ptr<int> data3;
    ReadData3<>(1, data3);

    std::shared_ptr<double> data_dbl;
    ReadData3<double>(1, data_dbl);

    std::shared_ptr<double> dataT = std::shared_ptr<double>();
    ReadData_Test(0, dataT);

    std::shared_ptr<int> dataT2;
    ReadData_Test(0, dataT2);

    int a = sizeof(jmc_ieee754_double);

    // 设置进程和线程的亲缘性
    jimi_set_thread_affinity(0);

    // 设置CRTDBG的环境(Debug模式下, 检查内存越界和内存泄漏问题)
    jimi_set_crtdbg_env(ALLOW_DISPLAY_MEMORY_LEAK, ALWAYS_CHECK_BOUNDS);

    jmLog.log_begin();

    jimi::CommandLine cmdLine;
    int cnt;
    if ((cnt = cmdLine.parse(argc, argv)) >= 0) {
        std::string args = cmdLine.getCmdLine();
        jmLog.info(args.c_str());
    }

    // Test System.out.info()
    jimi::System.out.info("System.out.info() test.\n");

    // Test System.out.print()
    jimi::System.out.print("System.out.print() test.\n");

    // Test System.out.println()
    jimi::System.out.println("System.out.println() test.\n");

#if 0
#if !defined(_MSC_VER) || (_MSC_VER >= 1600)
    if (true) {
        auto_rvalue_test();
        jmLog.log_end();
        jimi::Console.ReadKey();
        return 0;
    }
#endif
#endif

#if 0
    if (true) {
        template_inherit_test();
        jmLog.log_end();
        jimi::Console.ReadKey();
        return 0;
    }
#endif

    // CPU 唤醒/预热 500毫秒
    jimi_cpu_warmup(500);

#if 0
    WidgetSampleTest();
    printf("\n");

    jimi::Console.ReadKey_NewLine(true);
#endif

#if 0
    malloc_addr_test();

    jimi::Console.ReadKey_NewLine(true);
    return 0;
#endif

#if 0
    int cpuinfo[4] = { 0 };
    __cpuid(cpuinfo, 0x80000001);

    unsigned int n = 0;

    for (n = 0; n < 16; ++n) {
        printf("__lzcnt(%d)   = %d\n", n, __lzcnt(n));
        printf("__lzcnt16(%d) = %d\n", n, __lzcnt16(n));
        //printf("__lzcnt64(%d) = %d\n", n, __lzcnt64(n));
    }

    n = 0xffff;

    printf("__lzcnt(%d)   = %d\n", n, __lzcnt(n));
    printf("__lzcnt16(%d) = %d\n", n, __lzcnt16(n));
    //printf("__lzcnt64(%d) = %d\n", n, __lzcnt64(n));
    printf("\n");

    for (n = 0; n < 16; ++n) {
        printf("__builtin_clz(%5d) [BSR] = %d\n", n, __builtin_clz(n));
    }

    n = 0xff00;
    printf("__builtin_clz(%5d) [BSR] = %d\n", n, __builtin_clz(n));

    n = 0;
    printf("__builtin_clz(%5d) [BSR] = %d\n", n, __builtin_clz(n));
    printf("\n");

    for (n = 0; n < 16; ++n) {
        printf("__builtin_ctz(%5d) [BSF] = %d\n", n, __builtin_ctz(n));
    }

    n = 0xff00;
    printf("__builtin_ctz(%5d) [BSF] = %d\n", n, __builtin_ctz(n));

    n = 0;
    printf("__builtin_ctz(%5d) [BSF] = %d\n", n, __builtin_ctz(n));
    printf("\n");

    if (true && 1) {
        jmLog.log_end();
        jimi::Console.ReadKey();
        return 0;
    }
#endif

#if 0
    bool result = win_iconv_test();
    if (result && 0) {
        jmLog.log_end();
        jimi::Console.ReadKey();
        return 0;
    }
#endif

#if 0
    IEEE754_Double_Test();
#endif

#if 0
    Emplace_Test();
#endif

#if 1
    do {
        jimi::string str("This is a Test String.");
        str.dump();
        jimi::Console.WriteLine();

        str.getStorage().dump();
    } while (0);

    jimi::Console.WriteLine();

    do {
        AA aa;
        aa.dump();

        ReleaseUtils::dump((void *)(*(unsigned int *)(&aa)), 32);

        ReleaseUtils::dump((void *)(*(unsigned int *)(&aa.resA)), sizeof(aa.resA));
    } while (0);

    jimi::Console.WriteLine();

    do {
        BB bb;
        bb.dump();

        ReleaseUtils::dump((void *)(*(unsigned int *)(&bb)), 32);

        ReleaseUtils::dump((void *)(*(unsigned int *)(&bb.resB)), sizeof(bb.resB));
    } while (0);

    jimi::Console.WriteLine();

    if (true && 1) {
        jmLog.log_end();
        jimi::Console.ReadKey();
        return 0;
    }
#endif

#if 1
    sprintf_lite_test();
    printf("\n");

    if (true && 0) {
        jmLog.log_end();
        jimi::Console.ReadKey();
        return 0;
    }

    //jimi::Console.ReadKey_NewLine(true);
#endif

    // 测试std::string是否使用了COW(Copy On Write)
    //String_Copy_On_Write_Test();

    // Memcpy 内存复制测试
    //Memcpy_Test();

#if 0
    OStringStream_Test();

    if (true && 1) {
        jmLog.log_end();
        jimi::Console.ReadKey();
        return 0;
    }
#endif

#if 0
    String_Base_Test();
    if (true && 0) {
        jmLog.log_end();
        jimi::Console.ReadKey();
        return 0;
    }
#endif

#if 0
    String_Format_Test();
    if (true && 0) {
        jmLog.log_end();
        jimi::Console.ReadKey();
        return 0;
    }
#endif

#if 0
    ftol_test_main();
    dtol_test_main();
    if (true && 0) {
        sLog.log_end();
        jimi::Console.ReadKey();
        return 0;
    }
    jimi::Console.ReadKey();
#endif

#if 1
    Log10_Test1(1E+308);
    Log10_Test1(1E+200);
    Log10_Test1(1E+100);
    Log10_Test2();
    //Log10_Test3();

    jimi::Console.ReadKey_NewLine(true);

    Int_Log10_Test();
    Int_Log10_TestNums();

    jimi::Console.ReadKey_NewLine(true);

    Double_And_Float_Test();

    jimi::Console.ReadKey_NewLine(true);
#endif

#if 1
    SmallString_Test();
    
    if (true && 0) {
        jmLog.log_end();
        jimi::Console.ReadKey();
        return 0;
    }

    jimi::Console.ReadKey_NewLine(true);
#endif

#if 1
  #if !defined(VSNPRINTF_DISPLAY_TEST_RESULT) || (VSNPRINTF_DISPLAY_TEST_RESULT == 0)
    String_Snprintf_Test();

    jimi::Console.ReadKey_NewLine(true);
  #endif

  #if 1
    Sprintf_Preformance_Test_Integer1();

    Snprintf_Preformance_Test_Integer1();
    Snprintf_Preformance_Test_Integer2();
    Snprintf_Preformance_Test_Integer3();

    jimi::Console.ReadKey_NewLine(true);
  #endif

  #if 1
    Snprintf_Preformance_Test_Double1();
    Snprintf_Preformance_Test_Double2();
    Snprintf_Preformance_Test_Double3();
    Snprintf_Preformance_Test_Double4();

    jimi::Console.ReadKey_NewLine(true);
  #endif

  #if 1
    Snprintf_Preformance_Test_String1();
    Snprintf_Preformance_Test_String2();
    Snprintf_Preformance_Test_String3();
    Snprintf_Preformance_Test_String4();
    Snprintf_Preformance_Test_String5();

    jimi::Console.ReadKey_NewLine(true);
  #endif

    //OStringStream_Performance_Test();

    if (true && 0) {
        jmLog.log_end();
        jimi::Console.ReadKey();
        return 0;
    }
#endif

#if 1
    String_Performance_Test();
    String_Performance_Test2();

    jimi::Console.ReadKey_NewLine(true);
#endif

#if 1
    String_StrLen_Test();
    jimi::Console.ReadKey_NewLine(true);
#endif

    Jimi_StrLwr_Verify();

#if 1
    String_StrLwr_Test(4);
    String_StrLwr_Test(8);
    String_StrLwr_Test(10);
    String_StrLwr_Test(12);
    String_StrLwr_Test(16);
    String_StrLwr_Test(24);
    String_StrLwr_Test(32);
    String_StrLwr_Test(64);
    String_StrLwr_Test(128);
    String_StrLwr_Test(256);
    String_StrLwr_Test(1024);

    jimi::Console.ReadKey_NewLine(true);
#endif

#if 0
    // char_traits<T>相关字符串函数的测试
    Char_Traits_Test();
#endif

#if 0
    SampleThread *thread = new SampleThread();
    thread->Start();
    thread->Join();
    thread->Abort(1000);
    if (thread) {
        delete thread;
    }

    WorkerThread *workerThread = new WorkerThread();
    workerThread->Start();
    workerThread->Join();
    if (workerThread) {
        delete workerThread;
    }

    printf("\n");
#endif

#if 0
     do {
        jimi::Object *object = new jimi::Object();
        jimi::Object &newObject = object->Clone();
        printf("newObject.Equals(object) = %d\n\n", newObject.Equals(object));
        object->Close();
        newObject.Close();
        if (object)
            delete object;
    } while (0);

    printf("\n");
#endif

#if 0
    do {
        ManualResetEvent *event = new ManualResetEvent(false);
        //ManualResetEvent event2(false);
        EventWaitHandle *event3 = new EventWaitHandle();
        EventWaitHandle *event_base = (EventWaitHandle *)event;
        event->Set();
        event->Reset();
        if (event_base) {
            delete event_base;
            event_base = NULL;
        }
        if (event) {
            //delete event;
            event = NULL;
        }
        bool bSignal = event3->WaitOne();
        if (event3)
            delete event3;
    } while (0);

    printf("\n");
#endif

    jimi::mt::mutex read_mutex;
    jimi::mt::scoped_lock<jimi::mt::mutex> lock(read_mutex);
    lock.acquire(read_mutex);
    lock.try_acquire(read_mutex, 4000);
    lock.release();

    jmLog.log_end();

    jimi::Console.ReadKey();
    return 0;
}

NS_UNITEST_END

NS_JIMI_BEGIN

class MyProgram : public jimi::Program
{
public:
    void Main(int argc, char *argv[]) {
#if defined(UNITEST_USE_NAMESPACE) && (UNITEST_USE_NAMESPACE != 0)
        UnitTest::UnitTest_Main(argc, argv);
#else
        UnitTest_Main(argc, argv);
#endif
    }

    void Main(int argc, wchar_t *argv[]) {
        // TODO:
    }

    void Main(Program::StringArray args) {
        size_t size = args.size();
        printf("args.size() = %d\n\n", size);
        printf("Press any key to continue ...\n");
    }

    void Main(Program::WStringArray args) {
        // TODO:
    }
};

NS_JIMI_END

int main(int argc, char *argv[])
{
#if 0
    jimi::MyProgram program;
#else
    //jimi::Program program(&UnitTest_Main, 0);
    jimi::Program program;
#if defined(UNITEST_USE_NAMESPACE) && (UNITEST_USE_NAMESPACE != 0)
    program.SetMain(&UnitTest::UnitTest_Main);
#else
    program.SetMain(&UnitTest_Main);
#endif
    program.SetReturnValue(1);
#endif
    int nResult = program.Run(argc, argv);
    return nResult;
}

#if defined(_MSC_VER) && (_MSC_VER != 0)
#pragma warning(pop)                    // 恢复warning状态
#endif
