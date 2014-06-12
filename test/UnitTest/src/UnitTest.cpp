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

#include <jimi/log/Logger.h>
#include <jimi/system/stop_watch.h>

#include <asmlib/asmlib.h>
#include "SampleThread.h"
#include "FastMemcpy.h"

#include <jimic/platform/win/fast_memcpy.h>
#include <jimic/string/jmf_strings.h>

#include <string>

/**
 * for asmlib
 */
#pragma comment(lib, "libacof32.lib")

/* 基于CRT的内存泄漏检测 */
#if USE_CRTDBG_CHECK

//
// C++中基于CRT的内存泄漏检测
//
// Reference: http://www.cnblogs.com/weiym/archive/2013/02/25/2932810.html
//

#ifdef _DEBUG
  #define DEBUG_CLIENTBLOCK  new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
  #define DEBUG_CLIENTBLOCK
#endif

// 如果不定义这个宏, C方式的malloc泄露不会被记录下来
#define _CRTDBG_MAP_ALLOC

// crtdbg.h must be behind the stdlib.h
#include <crtdbg.h>

#ifdef _DEBUG
  #define new DEBUG_CLIENTBLOCK
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
// 如果你没有安装vld(Visual Leak Detector), 请注释掉这一句.
#include <vld.h>
#endif  /* _DEBUG */
#endif  /* _MSC_VER */

using namespace std;

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

    // 设置 CRT 报告模式
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);

    // 如果已经引用vld.h, 则不显示crtdbg的内存泄漏信息
#ifndef VLD_RPTHOOK_INSTALL
    // 进程退出时, 显示内存泄漏信息
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif  /* VLD_RPTHOOK_INSTALL */

#endif  /* USE_CRTDBG_CHECK */
}

/* 预热时间至少要大于500毫秒, 如果还不够, 可以自行增加最小预热时间 */

void jimi_cpu_warmup()
{
#ifndef _DEBUG
    stop_watch sw;
    volatile int sum = 0;
    double elapsedTime = 0.0;
    printf("CPU warm up start ...\n");
    do {
        sw.restart();
        // 如果有聪明的编译器能发现这是一个固定值就牛比了, 应该没有
        for (int i = 0; i < 10000; ++i) {
            sum += i;
            // 循环顺序故意颠倒过来的
            for (int j = 5000; j >= 0; --j) {
                sum -= j;
            }
        }
        sw.stop();
        elapsedTime += sw.getMillisec();
    } while (elapsedTime < 500.0);
    // 输出sum的值只是为了防止编译器把循环优化掉
    printf("sum = %u, time: %0.3f ms\n", sum, elapsedTime);
    printf("CPU warm up done ... \n\n");
#endif
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

/* String类的基础测试 */

void String_Base_Test()
{
    String str1 = "abcdefg";
    String str2 = "hijklmnop";
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
}

void String_Performance_Test()
{
#ifndef _DEBUG
    const int LOOP_TIMES = 1000000;
#else
    const int LOOP_TIMES = 200000;
#endif
    //char *str1, *str2;
    size_t len1, len2;
    char buffer1[32];
    char buffer2[32];
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
        strcpy_s(buffer2, 32, "abcdefghijk");
        len2 = strlen("abcdefghijk");
    }
    sw.stop();
    time2 = sw.getMillisec();

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        //jimi::String str = "abcdefghijk";
        jimi::String str("abcdefghijk", sizeof("abcdefghijk") - 1);
    }
    sw.stop();
    time3 = sw.getMillisec();
    
    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        //std::string str = "abcdefghijk";
        std::string str("abcdefghijk", sizeof("abcdefghijk") - 1);
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
        len1 = jmf_strlen(buffer1);
        //len1 = wcslen((wchar_t *)buffer1);
    }
    sw.stop();
    time5 = sw.getMillisec();

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        strcpy_s(buffer2, 32, "abcdefghijklmnopqrstuvwxyz");
        len2 = strlen(buffer2);
    }
    sw.stop();
    time6 = sw.getMillisec();

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        //jimi::String str = "abcdefghijklmnopqrstuvwxyz";
        jimi::String str("abcdefghijklmnopqrstuvwxyz", sizeof("abcdefghijklmnopqrstuvwxyz") - 1);
    }
    sw.stop();
    time7 = sw.getMillisec();

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        //std::string str = "abcdefghijklmnopqrstuvwxyz";
        std::string str("abcdefghijklmnopqrstuvwxyz", sizeof("abcdefghijklmnopqrstuvwxyz") - 1);
    }
    sw.stop();
    time8 = sw.getMillisec();

    printf("buffer1 = %s, len1 = %d, len2 = %d\n\n", buffer1, len1, len2);

    printf("%-30s time = %0.5f ms.\n", "strcpy()     str = \"abcdefghijk\";",   time1);
    printf("%-30s time = %0.5f ms.\n", "strcpy_s()   str = \"abcdefghijk\";",   time2);
    printf("%-30s time = %0.5f ms.\n", "jimi::String str = \"abcdefghijk\";",   time3);
    printf("%-30s time = %0.5f ms.\n", "std::string  str = \"abcdefghijk\";",   time4);
    printf("\n");
    printf("%-30s time = %0.5f ms.\n", "strcpy()     str = \"abcdefg...xyz\";", time5);
    printf("%-30s time = %0.5f ms.\n", "strcpy_s()   str = \"abcdefg...xyz\";", time6);
    printf("%-30s time = %0.5f ms.\n", "jimi::String str = \"abcdefg...xyz\";", time7);
    printf("%-30s time = %0.5f ms.\n", "std::string  str = \"abcdefg...xyz\";", time8);

    printf("\n");    
}

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
    printf("%-16s size = %d K, time = %0.5f ms\n", "memcpy_mmx4()",  buf_size / 1024, time2);
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
    printf("%-16s size = %d K, time = %0.5f ms\n", "memcpy_mmx4()",  buf_size / 1024, time2);
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
    printf("%-16s size = %d M, time = %0.5f ms\n", "memcpy_mmx4()",  buf_size / (1024 * 1024), time2);
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
    printf("%-16s size = %d M, time = %0.5f ms\n", "memcpy_mmx4()",  buf_size / (1024 * 1024), time2);
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
            pstr = jimi::char_traits<char>::strncpy_u(pstr3, pstr1, str_len);
            pstr3[str_len] = '\0';
        }
        sw.stop();
        time2 = sw.getMillisec();

        sw.restart();
        for (i = 0; i < LOOP_TIMES; ++i) {
            pstr = jimi::char_traits<char>::strncpy_u2(pstr4, pstr1, str_len);
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

int UnitTest_Main(int argc, char *argv[])
{
    // 设置CRTDBG的环境(Debug模式下, 检查内存越界和内存泄漏问题)
    set_crtdbg_env();

    sLog.log_begin();

    util::CommandLine cmdLine;
    int cnt;
    if ((cnt = cmdLine.parse(argc, argv)) >= 0) {
        string &strCmdLine = cmdLine.getCmdLine();
        sLog.info(strCmdLine.c_str());
    }

    printf("\n");

    // CPU 唤醒/预热
    jimi_cpu_warmup();

    // 测试std::string是否使用了COW(Copy On Write)
    String_Copy_On_Write_Test();

    // Memcpy 内存复制测试
    //Memcpy_Test();

    String_Base_Test();

    String_Performance_Test();

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

    ::system("pause");    
    return 0;
}

NS_UNITEST_END

class JimiProgram
{
public:
    int Run(int argc, char *argv[])    { return 0; };
    int Run(int argc, wchar_t *argv[]) { return 0; };
};

int main(int argc, char *argv[])
{
    JimiProgram program;
    int nResult = program.Run(argc, argv);

    return UnitTest_Main(argc, argv);
}
