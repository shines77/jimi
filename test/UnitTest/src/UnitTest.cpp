//
// UnitTest.cpp : 定义控制台应用程序的入口点。
//

// whether use crtdbg check?
#ifndef USE_CRTDBG_CHECK
#define USE_CRTDBG_CHECK    1
#endif

//
// From: http://msdn.microsoft.com/zh-cn/library/e5ewb1h3%28v=vs.90%29.aspx
// From: http://msdn.microsoft.com/en-us/library/x98tx3cf.aspx
//
#if USE_CRTDBG_CHECK
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#endif
#endif

#include "UnitTest.h"

#include <jimi/core/jimi_def.h>
#include <jimi/util/cmd_line.h>
#include <jimi/thread/thread.h>
#include <jimi/system/mutex.h>
#include <jimi/system/scoped_lock.h>

#include <jimi/thread/Event.h>
#include <jimi/lang/Object.h>
#include <jimi/lang/String.h>
#include <jimi/lang/Formatter.h>

#include <jimi/log/Logger.h>
#include <jimi/system/stop_watch.h>
#include <jimi/system/Program.h>
#include <jimi/system/Console.h>

#include <asmlib/asmlib.h>
#include "SampleThread.h"
#include "FastMemcpy.h"
//#include "cpp11_format.h"

#include <jimic/platform/win/fast_memcpy.h>
#include <jimic/string/jmf_strings.h>
#include <jimic/string/iconv_win.h>

#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <intrin.h>
#include <string>
#include <locale>
#include <algorithm>
#include <functional>
#include <cctype>

#include <boost/locale.hpp>
#include <boost/locale/encoding.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <iostream>

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
#pragma comment(lib, "libboost_locale-vc120-mt-gd-1_55.lib")

/* 基于CRT的内存泄漏检测 */
#if USE_CRTDBG_CHECK

//
// C++中基于CRT的内存泄漏检测
//
// Reference: http://www.cnblogs.com/weiym/archive/2013/02/25/2932810.html
//

#ifdef _DEBUG
  #define DEBUG_CLIENTBLOCK  new(_CLIENT_BLOCK, __FILE__, __LINE__)
#else
  #define DEBUG_CLIENTBLOCK
#endif

// 如果不定义这个宏, C方式的malloc泄露不会被记录下来
#define _CRTDBG_MAP_ALLOC

// crtdbg.h must be behind the stdlib.h
#include <crtdbg.h>

#ifdef _DEBUG
  #ifndef new
    #define new DEBUG_CLIENTBLOCK
  #endif
#endif

#endif  /* USE_CRTDBG_CHECK */

/**********************************************************
 *
 *  Use Visual Leak Detector(vld) for Visual C++,
 *  Homepage: http://vld.codeplex.com/
 *
 **********************************************************/
#ifdef _MSC_VER
#ifdef _DEBUG
// 如果你没有安装vld(Visual Leak Detector), 请注释掉这一句. vld的官网请看上面.
#include <vld.h>
#endif  /* _DEBUG */
#endif  /* _MSC_VER */

using namespace std;
//using namespace boost;

USING_NS_JIMI;
USING_NS_JIMI_LOG;
USING_NS_JIMI_SYSTEM;

USING_NS_UNITEST;

NS_UNITEST_BEGIN

/* 设置CRTDBG的环境(Debug模式下, 检查内存越界和内存泄漏问题) */

void set_crtdbg_env()
{
/* 使用CRTDBG将会检查内存越界问题, 如果你使用了vld, 内存泄漏信息可关闭 */
#if USE_CRTDBG_CHECK

#ifdef _DEBUG

    // 设置 CRT 报告模式
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);

    // 如果已经引用vld.h, 则不显示crtdbg的内存泄漏信息
#ifndef VLD_RPTHOOK_INSTALL
    // 进程退出时, 显示内存泄漏信息
    // _CRTDBG_CHECK_ALWAYS_DF 表示每次分配/释放内存时, 系统会自动调用_CrtCheckMemory(), 检查内存越界情况
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
#endif  /* VLD_RPTHOOK_INSTALL */

#endif  /* _DEBUG */

#endif  /* USE_CRTDBG_CHECK */
}

/* 预热时间至少要大于500毫秒, 如果还不够, 可以自行增加最小预热时间 */

void jimi_cpu_warmup(int delayTime)
{
#ifndef _DEBUG
    stop_watch sw;
    volatile int sum = 0;
    double elapsedTime = 0.0;
    double delayTimeLimit = (double)delayTime;
    printf("CPU warm up start ...\n");
    fflush(stdout);
    do {
        sw.restart();
        // 如果有聪明的编译器能发现这是一个固定值就NB了, 应该没有
        for (int i = 0; i < 10000; ++i) {
            sum += i;
            // 循环顺序故意颠倒过来的
            for (int j = 5000; j >= 0; --j) {
                sum -= j;
            }
        }
        sw.stop();
        elapsedTime += sw.getMillisec();
    } while (elapsedTime < delayTimeLimit);
    // 输出sum的值只是为了防止编译器把循环优化掉
    printf("sum = %u, time: %0.3f ms\n", sum, elapsedTime);
    printf("CPU warm up done  ... \n\n");
    fflush(stdout);
#endif
}

/* 从size转换成KB, MB, GB, TB */

int get_bytes_display(char *buffer, size_t buf_size, size_t size)
{
    int result;
    if (size <= 8192)
        result = jm_sprintf(buffer, buf_size, "%d", size);
    else if (size < 1024 * 1024)
        result = jm_sprintf(buffer, buf_size, "%d K", size / 1024);
    else if (size < 1024 * 1024 * 1024)
        result = jm_sprintf(buffer, buf_size, "%d M", size / (1024 * 1024));
    else
        result = jm_sprintf(buffer, buf_size, "%d G", size / (1024 * 1024 * 1024));
    return result;
}

/* 从size转换成KB, MB, GB, TB */

int get_bytes_display64(char *buffer, size_t buf_size, uint64_t size)
{
    int result;
    if (size <= 8192ULL)
        result = jm_sprintf(buffer, buf_size, "%d", size);
    else if (size < 0x0000000000100000ULL)
        result = jm_sprintf(buffer, buf_size, "%d K", size / 1024ULL);
    else if (size < 0x0000000040000000ULL)
        result = jm_sprintf(buffer, buf_size, "%d M", size / 0x0000000000100000ULL);
    else if (size < 0x0000010000000000ULL)
        result = jm_sprintf(buffer, buf_size, "%d G", size / 0x0000000040000000ULL);
    else
        result = jm_sprintf(buffer, buf_size, "%d T", size / 0x0000010000000000ULL);
    return result;
}

/* 测试std::string是否使用了COW(Copy On Write) */

void String_Copy_On_Write_Test()
{
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
}

void String_Performance_Test()
{
#ifndef _DEBUG
    const int LOOP_TIMES = 1000000;
#else
    const int LOOP_TIMES = 200000;
#endif
    size_t len1, len2;
    char buffer1[512];
    char buffer2[512];
    double time1, time2, time3, time4;
    double time5, time6, time7, time8;
    int i, j = 0, loop_times = 0;
    stop_watch sw;

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
        //jimi::string str = "abcdefghijk";
        //jimi::string str("abcdefghijk", sizeof("abcdefghijk") - 1);
        jimi::string str("abcdefghijk");
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
        //jimi::string str = "abcdefghijklmnopqrstuvwxyz";
        //jimi::string str("abcdefghijklmnopqrstuvwxyz", sizeof("abcdefghijklmnopqrstuvwxyz") - 1);
        jimi::string str("abcdefghijklmnopqrstuvwxyz");
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
    printf("%-30s time = %0.5f ms.\n", "jimi::string str = \"abcdefghijk\";",   time3);
    printf("\n");
    printf("%-30s time = %0.5f ms.\n", "strcpy()     str = \"abcdefg...xyz\";", time5);
    printf("%-30s time = %0.5f ms.\n", "strcpy_s()   str = \"abcdefg...xyz\";", time6);
    printf("%-30s time = %0.5f ms.\n", "std::string  str = \"abcdefg...xyz\";", time8);
    printf("%-30s time = %0.5f ms.\n", "jimi::string str = \"abcdefg...xyz\";", time7);

    printf("\n");
}

void String_Performance_Test2()
{
#ifndef _DEBUG
    const int LOOP_TIMES = 1000000;
#else
    const int LOOP_TIMES = 200000;
#endif
    size_t len1, len2;
    char buffer1[512];
    char buffer2[512];
    double time1, time2, time3, time4;
    double time5, time6, time7, time8;
    int i, j = 0, loop_times = 0;
    stop_watch sw;

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
        //jimi::string str = "abcdefghijk";
        //jimi::string str("abcdefghijk", sizeof("abcdefghijk") - 1);
        jimi::string str("abcdefghijk");
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
        strcpy(buffer1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
        //strcpy(buffer1 + (i & 2), "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
        /*
        strcpy(buffer1 + 0, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
        strcpy(buffer1 + 4, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
        strcpy(buffer1 + 0, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
        strcpy(buffer1 + 4, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
        //*/
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
        //jimi::string str = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";
        //jimi::string str("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz", sizeof("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz") - 1);
        jimi::string str("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
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

    printf("%-30s time = %0.5f ms.\n", "strcpy()     str = \"abcdefghijk\";",   time1);
    printf("%-30s time = %0.5f ms.\n", "strcpy_s()   str = \"abcdefghijk\";",   time2);
    printf("%-30s time = %0.5f ms.\n", "std::string  str = \"abcdefghijk\";",   time4);
    printf("%-30s time = %0.5f ms.\n", "jimi::string str = \"abcdefghijk\";",   time3);
    printf("\n");
    printf("%-30s time = %0.5f ms.\n", "strcpy()     str = \"abcdefg...xyz\";", time5);
    printf("%-30s time = %0.5f ms.\n", "strcpy_s()   str = \"abcdefg...xyz\";", time6);
    printf("%-30s time = %0.5f ms.\n", "std::string  str = \"abcdefg...xyz\";", time8);
    printf("%-30s time = %0.5f ms.\n", "jimi::string str = \"abcdefg...xyz\";", time7);

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
    stop_watch sw;

#if 1
    jimi::string str1 = "abcdefg";
    jimi::string str2 = "hijklmnop";
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

    jimi::string str3('a');
    printf("str3.c_str() = %s\n", str3.c_str());
    printf("\n");

    jimi::string str4;
    str4 = jabberwocky;
    printf("str4.c_str() = \n%s\n\n", str4.c_str());
    printf("str4.size()  = %d bytes\n", str4.size());
    printf("\n");

    jimi::string str5;
    //str5.append_format_c("%d %x %f %u %c %b", 9999, 8888, 10.9999, 10000000, 33, true);
    str5.append_format_c("%d %s %d %s", 111, "222erer", 33333, "ffffff44");
    printf("str5.c_str() = \n%s\n\n", str5.c_str());
    printf("str5.size()  = %d bytes\n", str5.size());
    printf("\n");

    jimi::string str6;
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

    /////////////////////////////////////////////////////////////////////////////////////////////////

    {
        int delta;
        jimi::string strTest((size_t)999999999);
#if 1
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            //strTest.format("{0}, {1}, {2}, {{3}, {3}", (unsigned int)111, "222erer", (unsigned long)33333, "ffffff44");
            strTest.format("{0}, {1}, {2}, {{3}, {3}", (unsigned int)num1, buf1, (unsigned long)num2, buf2);
        }
        sw.stop();
        time = sw.getMillisec();

        printf("===================================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        str.format(\"{0}, {1}, {2}, {{3}, {3}\",\n"
               "                    111, \"222erer\", 33333, \"ffffff44\");\n");
        printf("    }\n\n");
        printf("===================================================================================\n\n");

        jimi::string strTest2((size_t)128);
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

        printf("===================================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        str.format_c(\"{0}, {1}, {2}, {{3}, {3}\", \"%%d %%s %%d %%s\",\n"
               "                      111, \"222erer\", 33333, \"ffffff44\");\n");
        printf("    }\n\n");
        printf("===================================================================================\n\n");

        jimi::string strTest2((size_t)128);
        strTest2.format_c("{0}, {1}, {2}, {{3}, {3}", "%d %s %d %s", 111, "222erer", 33333, "ffffff44");
        delta = strTest2.size();
#else
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            strTest.append_format_c("%d, %s, %d, {3}, %s", 111, "222erer", 33333, "ffffff44");
        }
        sw.stop();
        time = sw.getMillisec();

        printf("===================================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        str.append_format_c(\"%%d, %%s, %%d, {3}, %%s\",\n"
               "                            111, \"222erer\", 33333, \"ffffff44\");\n");
        printf("    }\n\n");
        printf("===================================================================================\n\n");

        jimi::string strTest2((size_t)128);
        strTest2.append_format_c("%d, %s, %d, {3}, %s", 111, "222erer", 33333, "ffffff44");
        delta = strTest2.size();
#endif

        printf("str.c_str() = %s\n\n", strTest2.c_str());

        printf("time = %0.3f ms, delta = %d.\n\n", time, delta);
        printf("strTest.size()  = %d bytes\n", strTest.size());
        printf("\n");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////

#if 1
    {
        int delta;
        //jimi::string strTest((size_t)1024);
        jimi::string strTest;
        jimi::formatter<6> formator;
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            strTest = formator.append((unsigned int)111, ", ", "222erer", ", ", (unsigned long)33333, ", ", "{3}, ", "ffffff44");
        }
        sw.stop();
        time = sw.getMillisec();

        printf("===================================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        str = formator.append(111, \", \", \"222erer\", \", \",\n"
               "                              33333, \", \", \"{3}, \", \"ffffff44\");\n");
        printf("    }\n\n");
        printf("===================================================================================\n\n");

        jimi::string strTest2;
        strTest2 = formator.append((unsigned int)111, ", ", "222erer", ", ", (unsigned long)33333, ", ", "{3}, ", "ffffff44");
        delta = strTest2.size();

        printf("str.c_str() = %s\n\n", strTest2.c_str());

        printf("time = %0.3f ms, delta = %d.\n\n", time, delta);
        printf("strTest.size()  = %d bytes\n", strTest.size());
        printf("\n");
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////

#if 1
    {
        int delta;
        //jimi::string strTest((size_t)1024);
        jimi::string strTest;
        jimi::formatter<6> formator;
        auto fmtor = jimi::formatter<>().setFloat(jimi::detail::AlignRight, jimi::detail::FillNone, 3, 3).setDoublePrecision(6).setDouble(0, 0, 3, 0);
        auto fmtor2 = jimi::formatter<>().setFloat("%0.3f").setDouble("%0.5f").setInt32("%-08d").setInt64("%+020d").setString("%-30s");
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            strTest.clear();
            //delta = formator.append_to(strTest, (unsigned int)111, ", ", "222erer", ", ", (unsigned long)33333, ", ", "{3}, ", "ffffff44");
            delta = formator.append_to(strTest, (unsigned int)num1, ", ", buf1, ", ", (unsigned long)num2, ", ", "{3}, ", buf2);
        }
        sw.stop();
        time = sw.getMillisec();

        printf("===================================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        n = formator.append_to(str, 111, \", \", \"222erer\", \", \",\n"
               "                               33333, \", \", \"{3}, \", \"ffffff44\");\n");
        printf("    }\n\n");
        printf("===================================================================================\n\n");

        jimi::string strTest2;
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
        //jimi::string strTest((size_t)1024);
        jimi::string strTest;
        jimi::formatter<6> formator;
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            strTest.clear();
            //strTest = formator.format("{0}, {1}, {2}, {{3}, {3}", (unsigned int)111, "222erer", (unsigned long)33333, "ffffff44");
            strTest = formator.format("{0}, {1}, {2}, {{3}, {3}", (unsigned int)num1, buf1, (unsigned long)num2, buf2);
        }
        sw.stop();
        time = sw.getMillisec();

        printf("===================================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        str = formator.format(\"{0}, {1}, {2}, {{3}, {3}\",\n"
               "                              111, \"222erer\", 33333, \"ffffff44\");\n");
        printf("    }\n\n");
        printf("===================================================================================\n\n");

        jimi::string strTest2;
        //strTest2 = formator.format("{0}, {1}, {2}, {{3}, {3}", (unsigned int)111, "222erer", (unsigned long)33333, "ffffff44");
        strTest2 = formator.format("{0}, {1}, {2}, {{3}, {3}", (unsigned int)num1, buf1, (unsigned long)num2, buf2);
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
        //jimi::string strTest((size_t)1024);
        jimi::string strTest;
        jimi::formatter<6> formator;
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            strTest.clear();
            //delta = formator.format_to(strTest, "{0}, {1}, {2}, {{3}, {3}", (unsigned int)111, "222erer", (unsigned long)33333, "ffffff44");
            delta = formator.format_to(strTest, "{0}, {1}, {2}, {{3}, {3}", (unsigned int)num1, buf1, (unsigned long)num2, buf2);
        }
        sw.stop();
        time = sw.getMillisec();

        printf("===================================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        n = formator.format_to(str, \"{0}, {1}, {2}, {{3}, {3}\",\n"
               "                               111, \"222erer\", 33333, \"ffffff44\");\n");
        printf("    }\n\n");
        printf("===================================================================================\n\n");

        jimi::string strTest2;
        //delta = formator.format_to(strTest2, "{0}, {1}, {2}, {{3}, {3}", (unsigned int)111, "222erer", (unsigned long)33333, "ffffff44");
        delta = formator.format_to(strTest2, "{0}, {1}, {2}, {{3}, {3}", (unsigned int)num1, buf1, (unsigned long)num2, buf2);
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
        //jimi::string strTest((size_t)1024);
        jimi::string strTest;
        jimi::formatter<6> formator;
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            strTest.clear();
            delta = formator.format_fast_to(strTest, "?, ?, ?, ??3}, ?", (unsigned int)111, "222erer", (unsigned long)33333, "ffffff44");
            //delta = formator.format_fast_to(strTest, "?, ?, ?, ??3}, ?", (unsigned int)num1, buf1, (unsigned long)num2, buf2);
        }
        sw.stop();
        time = sw.getMillisec();

        printf("===================================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        n = formator.format_fast_to(str, \"?, ?, ?, ??3}, ?\",\n"
               "                                    111, \"222erer\", 33333, \"ffffff44\");\n");
        printf("    }\n\n");
        printf("===================================================================================\n\n");

        jimi::string strTest2;
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
        //jimi::string strTest((size_t)1024);
        jimi::string strTest;
        jimi::formatter<6> formator;
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            strTest.clear();
            delta = formator.format_fast_to2(strTest, "?, ?, ?, ??3}, ?", (unsigned int)111, "222erer", (unsigned long)33333, "ffffff44");
            //delta = formator.format_fast_to2(strTest, "?, ?, ?, ??3}, ?", (unsigned int)num1, buf1, (unsigned long)num2, buf2);
        }
        sw.stop();
        time = sw.getMillisec();

        printf("===================================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        n = formator.format_fast_to2(str, \"?, ?, ?, ??3}, ?\",\n"
               "                                     111, \"222erer\", 33333, \"ffffff44\");\n");
        printf("    }\n\n");
        printf("===================================================================================\n\n");

        jimi::string strTest2;
        delta = formator.format_fast_to2(strTest2, "?, ?, ?, ??3}, ?", (unsigned int)111, "222erer", (unsigned long)33333, "ffffff44");
        //delta = formator.format_fast_to2(strTest2, "?, ?, ?, ??3}, ?", (unsigned int)num1, buf1, (unsigned long)num2, buf2);
        delta = strTest2.size();

        printf("str.c_str() = %s\n\n", strTest2.c_str());

        printf("time = %0.3f ms, delta = %d.\n\n", time, delta);
        printf("strTest.size()  = %d bytes\n", strTest.size());
        printf("\n");
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////

    {
        int delta;
        jimi::string strTest((size_t)999999999);
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            strTest.append_format((unsigned int)111, ", ", "222erer", ", ", (unsigned long)33333, ", ", "{3}, ", "ffffff44");
        }
        sw.stop();
        time = sw.getMillisec();

        printf("===================================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        str.append_format(111, \", \", \"222erer\", \", \",\n"
               "                          33333, \", \", \"{3}, \", \"ffffff44\");\n");
        printf("    }\n\n");
        printf("===================================================================================\n\n");

        jimi::string strTest2((size_t)128);
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
        jimi::string strTest((size_t)999999999);
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            strTest.append_format((unsigned int)num1, ", ", buf1, ", ", (unsigned long)num2, ", ", "{3}, ", buf2);
        }
        sw.stop();
        time = sw.getMillisec();

        printf("===================================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        str.append_format(num1, \", \", buf1, \", \",\n"
               "                          num2, \", \", \"{3}, \", buf2);\n");
        printf("    }\n\n");
        printf("===================================================================================\n\n");

        jimi::string strTest2((size_t)128);
        strTest2.append_format((unsigned int)num1, ", ", buf1, ", ", (unsigned long)num2, ", ", "{3}, ", buf2);
        delta = strTest2.size();

        std::string strStd2 = strTest2.toStdString();

        printf("str.c_str() = %s\n\n", strTest2.c_str());

        printf("time = %0.3f ms, delta = %d.\n\n", time, delta);
        printf("strTest.size()  = %d bytes\n", strTest.size());
        printf("\n");
    }

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

        printf("===================================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        str = format(\"{0}, {1}, {2}, $3}, {3}\",\n"
               "                      111, \"222erer\", 33333, \"ffffff44\");\n");
        printf("    }\n\n");
        printf("===================================================================================\n\n");

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

        printf("===================================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        str = sprintf(buf, \"%%d, %%s, %%d, {3}, %%s\",\n"
               "                      111, \"222erer\", 33333, \"ffffff44\");\n");
        printf("    }\n\n");
        printf("===================================================================================\n\n");

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

static const unsigned char s_toUpper[] =
{
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,
    16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
    32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
    48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
    64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
    80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
    96,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
    80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  123, 124, 125, 126, 127,
    128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
    144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
    160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
    176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
    192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
    208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
};

template <typename T>
void strupr_table(T *str)
{
    while (*str != NULL) {
        *str = static_cast<T>(s_toUpper[static_cast<unsigned char>(*str)]);
        ++str;
    }
}

static const unsigned char s_toLower[] =
{
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,
    16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
    32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
    48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
    64,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
    112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 91,  92,  93,  94,  95,
    96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
    112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
    128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
    144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
    160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
    176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
    192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
    208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
};

//
// C++ (coverting lowercase to uppercase without the 'toupper' function)??!
//
// From: http://arstechnica.com/civis/viewtopic.php?f=20&t=696922
//

#define ngx_tolower(c)      (unsigned char) ((c >= 'A' && c <= 'Z') ? (c |  0x20) : c)
#define ngx_toupper(c)      (unsigned char) ((c >= 'a' && c <= 'z') ? (c & ~0x20) : c)

void ngx_strlwr(unsigned char *str)
{
    while (*str != NULL) {
        *str = ngx_tolower(*str);
        str++;
    }
}

/*
template <class T>
void stl_tolower_(T *begin, T *end)
{
    std::for_each(begin, end, std::tolower);
}

template <class T, class Func>
void stl_tolower(std::basic_string<T> &str)
{
    std::for_each(str.begin(), str.end(), std::tolower);
}

template <class T, class Func>
void stl_toupper(std::string& s)
{
    std::for_each(s.begin(), s.end(), std::toupper);
}
//*/

template <typename T>
void strlwr_std(T *str)
{
    T c;
    while ((c = *str) != NULL) {
        if (c >= static_cast<T>('A') && c <= static_cast<T>('Z')) {
            c |= 0x20;
            *str = c;
        }
        ++str;
    }
}

template <typename T>
void strlwr_table(T *str)
{
    while (*str != NULL) {
        *str = static_cast<T>(s_toLower[static_cast<unsigned char>(*str)]);
        ++str;
    }
}

template <class T>
inline T my_tolower(T _ch)
{
    static std::locale _loc;
    return std::tolower(_ch, _loc);
}

//template <class T>
inline char my_stl_tolower(char _ch, const std::locale _loc)
{
    return std::tolower(_ch, _loc);
}

void ngx_strlow(unsigned char *dest, unsigned char *src, size_t n)
{
    while (n) {
        *dest = ngx_tolower(*src);
        dest++;
        src++;
        n--;
    }
}

void Jm_StrLwr_Verify()
{
    int i, l = 0;
    int ch, errors = 0;
    size_t size, bufSize, str_len;
    int alignment, offset;
    char *buffer1, *buffer2, *buffer3, *buffer4;
    char *strTest1, *strTest2, *temp;
    stop_watch sw;
    char checkCharList[] = { '@', 'A', 'Q', 'Z', '[', 'a', 'q', 'z', '0', ' ', '\b' };
    int sizeCharList = jm_countof(checkCharList);

    printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\n");
    printf("Jm_StrLwr_Verify() start ...\n\n");

    sw.restart();
#if 1
    for (ch = 1; ch < 256; ++ch) {
#else
    for (l = 0; l < sizeCharList; ++l) {
        ch = checkCharList[l];
#endif
        // 校验头部32字节
        alignment = 4;
        bufSize = 256 * sizeof(char);
        buffer1 = (char *)_aligned_offset_malloc(bufSize, alignment, 0);
        buffer2 = (char *)_aligned_offset_malloc(bufSize, alignment, 0);
        buffer3 = (char *)_aligned_offset_malloc(bufSize, alignment, 0);
        buffer4 = (char *)_aligned_offset_malloc(bufSize, alignment, 0);
        for (i = 0; i < (int)bufSize; ++i) {
            //buffer1[i] = ngx_toupper(ch);
            //buffer2[i] = ngx_toupper(ch);
            buffer1[i] = 'A' + (ch & 15);
            buffer2[i] = 'A' + (ch & 15);
        }
        buffer1[bufSize - 1] = '\0';
        buffer2[bufSize - 1] = '\0';
        for (offset = 0; offset < 64; ++offset) {
            for (str_len = 0; str_len < 128; ++str_len) {
                memcpy(buffer3, buffer1, bufSize);
                memcpy(buffer4, buffer2, bufSize);

                strTest1 = buffer3 + offset;
                memset((void *)strTest1, ch, str_len);
                strTest1[str_len] = '\0';
                temp = jm_strlwr(strTest1, str_len + 1);
                //temp = strlwr(strTest1);

                if (ch == 1 && offset == 0 && str_len == 20) {
                    //_getch();
                    ch = ch;
                }

                if (ch == 'A' && offset == 28 && str_len == 24) {
                    //_getch();
                    ch = ch;
                }

                if (ch == 'A' && offset == 33 && str_len == 25) {
                    ch = ch;
                }

                if (ch == 'A' && offset == 32 && str_len == 24) {
                    ch = ch;
                }

                strTest2 = buffer4 + offset;
                memset((void *)strTest2, ch, str_len);
                strTest2[str_len] = '\0';
                size = jmf_strlwr(strTest2);

                if (memcmp(buffer3, buffer4, bufSize) != 0) {
                    // got some errors
                    printf("Error at char of: \'%c\', ch = %d, offset = %d, strlen = %d\n", ch, ch, offset, str_len);
                    printf("strTest1 = %s\n", strTest1);
                    printf("strTest2 = %s\n", strTest2);
                    errors++;
                    _getch();
                    printf("\n");
                }
            }
        }
        if (buffer1) _aligned_free(buffer1);
        if (buffer2) _aligned_free(buffer2);
        if (buffer3) _aligned_free(buffer3);
        if (buffer4) _aligned_free(buffer4);
    }
    sw.stop();

    if (errors == 0)
        printf("All verify is passed ... [time = %0.3f ms]\n", sw.getMillisec());

    printf("\n");
    printf("Jm_StrLwr_Verify() done ...\n\n");
    printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\n");
}

/* 每次进行strlwr转换时是否重新还原字符串到初始状态? */
#define STRLWR_RECOPY_EVERY_TIME        0

void StrLwr_Test(int nTestLen)
{
    int i;
    static const int alignment = 8;
#if defined(_DEBUG) || 0
    static const int loop_times = 8;
#else
    static const int loop_times = 200000;
#endif
    int nBufLen, nStrLen;
    char *tolower_test1, *tolower_test2, *tolower_test3;
    char *tolower_test4, *tolower_test5, *tolower_test6, *tolower_test7;
    char *result_str = NULL;
    double time1, time2, time3, time4, time5, time6, time7;
    stop_watch_ex sw;

    tolower_test1 = NULL;
    tolower_test2 = NULL;
    tolower_test3 = NULL;
    tolower_test4 = NULL;
    tolower_test5 = NULL;
    tolower_test6 = NULL;

    time1 = 0.0;
    time2 = 0.0;
    time3 = 0.0;
    time4 = 0.0;
    time5 = 0.0;
    time6 = 0.0;

    nStrLen = ::jm_strlen(jabberwocky);
    if (nTestLen > nStrLen)
        nTestLen = nStrLen;
    nBufLen = nTestLen + 32;
    if (nBufLen < 64)
        nBufLen = 64;

#if 1
    tolower_test1 = (char *)::_aligned_malloc(nBufLen * sizeof(char), alignment);
    if (tolower_test1) {
        ::jm_strncpy(tolower_test1, nBufLen, jabberwocky, nTestLen);
        tolower_test1[nTestLen] = '\0';
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
#if defined(STRLWR_RECOPY_EVERY_TIME) && (STRLWR_RECOPY_EVERY_TIME != 0)
            sw.suspend();
            ::jm_strncpy(tolower_test1, nBufLen, jabberwocky, nTestLen);
            tolower_test1[nTestLen] = '\0';
            sw.resume();
#endif
            result_str = ::strlwr(tolower_test1);
        }
        sw.stop();
        time1 = sw.getMillisec();
        //time1 = sw.getTotalMillisec();
        //printf("tolower_test1.c_str() = \n%s\n\ntolower_test1.size() = %d bytes\n", tolower_test1, ::jm_strlen(tolower_test1));
    }
    //printf("\n");

    tolower_test2 = (char *)::_aligned_malloc(nBufLen * sizeof(char), alignment);
    if (tolower_test2) {
        ::jm_strncpy(tolower_test2, nBufLen, jabberwocky, nTestLen);
        tolower_test2[nTestLen] = '\0';
        int str_len = ::jm_strlen(tolower_test2) + 1;
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
#if defined(STRLWR_RECOPY_EVERY_TIME) && (STRLWR_RECOPY_EVERY_TIME != 0)
            sw.suspend();
            ::jm_strncpy(tolower_test2, nBufLen, jabberwocky, nTestLen);
            tolower_test2[nTestLen] = '\0';
            sw.resume();
#endif
            result_str = ::jm_strlwr(tolower_test2, str_len);
        }
        sw.stop();
        time2 = sw.getMillisec();
        //time2 = sw.getTotalMillisec();
        //printf("tolower_test2.c_str() = \n%s\n\ntolower_test2.size() = %d bytes\n", tolower_test2, ::jm_strlen(tolower_test2));
    }
    //printf("\n");

    tolower_test3 = (char *)::_aligned_malloc(nBufLen * sizeof(char), alignment);
    if (tolower_test3) {
        ::jm_strncpy(tolower_test3, nBufLen, jabberwocky, nTestLen);
        tolower_test3[nTestLen] = '\0';
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
#if defined(STRLWR_RECOPY_EVERY_TIME) && (STRLWR_RECOPY_EVERY_TIME != 0)
            sw.suspend();
            ::jm_strncpy(tolower_test3, nBufLen, jabberwocky, nTestLen);
            tolower_test3[nTestLen] = '\0';
            sw.resume();
#endif
            ngx_strlwr((unsigned char *)tolower_test3);
        }
        sw.stop();
        time3 = sw.getMillisec();
        //time3 = sw.getTotalMillisec();
        //printf("tolower_test3.c_str() = \n%s\n\tolower_test3.size() = %d bytes\n", tolower_test3, ::jm_strlen(tolower_test3));
    }
    //printf("\n");

    tolower_test4 = (char *)::_aligned_malloc(nBufLen * sizeof(char), alignment);
    if (tolower_test4) {
        ::jm_strncpy(tolower_test4, nBufLen, jabberwocky, nTestLen);
        tolower_test4[nTestLen] = '\0';
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
#if defined(STRLWR_RECOPY_EVERY_TIME) && (STRLWR_RECOPY_EVERY_TIME != 0)
            sw.suspend();
            ::jm_strncpy(tolower_test4, nBufLen, jabberwocky, nTestLen);
            tolower_test4[nTestLen] = '\0';
            sw.resume();
#endif
            strlwr_std(tolower_test4);
        }
        sw.stop();
        time4 = sw.getMillisec();
        //time4 = sw.getTotalMillisec();
        //printf("tolower_test4.c_str() = \n%s\n\tolower_test4.size() = %d bytes\n", tolower_test4, ::jm_strlen(tolower_test4));
    }
    //printf("\n");

    tolower_test5 = (char *)::_aligned_malloc(nBufLen * sizeof(char), alignment);
    if (tolower_test5) {
        ::jm_strncpy(tolower_test5, nBufLen, jabberwocky, nTestLen);
        tolower_test5[nTestLen] = '\0';
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
#if defined(STRLWR_RECOPY_EVERY_TIME) && (STRLWR_RECOPY_EVERY_TIME != 0)
            sw.suspend();
            ::jm_strncpy(tolower_test5, nBufLen, jabberwocky, nTestLen);
            tolower_test5[nTestLen] = '\0';
            sw.resume();
#endif
            strlwr_table(tolower_test5);
        }
        sw.stop();
        time5 = sw.getMillisec();
        //time5 = sw.getTotalMillisec();
        //printf("tolower_test5.c_str() = \n%s\n\tolower_test5.size() = %d bytes\n", tolower_test5, ::jm_strlen(tolower_test5));
    }
    //printf("\n");

#if 1
    using namespace std::placeholders; 
    tolower_test6 = (char *)::_aligned_malloc(nBufLen * sizeof(char), alignment);
    if (tolower_test6) {
        char *begin, *end;
        ::jm_strncpy(tolower_test6, nBufLen, jabberwocky, nTestLen);
        tolower_test6[nTestLen] = '\0';
        begin = tolower_test6;
        end = tolower_test6 + jm_strlen(tolower_test6);
        std::string strlow = tolower_test6;
        std::locale loc;
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
#if defined(STRLWR_RECOPY_EVERY_TIME) && (STRLWR_RECOPY_EVERY_TIME != 0)
            sw.suspend();
            ::jm_strncpy(tolower_test6, nBufLen, jabberwocky, nTestLen);
            tolower_test6[nTestLen] = '\0';
            sw.resume();
#endif
            //std::for_each(strlow.begin(), strlow.end(), std::bind(std::tolower<char>, _1, loc));
            //std::for_each(strlow.begin(), strlow.end(), std::bind(std::toupper<char>, _1, loc));
            std::for_each(strlow.begin(), strlow.end(), std::tolower);
            //std::transform(strlow.begin(), strlow.end(), strlow.begin(), ::tolower);
        }
        sw.stop();
        time6 = sw.getMillisec();
        //time5 = sw.getTotalMillisec();
        //printf("tolower_test6.c_str() = \n%s\n\tolower_test6.size() = %d bytes\n", tolower_test6, ::jm_strlen(tolower_test6));
#if 0
            //stl_tolower<char>(begin, end);
            typedef char (*fn_tolower)(char _Ch, const std::locale& _Loc);
            typedef char (*fn_tolower2)(char _Ch);
            typedef std::function<char (char, const std::locale&)> fcn_tolower;
            //fn_tolower pfn_tolower = &std::tolower;
            //stl_tolower<char, fn_tolower>(strlow);
            //fn_tolower2 pfn_tolower = &my_tolower;
            //std::for_each<std::string::const_iterator, fn_tolower2>(strlow.begin(), strlow.end(), pfn_tolower);
            //std::for_each<std::string::const_iterator, fcn_tolower>(strlow.begin(), strlow.end(), std::bind(my_stl_tolower, _1, loc));
            //std::for_each(strlow.begin(), strlow.end(), std::bind(my_stl_tolower, _1, loc));
            std::for_each(strlow.begin(), strlow.end(), std::bind(std::tolower<char>, _1, loc));
            //std::transform(strlow.begin(), strlow.end(), strlow.begin(), ::tolower);
            //std::transform(strlow.begin(), strlow.end(), strlow.begin(), std::tolower);
#endif
    }
    //printf("\n");
#endif

#endif

#if 0
    tolower_test7 = (char *)::_aligned_offset_malloc(nBufLen * sizeof(char), alignment, (alignment - 4));
#else
    tolower_test7 = (char *)::_aligned_malloc(nBufLen * sizeof(char), alignment);
#endif
    if (tolower_test7) {
        size_t size;
        ::jm_strncpy(tolower_test7, nBufLen - 1, jabberwocky, nTestLen);
        tolower_test7[nTestLen] = '\0';
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
#if defined(STRLWR_RECOPY_EVERY_TIME) && (STRLWR_RECOPY_EVERY_TIME != 0)
            sw.suspend();
            if (::jm_strncpy(tolower_test7, nBufLen - 1, jabberwocky, nTestLen))
                tolower_test7[nTestLen] = '\0';
            sw.resume();
#endif
            size = ::jmf_strlwr(tolower_test7);
            //size = ::jmf_strlen(tolower_test7);
        }
        sw.stop();
        time7 = sw.getMillisec();
        //time6 = sw.getTotalMillisec();
        //printf("tolower_test7.c_str() = \n%s\n\tolower_test7.size() = %d bytes\n", tolower_test7, size);
    }
    //printf("\n");

    printf("str               size = %d bytes\n\n", nTestLen);

    if (loop_times > 100) {
        printf("strlwr(str)       time = %-7.3f ms\n", time1);
        printf("strlwr_s(str)     time = %-7.3f ms\n", time2);
        printf("ngx_strlwr(str)   time = %-7.3f ms\n", time3);
        printf("stl_tolower(str)  time = %-7.3f ms\n", time6);
        printf("strlwr_std(str)   time = %-7.3f ms\n", time4);
        printf("strlwr_table(str) time = %-7.3f ms\n", time5);
        printf("jmf_strlwr(str)   time = %-7.3f ms\n", time7);
        //printf("jmf_strlwr(str)   time = %-7.3f ms, size = %d\n", time7, jm_strlen(tolower_test7));
    }
    else {
        printf("strlwr(str)       time = %-9.6f ms\n", time1);
        printf("strlwr_s(str)     time = %-9.6f ms\n", time2);
        printf("ngx_strlwr(str)   time = %-9.6f ms\n", time3);
        printf("stl_tolower(str)  time = %-7.3f ms\n", time6);
        printf("strlwr_std(str)   time = %-9.6f ms\n", time4);
        printf("strlwr_table(str) time = %-9.6f ms\n", time5);
        printf("jmf_strlwr(str)   time = %-9.6f ms\n", time7);
    }
    printf("\n");

    if (tolower_test1) {
        ::_aligned_free(tolower_test1);
        tolower_test1 = NULL;
    }
    if (tolower_test2) {
        ::_aligned_free(tolower_test2);
        tolower_test2 = NULL;
    }
    if (tolower_test3) {
        ::_aligned_free(tolower_test3);
        tolower_test3 = NULL;
    }
    if (tolower_test4) {
        ::_aligned_free(tolower_test4);
        tolower_test4 = NULL;
    }
    if (tolower_test5) {
        ::_aligned_free(tolower_test5);
        tolower_test5 = NULL;
    }
    if (tolower_test6) {
        ::_aligned_free(tolower_test6);
        tolower_test6 = NULL;
    }
    if (tolower_test7) {
#if 0
        uintptr_t pv = (uintptr_t)tolower_test7;
        pv = (pv + (sizeof(uintptr_t) - 1)) & ~(sizeof(uintptr_t) - 1);
        uintptr_t *pvReal = (uintptr_t *)pv - 1;
#ifdef _DEBUG
        pvReal--;
#endif
        printf("pv = 0x%08X, pvReal = 0x%08X, *pvReal = 0x%08X\n\n", pv, pvReal, *pvReal);
        ::system("pause");
#endif
        ::_aligned_free(tolower_test7);
        tolower_test7 = NULL;
    }
}

//
// Faster strlen function. Warning: in rare cases it may
// cause page faults (if the string is at the end of
// the page). To avoid this, you must align the start
// of the string by 4 bytes, or you must add 3 extra
// bytes to the end of the string.
// E.g., instead of char* str = (char*)malloc(size); use
//                  char* str = (char*)malloc(size + 3);
//
int strlen_my(const char *s) {
	int len = 0;
	for(;;) {
		unsigned x = *(unsigned*)s;
		if ((x & 0xFF) == 0) return len;
		if ((x & 0xFF00) == 0) return len + 1;
		if ((x & 0xFF0000) == 0) return len + 2;
		if ((x & 0xFF000000) == 0) return len + 3;
		s += 4, len += 4;
	}
}

int strlen_AgnerFog(const char* s) {
	__asm {
        mov     eax, [s]               ; get pointer s 
        lea     edx, [eax+3]           ; pointer+3 used in the end 
l1:     
		mov     ebx, [eax]             ; read 4 bytes of string 
        add     eax, 4                 ; increment pointer 
        lea     ecx, [ebx-01010101H]   ; subtract 1 from each byte 
        not     ebx                    ; invert all bytes 
        and     ecx, ebx               ; and these two 
        and     ecx, 80808080H         ; test all sign bits 
        jz      l1                     ; no zero bytes, continue loop 

        mov     ebx, ecx 
        shr     ebx, 16 
        test    ecx, 00008080H         ; test first two bytes 
        cmovz   ecx, ebx               ; shift if not in first 2 bytes 
        lea     ebx, [eax+2]           ; .. and increment pointer by 2 
        cmovz   eax, ebx 
        add     cl,  cl                ; test first byte 
        sbb     eax, edx               ; compute length 
	}
}

__declspec(naked)
inline int __cdecl __builtin_clz(int bitmask)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        bsr eax, eax
        neg eax
        add eax, 31
        ret
    }
}

__declspec(naked)
inline int __cdecl __builtin_ctz(int bitmask)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        bsf eax, eax
        ret
    }
}

//
// From: http://www.strchr.com/strcmp_and_strlen_using_sse_4.2
//
size_t sse2_strlen(const char* s)
{
    __m128i zero = _mm_set1_epi8( 0 );
    __m128i *s_aligned = (__m128i*) (((long)s) & -0x10L);
    uint8_t misbits = ((long)s) & 0x0f;
    __m128i s16cs = _mm_load_si128( s_aligned );
    __m128i bytemask = _mm_cmpeq_epi8( s16cs, zero );
    int bitmask = _mm_movemask_epi8( bytemask );
    bitmask = (bitmask >> misbits) << misbits;

    // Alternative: use TEST instead of BSF, then BSF at end (only). Much better on older CPUs
    // TEST has latency 1, while BSF has 3!
    while (bitmask == 0) {
        s16cs = _mm_load_si128( ++s_aligned );
        bytemask = _mm_cmpeq_epi8( s16cs, zero );
        bitmask = _mm_movemask_epi8( bytemask );
    }

    // bsf only takes 1 clock cycle on modern cpus
    return ((( const char* ) s_aligned ) - s) + __builtin_ctz(bitmask);
}

void Fast_StrLen_Test()
{
#ifndef _DEBUG
    const int LOOP_TIMES = 1000000;
#else
    const int LOOP_TIMES = 200000;
#endif
    char *str1, *str2, *str3;
    char *pbuffer;
    size_t len1, len2, len3, len4;
    size_t len5, len6, len7, len8;
    char buffer1[1024], buffer2[1024], buffer3[1024], buffer4[1024];
    double time1, time2, time3, time4;
    double time5, time6, time7, time8;
    int i, j = 0, loop_times = 0;
    stop_watch sw;

    ////////////////////////////////////////////////////////////////////////////////////////////////////

    typedef size_t (*pfn_strlen) (const char *_Str);
    pfn_strlen pstrlen = (pfn_strlen)&strlen;
    pbuffer = &buffer1[0];

    // 32 bytes
    strcpy(buffer1, "abcdefghijklmnopqrstuvwxyz|ab1");

    sw.restart();
#if 0
    for (i = 0; i < LOOP_TIMES; ++i) {
        len1 = strlen(buffer1);
    }
#else
    __asm {
        push    ebx
        push    edx
        mov     ebx, LOOP_TIMES
        align   16
strlen_loop1_1:
        mov     edx, pbuffer
        push    edx
        call    dword ptr [pstrlen]
        add     esp, 4
        mov     len1, eax
        dec     ebx
        jnz     strlen_loop1_1
        pop     edx
        pop     ebx
    }
#endif
    sw.stop();
    time1 = sw.getMillisec();

    // 32 bytes
    strcpy(buffer2, "abcdefghijklmnopqrstuvwxyz|ab2");

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len2 = jimi::char_traits<char>::strlen(buffer2);
    }
    sw.stop();
    time2 = sw.getMillisec();

    // 32 bytes
    strcpy(buffer3, "abcdefghijklmnopqrstuvwxyz|ab3");

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len3 = A_strlen(buffer3);
    }
    sw.stop();
    time3 = sw.getMillisec();

    // 32 bytes
    strcpy(buffer4, "abcdefghijklmnopqrstuvwxyz|ab4");

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len4 = jmf_strlen(buffer4);
    }
    sw.stop();
    time4 = sw.getMillisec();

    // 64 bytes
    strcpy(buffer1, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijk1");

    sw.restart();
#if 0
    for (i = 0; i < LOOP_TIMES; ++i) {
        len5 = strlen(buffer1);
    }
#else
    __asm {
        push    ebx
        push    edx
        mov     ebx, LOOP_TIMES
        align   16
strlen_loop1_5:
        mov     edx, pbuffer
        push    edx
        call    dword ptr [pstrlen]
        add     esp, 4
        mov     len5, eax
        dec     ebx
        jnz     strlen_loop1_5
        pop     edx
        pop     ebx
    }
#endif
    sw.stop();
    time5 = sw.getMillisec();

    // 64 bytes
    strcpy(buffer2, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijk2");

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len6 = jimi::char_traits<char>::strlen(buffer2);
    }
    sw.stop();
    time6 = sw.getMillisec();

    // 64 bytes
    strcpy(buffer3, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijk3");

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len7 = A_strlen(buffer3);
    }
    sw.stop();
    time7 = sw.getMillisec();

    // 64 bytes
    strcpy(buffer4, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijk4");

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len8 = jmf_strlen(buffer4);
    }
    sw.stop();
    time8 = sw.getMillisec();

    buffer1[20] = '\0';
    printf("buffer1 = %s, len1 = %d, len2 = %d, len3 = %d, len4 = %d\n\n", buffer1, len1, len2, len3, len4);

    printf("%-30s bytes = %d, time = %0.5f ms.\n", "::strlen()",                    len1, time1);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "char_traits<char>::strlen()",   len2, time2);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "asmlib::A_strlen()",            len3, time3);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "jimi::jmf_strlen()",            len4, time4);
    printf("\n");
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "::strlen()",                    len5, time5);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "char_traits<char>::strlen()",   len6, time6);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "asmlib::A_strlen()",            len7, time7);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "jimi::jmf_strlen()",            len8, time8);
    printf("\n");

    ////////////////////////////////////////////////////////////////////////////////////////////////////

    // 128 bytes
    strcpy(buffer1, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrs1");

    sw.restart();
#if 0
    for (i = 0; i < LOOP_TIMES; ++i) {
        len1 = strlen(buffer1);
    }
#else
    __asm {
        push    ebx
        push    edx
        mov     ebx, LOOP_TIMES
        align   16
strlen_loop2_1:
        mov     edx, pbuffer
        push    edx
        call    dword ptr [pstrlen]
        add     esp, 4
        mov     len1, eax
        dec     ebx
        jnz     strlen_loop2_1

#if 0
		// SSE 指令
		addss	xmm1, xmmword ptr [eax]
		// AVX 指令
		vaddss	xmm1, xmmword ptr [eax]
#endif

        pop     edx
        pop     ebx
    }
#endif
    sw.stop();
    time1 = sw.getMillisec();

    // 128 bytes
    strcpy(buffer2, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrs2");

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len2 = jimi::char_traits<char>::strlen(buffer2);
    }
    sw.stop();
    time2 = sw.getMillisec();

    // 128 bytes
    strcpy(buffer3, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrs3");

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len3 = A_strlen(buffer3);
    }
    sw.stop();
    time3 = sw.getMillisec();

    // 128 bytes
    strcpy(buffer4, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrs4");

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len4 = jmf_strlen(buffer4);
    }
    sw.stop();
    time4 = sw.getMillisec();

    // 256 bytes
    strcpy(buffer1, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
                    "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrs1");

    sw.restart();
#if 0
    for (i = 0; i < LOOP_TIMES; ++i) {
        len5 = strlen(buffer1);
    }
#else
    __asm {
        push    ebx
        push    edx
        mov     ebx, LOOP_TIMES
        align   16
strlen_loop2_5:
        mov     edx, pbuffer
        push    edx
        call    dword ptr [pstrlen]
        add     esp, 4
        mov     len5, eax
        dec     ebx
        jnz     strlen_loop2_5
        pop     edx
        pop     ebx
    }
#endif
    sw.stop();
    time5 = sw.getMillisec();

    // 256 bytes
    strcpy(buffer2, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
                    "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrs2");

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len6 = jimi::char_traits<char>::strlen(buffer2);
    }
    sw.stop();
    time6 = sw.getMillisec();

    // 256 bytes
    strcpy(buffer3, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
                    "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrs3");

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len7 = A_strlen(buffer3);
    }
    sw.stop();
    time7 = sw.getMillisec();

    // 256 bytes
    strcpy(buffer4, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
                    "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrs4");

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len8 = jmf_strlen(buffer4);
    }
    sw.stop();
    time8 = sw.getMillisec();

    buffer1[20] = '\0';
    printf("buffer1 = %s, len1 = %d, len2 = %d, len3 = %d, len4 = %d\n\n", buffer1, len1, len2, len3, len4);

    printf("%-30s bytes = %d, time = %0.5f ms.\n", "::strlen()",                    len1, time1);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "char_traits<char>::strlen()",   len2, time2);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "asmlib::A_strlen()",            len3, time3);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "jimi::jmf_strlen()",            len4, time4);
    printf("\n");
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "::strlen()",                    len5, time5);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "char_traits<char>::strlen()",   len6, time6);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "asmlib::A_strlen()",            len7, time7);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "jimi::jmf_strlen()",            len8, time8);
    printf("\n");

    ///////////////////////////////////////////////////////////////////////

    // 511 bytes
    strcpy(buffer1, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
        "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
        "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
        "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqr1"
        );

    sw.restart();
#if 0
    for (i = 0; i < LOOP_TIMES; ++i) {
        len5 = strlen(buffer1);
    }
#else
    __asm {
        push    ebx
        push    edx
        mov     ebx, LOOP_TIMES
        align   16
strlen_loop3_5:
        mov     edx, pbuffer
        push    edx
        call    dword ptr [pstrlen]
        add     esp, 4
        mov     len5, eax
        dec     ebx
        jnz     strlen_loop3_5
        pop     edx
        pop     ebx
    }
#endif
    sw.stop();
    time5 = sw.getMillisec();

    // 511 bytes
    strcpy(buffer2, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
        "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
        "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
        "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqr2"
        );

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len6 = jimi::char_traits<char>::strlen(buffer2);
    }
    sw.stop();
    time6 = sw.getMillisec();

    // 511 bytes
    strcpy(buffer3, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
        "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
        "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
        "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqr3"
        );

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len7 = A_strlen(buffer3);
    }
    sw.stop();
    time7 = sw.getMillisec();

    // 511 bytes
    strcpy(buffer4, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
        "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
        "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
        "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqr4"
        );

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len8 = jmf_strlen(buffer4);
    }
    sw.stop();
    time8 = sw.getMillisec();

    buffer1[20] = '\0';
    printf("buffer1 = %s, len1 = %d, len2 = %d, len3 = %d, len4 = %d\n\n", buffer1, len5, len6, len7, len8);

    printf("%-30s bytes = %d, time = %0.5f ms.\n", "::strlen()",                    len5, time5);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "char_traits<char>::strlen()",   len6, time6);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "asmlib::A_strlen()",            len7, time7);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "jimi::jmf_strlen()",            len8, time8);
    printf("\n");

    ///////////////////////////////////////////////////////////////////////

    // long string jabberwocky
    strcpy(buffer1, jabberwocky);

    sw.restart();
#if 0
    for (i = 0; i < LOOP_TIMES; ++i) {
        len5 = strlen(buffer1);
    }
#else
    __asm {
        push    ebx
        push    edx
        mov     ebx, LOOP_TIMES
        align   16
strlen_loop4_5:
        mov     edx, pbuffer
        push    edx
        call    dword ptr [pstrlen]
        add     esp, 4
        mov     len5, eax
        dec     ebx
        jnz     strlen_loop4_5
        pop     edx
        pop     ebx
    }
#endif
    sw.stop();
    time5 = sw.getMillisec();

    // long string jabberwocky
    strcpy(buffer2, jabberwocky);

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len6 = jimi::char_traits<char>::strlen(buffer2);
    }
    sw.stop();
    time6 = sw.getMillisec();

    // long string jabberwocky
    strcpy(buffer3, jabberwocky);

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len7 = A_strlen(buffer3);
    }
    sw.stop();
    time7 = sw.getMillisec();

    // long string jabberwocky
    strcpy(buffer4, jabberwocky);

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len8 = jmf_strlen(buffer4);
    }
    sw.stop();
    time8 = sw.getMillisec();

    buffer1[20] = '\0';
    printf("buffer1 = %s, len1 = %d, len2 = %d, len3 = %d, len4 = %d\n\n", buffer1, len5, len6, len7, len8);

    printf("%-30s bytes = %d, time = %0.5f ms.\n", "::strlen()",                    len5, time5);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "char_traits<char>::strlen()",   len6, time6);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "asmlib::A_strlen()",            len7, time7);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "jimi::jmf_strlen()",            len8, time8);
    printf("\n");

    size_t buf_size = 2 * 1024 * 1024 * sizeof(char);

    str1 = (char *)::malloc(buf_size);
    if (str1) {
        memset(str1, 'A', buf_size - 1);
        str1[buf_size - 1] = '\0';

        sw.restart();
#if 0
        for (i = 0; i < 1000; ++i) {
            len1 = ::strlen(str1);
        }
#else
            __asm {
                push    ebx
                push    edx
                mov     ebx, 1000
                align   16
strlen_loop4_1:
                mov     edx, str1
                push    edx
                call    dword ptr [pstrlen]
                add     esp, 4
                mov     len1, eax
                dec     ebx
                jnz     strlen_loop4_1
                pop     edx
                pop     ebx
            }
#endif
        sw.stop();
        time1 = sw.getMillisec();
    }

    str2 = (char *)::malloc(buf_size);
    if (str2) {
        memset(str2, 'A', buf_size - 1);
        str2[buf_size - 1] = '\0';

        sw.restart();
        for (i = 0; i < 1000; ++i) {
            len2 = ::A_strlen(str2);
        }
        sw.stop();
        time2 = sw.getMillisec();
    }

    str3 = (char *)::malloc(buf_size);
    if (str3) {
        memset(str3, 'A', buf_size - 1);
        str3[buf_size - 1] = '\0';

        sw.restart();
        for (i = 0; i < 1000; ++i) {
            len3 = ::jmf_strlen(str3);
        }
        sw.stop();
        time3 = sw.getMillisec();
    }

    char bufsize_text1[128], bufsize_text2[128], bufsize_text3[128];
    bufsize_text1[0] = '\0';
    get_bytes_display(bufsize_text1, jm_countof(bufsize_text1), len1);
    bufsize_text2[0] = '\0';
    get_bytes_display(bufsize_text2, jm_countof(bufsize_text2), len2);
    bufsize_text3[0] = '\0';
    get_bytes_display(bufsize_text3, jm_countof(bufsize_text3), len3);

    printf("%-30s bytes = %5s, time = %0.5f ms.\n", "::strlen()",         bufsize_text1, time1);
    printf("%-30s bytes = %5s, time = %0.5f ms.\n", "asmlib::A_strlen()", bufsize_text2, time2);
    printf("%-30s bytes = %5s, time = %0.5f ms.\n", "jimi::jmf_strlen()", bufsize_text3, time3);
    printf("\n");

    printf("jimi::jmf_strlen() use time is ::strlen()         use time : %0.3f %%.\n", time3 / time1 * 100.0);
    printf("jimi::jmf_strlen() use time is asmlib::A_strlen() use time : %0.3f %%.\n", time3 / time2 * 100.0);
    printf("\n");

    if (str1) ::free(str1);
    if (str2) ::free(str2);
    if (str3) ::free(str3);
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
    stop_watch sw;
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
    stop_watch sw;
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
    stop_watch sw;
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
    stop_watch sw;
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

    char *szAnsi_Test1 = "23123213我们都是中国人, 我们都是地球人fkjadhhfkljdhfkljdhfkldhklfhdkjafhkldjhfkjadhfkjhdakjfhdkajhfkldash"
        "英雄所见略同, 不经历风雨怎么见彩虹???jfkljdl4348972949384iyjdhfjdgjfg3h4eih3k19283变形金刚4, 变形金刚四⑦⑦";
    wchar_t *szUtf16_Test1 = L"23123213我们都是中国人, 我们都是地球人fkjadhhfkljdhfkljdhfkldhklfhdkjafhkldjhfkjadhfkjhdakjfhdkajhfkldash"
        L"英雄所见略同, 不经历风雨怎么见彩虹???jfkljdl4348972949384iyjdhfjdgjfg3h4eih3k19283变形金刚4, 变形金刚四⑦⑦";

    printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\n");

    win_iconv_ansi2unicode_test(szAnsi_Test1, echo_result);
    //::system("pause");

    win_iconv_unicode2ansi_test(szUtf16_Test1, echo_result);
    //::system("pause");

    char *szAnsi_Test2 = "我们都是中国人, 我们都是地球人";
    wchar_t *szUtf16_Test2 = L"我们都是中国人, 我们都是地球人";

    printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\n");

    win_iconv_ansi2unicode_test(szAnsi_Test2, echo_result);
    //::system("pause");

    win_iconv_unicode2ansi_test(szUtf16_Test2, echo_result);

    printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\n");
    return true;
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

    Console.ReadKey(true, false);
}

class resA
{
public:
    resA()  { std::cout << "resA"  << std::endl; }
    ~resA() { std::cout << "~resA" << std::endl; }
};

class resB
{
public:
    resB()  { std::cout << "resB"  << std::endl; }
    ~resB() { std::cout << "~resB" << std::endl; }
};

template<class T>
class A
{
public:
    A()  { std::cout << "A" << std::endl; }
    ~A() {
        std::cout << "~A" << std::endl;
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
    B() : hasDisposed_(false) { std::cout << "B" << std::endl; }
    ~B() { std::cout << "~B" << std::endl; }
    bool hasDisposed() { return hasDisposed_; };
    void setDisposed() { hasDisposed_ = true; };

private:
    bool hasDisposed_;
    resB resB;
};

void template_inherit_test()
{
    A<B>* a = new B();
    a->Destroy();
    //delete a;
}

int UnitTest_Main(int argc, char *argv[])
{
    // 设置CRTDBG的环境(Debug模式下, 检查内存越界和内存泄漏问题)
    set_crtdbg_env();

    sLog.log_begin();

    ///*
    jimi::util::CommandLine cmdLine;
    int cnt;
    if ((cnt = cmdLine.parse(argc, argv)) >= 0) {
        std::string strCmdLine = cmdLine.getCmdLine();
        sLog.info(strCmdLine.c_str());
    }
    //*/

#if 0
    if (true) {
        template_inherit_test();
        ::system("pause");
        sLog.log_end();
        return 0;
    }
#endif

    // CPU 唤醒/预热 500毫秒
    jimi_cpu_warmup(500);

#if 0
    malloc_addr_test();

    ::system("pause");
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
        ::system("pause");
        sLog.log_end();
        return 0;
    }
#endif

#if 0
    bool result = win_iconv_test();
    if (result && 0) {
        ::system("pause");
        sLog.log_end();
        return 0;
    }
#endif

#if 0
    IEEE754_Double_Test();
#endif

    // 测试std::string是否使用了COW(Copy On Write)
    //String_Copy_On_Write_Test();

    // Memcpy 内存复制测试
    //Memcpy_Test();

#if 1
    String_Base_Test();
    if (true && 1) {
        ::system("pause");
        sLog.log_end();
        return 0;
    }
#endif

#if 1
    String_Performance_Test();
    String_Performance_Test2();
#endif

#if 1
    Fast_StrLen_Test();
#endif

    //::system("pause");

    Jm_StrLwr_Verify();

#if 1
    StrLwr_Test(4);
    StrLwr_Test(8);
    StrLwr_Test(10);
    StrLwr_Test(12);
    StrLwr_Test(16);
    StrLwr_Test(24);
    StrLwr_Test(32);
    StrLwr_Test(64);
    StrLwr_Test(128);
    StrLwr_Test(256);
    StrLwr_Test(1024);
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

    ///*
    //system::mutex read_mutex;
    //system::scoped_lock<system::mutex> lock(read_mutex);
    system::mutex read_mutex;
    system::scoped_lock<mutex> lock(read_mutex);
    lock.acquire(read_mutex);
    lock.try_acquire(read_mutex, 4000);
    lock.release();
    //*/

    sLog.log_end();

    //::system("pause");
    //Console::ReadKey();
    Console.ReadKey();
    return 0;
}

NS_UNITEST_END

NS_JIMI_BEGIN

class MyProgram : public jimi::Program
{
public:
    void Main(int argc, char *argv[]) {
        UnitTest_Main(argc, argv);
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
    program.SetMain(&UnitTest_Main);
    program.SetReturnValue(1);
#endif
    int nResult = program.Run(argc, argv);
    return nResult;
}

#if defined(_MSC_VER) && (_MSC_VER != 0)
#pragma warning(pop)                    // 恢复warning状态
#endif
