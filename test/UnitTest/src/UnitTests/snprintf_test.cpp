
#include "snprintf_test.h"

#include <jimi/core/jimi_def.h>
#include <jimi/system/stop_watch.h>

#include <jimic/string/jm_strings.h>
#include <jimic/string/sprintf.h>
#include <jimic/string/csharp_sprintf.h>

#include <iostream>
#include <sstream>
#include <iomanip>

void String_Snprintf_Test()
{
    char fmtbuf[256] = { 0 };
    int fmtlen;

    printf("******************************************************************************\n\n");
    printf("  String_Snprintf_Test()\n\n");
    printf("******************************************************************************\n\n");

    memset(fmtbuf, '?', jm_countof(fmtbuf));
    fmtlen = jmc_csharp_snprintf(fmtbuf, jm_countof(fmtbuf),
                                 "{0:%d}, {1:%0.3f}, {2:%5d}, {3:%08d}, {4:%-20.8s}, 0x{5:%08X}, {6{:%05d}, {025d:05d}",
                                 "%d, %f, %d, %d, %X, %d",
                                 999, 12.345, 8888, 99999, "abcdhgjhged", 123456);
    //fmtbuf[fmtlen] = '\0';
    printf("jmc_csharp_snprintf(buf, count,\n"
           "    \"{0:%%d}, {1:%%0.3f}, {2:%%5d}, {3:%%08d}, {4:%%-20.8s}, 0x{5:%%08X}, {6{:%%05d}, {025d:05d}\");\n\n");
    printf("buf = %s\n\n", fmtbuf);
    printf("len = %d, strlen() = %d\n", fmtlen, jm_strlen(fmtbuf));
    printf("\n");

    fmtbuf[0] = '\0';
    memset(fmtbuf, '?', jm_countof(fmtbuf));
    fmtlen = jmc_snprintf(fmtbuf, jm_countof(fmtbuf), jm_countof(fmtbuf) - 1,
                          "%d, %0.3f, %5d, %08d, %-020.8s, 0x%08X,\n%06.3d, %05.3d, %05.3d, %-06.3d, %-05.3d, %-05.3d, 05d",
                          777, 12.345, 8888, 99999, "abcdhgjhged", 123456, 1, 1234, 12345678, 1, 1234, 12345678);
    //fmtbuf[fmtlen] = '\0';
    printf("jmc_snprintf(buf, bufsize, count,\n"
           "    \"%%04d, %%0.3f, %%5d, %%08d, %%-20.8s, 0x%%08X,\n"
           "     %%06.3d, %%05.3d, %%05.3d, %%-06.3d, %%-05.3d, %%-05.3d, 05d\");\n\n");
    printf("buf = %s\n\n", fmtbuf);
    printf("len = %d, strlen() = %d\n", fmtlen, jm_strlen(fmtbuf));
    printf("\n");

    fmtbuf[0] = '\0';
    memset(fmtbuf, '?', jm_countof(fmtbuf));
    fmtlen = jm_snprintf(fmtbuf, jm_countof(fmtbuf), jm_countof(fmtbuf) - 1,
                         "%d, %0.3f, %5d, %08d, %-020.8s, 0x%08X,\n%06.3d, %05.3d, %05.3d, %-06.3d, %-05.3d, %-05.3d, 05d",
                         777, 12.345, 8888, 99999, "abcdhgjhged", 123456, 1, 1234, 12345678, 1, 1234, 12345678);
    //fmtbuf[fmtlen] = '\0';
    printf("jm_snprintf(buf, bufsize, count,\n"
           "    \"%%04d, %%0.3f, %%5d, %%08d, %%-020.8s, 0x%%08X,\n"
           "     %%06.3d, %%05.3d, %%05.3d, %%-06.3d, %%-05.3d, %%-05.3d, 05d\");\n\n");
    printf("buf = %s\n\n", fmtbuf);
    printf("len = %d, strlen() = %d\n", fmtlen, jm_strlen(fmtbuf));
    printf("\n");

    printf("\n");
    //::system("pause");
}

void Sprintf_Preformance_Test_Integer()
{
    int i;
    double time, timeReference;
    int loop_times = 9999999;
    jimi::stop_watch sw;

    char fmtbuf1[512] = { 0 };
    char fmtbuf2[512] = { 0 };
    int fmtlen;

#if !defined(VSNPRINTF_SHORT_DISPLAY) || (VSNPRINTF_SHORT_DISPLAY == 0)
    printf("******************************************************************************\n\n");
    printf("  Sprintf_Preformance_Test_Integer()\n\n");
    printf("******************************************************************************\n\n");
#else
    printf("==============================================================================\n\n");
    printf("  Sprintf_Preformance_Test_Integer()\n\n");
#endif

#ifndef _DEBUG
    loop_times = 999999;
#else
    loop_times = 9999;
#endif

    printf("==============================================================================\n\n");
    printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
    printf("        len = jmc_sprintf(buf, bufsize, count,\n"
           "                          \"%%d, %%d, %%d, %%d, %%d\\n\"\n"
           "                          \"%%d, %%d, %%d, %%d, %%d\",\n"
           "                           12,  1234,  123456,  12345678,  123456789,\n"
           "                          -12, -1234, -123456, -12345678, -123456789);\n");
    printf("    }\n\n");
    printf("==============================================================================\n\n");

#if defined(VSNPRINTF_TEST_JM_SNPRINTF) && (VSNPRINTF_TEST_JM_SNPRINTF != 0)
    {
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            fmtlen = jm_sprintf(fmtbuf1, jm_countof(fmtbuf1),
                                "%d, %d, %d, %d, %d\n"
                                "%d, %d, %d, %d, %d",
                                 12,  1234,  123456,  12345678,  123456789,
                                -12, -1234, -123456, -12345678, -123456789);
        }
        sw.stop();
        time = sw.getMillisec();

#if defined(VSNPRINTF_SHORT_DISPLAY) && (VSNPRINTF_SHORT_DISPLAY != 0)
        printf(">>> %-15s <<<  time = %-8.3f ms\n\n", "jm_sprintf()", time);
#else
        printf(">>> %-18s <<<\n\n", "jm_sprintf()");
        printf("result =\n%s\n\n", fmtbuf1);
        printf("len  = %d, strlen() = %d\n", fmtlen, jm_strlen(fmtbuf1));
        printf("time = %0.3f ms\n", time);
        printf("\n");
#endif // VSNPRINTF_SHORT_DISPLAY
    }
#endif // VSNPRINTF_TEST_JM_SNPRINTF

#if 1
    {
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            fmtlen = sprintf(fmtbuf1,
                             "%d, %d, %d, %d, %d\n"
                             "%d, %d, %d, %d, %d",
                              12,  1234,  123456,  12345678,  123456789,
                             -12, -1234, -123456, -12345678, -123456789);
        }
        sw.stop();
        time = sw.getMillisec();
        timeReference = time;

#if defined(VSNPRINTF_SHORT_DISPLAY) && (VSNPRINTF_SHORT_DISPLAY != 0)
        printf(">>> %-15s <<<  time = %-8.3f ms\n\n", "sprintf()", time);
#else
        printf(">>> %-18s <<<\n\n", "sprintf()");
        printf("result =\n%s\n\n", fmtbuf1);
        printf("len  = %d, strlen() = %d\n", fmtlen, jm_strlen(fmtbuf1));
        printf("time = %0.3f ms\n", time);
        printf("\n");
#endif // VSNPRINTF_SHORT_DISPLAY
    }
#endif

#if 1
    {
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            fmtlen = jmc_sprintf(fmtbuf2,
                                 "%d, %d, %d, %d, %d\n"
                                 "%d, %d, %d, %d, %d",
                                  12,  1234,  123456,  12345678,  123456789,
                                 -12, -1234, -123456, -12345678, -123456789);
        }
        sw.stop();
        time = sw.getMillisec();

#if defined(VSNPRINTF_SHORT_DISPLAY) && (VSNPRINTF_SHORT_DISPLAY != 0)
        printf(">>> %-15s <<<  time = %-8.3f ms\n\n", "jmc_sprintf()", time);
        printf("jmc_sprintf() preformance is sprintf() %0.3f X times.\n\n", timeReference / time);
#else
        printf(">>> %-18s <<<\n\n", "jmc_sprintf()");
        printf("result =\n%s\n\n", fmtbuf2);
        printf("len  = %d, strlen() = %d\n", fmtlen, jm_strlen(fmtbuf2));
        printf("time = %0.3f ms, preformance is sprintf(): %0.3f X (times)\n", time, timeReference / time);
        printf("\n");
#endif // VSNPRINTF_SHORT_DISPLAY
    }
#endif
    printf("\n");
}

void Snprintf_Preformance_Test_Integer1()
{
    int i;
    double time, timeReference;
    int loop_times = 9999999;
    jimi::stop_watch sw;

    char fmtbuf1[512] = { 0 };
    char fmtbuf2[512] = { 0 };
    int fmtlen;

#if !defined(VSNPRINTF_SHORT_DISPLAY) || (VSNPRINTF_SHORT_DISPLAY == 0)
    printf("******************************************************************************\n\n");
    printf("  Snprintf_Preformance_Test_Integer1()\n\n");
    printf("******************************************************************************\n\n");
#else
    printf("==============================================================================\n\n");
    printf("  Snprintf_Preformance_Test_Integer1()\n\n");
#endif

#ifndef _DEBUG
    loop_times = 999999;
#else
    loop_times = 9999;
#endif

    printf("==============================================================================\n\n");
    printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
    printf("        len = jmc_snprintf(buf, bufsize, count,\n"
           "                           \"%%d, %%d, %%d, %%d, %%d\\n\"\n"
           "                           \"%%d, %%d, %%d, %%d, %%d\",\n"
           "                            12,  1234,  123456,  12345678,  123456789,\n"
           "                           -12, -1234, -123456, -12345678, -123456789);\n");
    printf("    }\n\n");
    printf("==============================================================================\n\n");

#if defined(VSNPRINTF_TEST_JM_SNPRINTF) && (VSNPRINTF_TEST_JM_SNPRINTF != 0)
    {
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            fmtlen = jm_snprintf(fmtbuf1, jm_countof(fmtbuf1), jm_countof(fmtbuf1) - 1,
                                 "%d, %d, %d, %d, %d\n"
                                 "%d, %d, %d, %d, %d",
                                  12,  1234,  123456,  12345678,  123456789,
                                 -12, -1234, -123456, -12345678, -123456789);
        }
        sw.stop();
        time = sw.getMillisec();

#if defined(VSNPRINTF_SHORT_DISPLAY) && (VSNPRINTF_SHORT_DISPLAY != 0)
        printf(">>> %-15s <<<  time = %-8.3f ms\n\n", "jm_snprintf()", time);
#else
        printf(">>> %-18s <<<\n\n", "jm_snprintf()");
        printf("result =\n%s\n\n", fmtbuf1);
        printf("len  = %d, strlen() = %d\n", fmtlen, jm_strlen(fmtbuf1));
        printf("time = %0.3f ms\n", time);
        printf("\n");
#endif // VSNPRINTF_SHORT_DISPLAY
    }
#endif // VSNPRINTF_TEST_JM_SNPRINTF

#if 1
    {
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
#ifdef _MSC_VER
            fmtlen = _snprintf_s(fmtbuf1, jm_countof(fmtbuf1), jm_countof(fmtbuf1) - 1,
#else
            fmtlen =    snprintf(fmtbuf1, sizeof(fmtbuf1),
#endif // _MSC_VER
                                 "%d, %d, %d, %d, %d\n"
                                 "%d, %d, %d, %d, %d",
                                  12,  1234,  123456,  12345678,  123456789,
                                 -12, -1234, -123456, -12345678, -123456789);
        }
        sw.stop();
        time = sw.getMillisec();
        timeReference = time;

#if defined(VSNPRINTF_SHORT_DISPLAY) && (VSNPRINTF_SHORT_DISPLAY != 0)
  #ifdef _MSC_VER
        printf(">>> %-15s <<<  time = %-8.3f ms\n\n", "_snprintf_s()", time);
  #else
        printf(">>> %-15s <<<  time = %-8.3f ms\n\n", "snprintf()", time);
  #endif // _MSC_VER
#else
  #ifdef _MSC_VER
        printf(">>> %-18s <<<\n\n", "_snprintf_s()");
  #else
        printf(">>> %-18s <<<\n\n", "snprintf()");
  #endif // _MSC_VER
        printf("result = \n%s\n\n", fmtbuf1);
        printf("len  = %d, strlen() = %d\n", fmtlen, jm_strlen(fmtbuf1));
        printf("time = %0.3f ms\n", time);
        printf("\n");
#endif // VSNPRINTF_SHORT_DISPLAY
    }
#endif

#if 1
    {
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            fmtlen = jmc_snprintf(fmtbuf2, jm_countof(fmtbuf2), jm_countof(fmtbuf2) - 1,
                                  "%d, %d, %d, %d, %d\n"
                                  "%d, %d, %d, %d, %d",
                                   12,  1234,  123456,  12345678,  123456789,
                                  -12, -1234, -123456, -12345678, -123456789);
        }
        sw.stop();
        time = sw.getMillisec();

#if defined(VSNPRINTF_SHORT_DISPLAY) && (VSNPRINTF_SHORT_DISPLAY != 0)
        printf(">>> %-15s <<<  time = %-8.3f ms\n\n", "jmc_snprintf()", time);
  #ifdef _MSC_VER
        printf("jmc_sprintf() preformance is _snprintf_s() %0.3f X times.\n\n", timeReference / time);
  #else
        printf("jmc_sprintf() preformance is snprintf() %0.3f X times.\n\n", timeReference / time);
  #endif // _MSC_VER
#else
        printf(">>> %-18s <<<\n\n", "jmc_snprintf()");
        printf("result = \n%s\n\n", fmtbuf2);
        printf("len  = %d, strlen() = %d\n", fmtlen, jm_strlen(fmtbuf2));
  #ifdef _MSC_VER
        printf("time = %0.3f ms, preformance is _snprintf_s(): %0.3f X (times)\n", time, timeReference / time);
  #else
        printf("time = %0.3f ms, preformance is snprintf(): %0.3f X (times)\n", time, timeReference / time);
  #endif // _MSC_VER
        printf("\n");
#endif // VSNPRINTF_SHORT_DISPLAY
    }
#endif
    printf("\n");
}

void Snprintf_Preformance_Test_Integer2()
{
    int i;
    double time, timeReference;
    int loop_times = 9999999;
    jimi::stop_watch sw;

    char fmtbuf1[512] = { 0 };
    char fmtbuf2[512] = { 0 };
    int fmtlen;

#if !defined(VSNPRINTF_SHORT_DISPLAY) || (VSNPRINTF_SHORT_DISPLAY == 0)
    printf("******************************************************************************\n\n");
    printf("  Snprintf_Preformance_Test_Integer2()\n\n");
    printf("******************************************************************************\n\n");
#else
    printf("==============================================================================\n\n");
    printf("  Snprintf_Preformance_Test_Integer2()\n\n");
#endif

#ifndef _DEBUG
    loop_times = 999999;
#else
    loop_times = 9999;
#endif

    printf("==============================================================================\n\n");
    printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
    printf("        len = jmc_snprintf(buf, bufsize, count,\n"
           "                           \"%%d, %%d, %%d, %%d, %%04d, %%5d, %%08d\\n\"\n"
           "                           \"%%06.3d, %%05.3d, %%05.3d, %%-06.3d, %%-05.3d, %%-05.3d, 05d\",\n"
           "                           123, 123456789, -123, -123456789, 777, 8888, 99999,\n"
           "                           1, 1234, 12345678, 1, 1234, 12345678);\n");
    printf("    }\n\n");
    printf("==============================================================================\n\n");

#if defined(VSNPRINTF_TEST_JM_SNPRINTF) && (VSNPRINTF_TEST_JM_SNPRINTF != 0)
    {
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            fmtlen = jm_snprintf(fmtbuf1, jm_countof(fmtbuf1), jm_countof(fmtbuf1) - 1,
                                 "%d, %d, %d, %d, %04d, %5d, %08d\n"
                                 "%06.3d, %05.3d, %05.3d, %-06.3d, %-05.3d, %-05.3d, 05d",
                                 123, 123456789, -123, -123456789, 777, 8888, 99999,
                                 1, 1234, 12345678, 1, 1234, 12345678);
        }
        sw.stop();
        time = sw.getMillisec();

#if defined(VSNPRINTF_SHORT_DISPLAY) && (VSNPRINTF_SHORT_DISPLAY != 0)
        printf(">>> %-15s <<<  time = %-8.3f ms\n\n", "jm_snprintf()", time);
#else
        printf(">>> %-18s <<<\n\n", "jm_snprintf()");
        printf("result =\n%s\n\n", fmtbuf1);
        printf("len  = %d, strlen() = %d\n", fmtlen, jm_strlen(fmtbuf1));
        printf("time = %0.3f ms\n", time);
        printf("\n");
#endif // VSNPRINTF_SHORT_DISPLAY
    }
#endif // VSNPRINTF_TEST_JM_SNPRINTF

#if 1
    {
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
#ifdef _MSC_VER
            fmtlen = _snprintf_s(fmtbuf1, jm_countof(fmtbuf1), jm_countof(fmtbuf1) - 1,
#else
            fmtlen =    snprintf(fmtbuf1, sizeof(fmtbuf1),
#endif // _MSC_VER
                                 "%d, %d, %d, %d, %04d, %5d, %08d\n"
                                 "%06.3d, %05.3d, %05.3d, %-06.3d, %-05.3d, %-05.3d, 05d",
                                 123, 123456789, -123, -123456789, 777, 8888, 99999,
                                 1, 1234, 12345678, 1, 1234, 12345678);
        }
        sw.stop();
        time = sw.getMillisec();
        timeReference = time;

#if defined(VSNPRINTF_SHORT_DISPLAY) && (VSNPRINTF_SHORT_DISPLAY != 0)
  #ifdef _MSC_VER
        printf(">>> %-15s <<<  time = %-8.3f ms\n\n", "_snprintf_s()", time);
  #else
        printf(">>> %-15s <<<  time = %-8.3f ms\n\n", "snprintf()", time);
  #endif // _MSC_VER
#else
  #ifdef _MSC_VER
        printf(">>> %-18s <<<\n\n", "_snprintf_s()");
  #else
        printf(">>> %-18s <<<\n\n", "snprintf()");
  #endif // _MSC_VER
        printf("result = \n%s\n\n", fmtbuf1);
        printf("len  = %d, strlen() = %d\n", fmtlen, jm_strlen(fmtbuf1));
        printf("time = %0.3f ms\n", time);
        printf("\n");
#endif
    }
#endif

#if 1
    {
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            fmtlen = jmc_snprintf(fmtbuf2, jm_countof(fmtbuf2), jm_countof(fmtbuf2) - 1,
                                  "%d, %d, %d, %d, %04d, %5d, %08d\n"
                                  "%06.3d, %05.3d, %05.3d, %-06.3d, %-05.3d, %-05.3d, 05d",
                                  123, 123456789, -123, -123456789, 777, 8888, 99999,
                                  1, 1234, 12345678, 1, 1234, 12345678);
        }
        sw.stop();
        time = sw.getMillisec();

#if defined(VSNPRINTF_SHORT_DISPLAY) && (VSNPRINTF_SHORT_DISPLAY != 0)
        printf(">>> %-15s <<<  time = %-8.3f ms\n\n", "jmc_snprintf()", time);
  #ifdef _MSC_VER
        printf("jmc_sprintf() preformance is _snprintf_s() %0.3f X times.\n\n", timeReference / time);
  #else
        printf("jmc_sprintf() preformance is snprintf() %0.3f X times.\n\n", timeReference / time);
  #endif // _MSC_VER
#else
        printf(">>> %-18s <<<\n\n", "jmc_snprintf()");
        printf("result = \n%s\n\n", fmtbuf2);
        printf("len  = %d, strlen() = %d\n", fmtlen, jm_strlen(fmtbuf2));
  #ifdef _MSC_VER
        printf("time = %0.3f ms, preformance is _snprintf_s(): %0.3f X (times)\n", time, timeReference / time);
  #else
        printf("time = %0.3f ms, preformance is snprintf(): %0.3f X (times)\n", time, timeReference / time);
  #endif // _MSC_VER
        printf("\n");
#endif // VSNPRINTF_SHORT_DISPLAY
    }
#endif
    printf("\n");
}

void Snprintf_Preformance_Test_Integer3()
{
    int i;
    double time, timeReference;
    int loop_times = 9999999;
    jimi::stop_watch sw;

    char fmtbuf1[512] = { 0 };
    char fmtbuf2[512] = { 0 };
    int fmtlen;

#if !defined(VSNPRINTF_SHORT_DISPLAY) || (VSNPRINTF_SHORT_DISPLAY == 0)
    printf("******************************************************************************\n\n");
    printf("  Snprintf_Preformance_Test_Integer3()\n\n");
    printf("******************************************************************************\n\n");
#else
    printf("==============================================================================\n\n");
    printf("  Snprintf_Preformance_Test_Integer3()\n\n");
#endif

#ifndef _DEBUG
    loop_times = 499999;
#else
    loop_times = 4999;
#endif

    printf("==============================================================================\n\n");
    printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
    printf("        len = jmc_snprintf(buf, bufsize, count,\n"
           "                           \"%%d, %%d, %%d, %%d, %%04d, %%5d, %%08d\\n\"\n"
           "                           \"%%06.3d, %%05.3d, %%05.3d, %%-06.3d, %%-05.3d, %%-05.3d, 05d\\n\"\n"
           "                           \"%%06.3d, %%05.3d, %%05.3d, %%-06.3d, %%-05.3d, %%-05.3d, 05d\",\n"
           "                           123, 123456789, -123, -123456789, 777, 8888, 99999,\n"
           "                            1,  1234,  12345678,  1,  1234,  12345678,\n"
           "                           -1, -1234, -12345678, -1, -1234, -12345678);\n");
    printf("    }\n\n");
    printf("==============================================================================\n\n");

#if defined(VSNPRINTF_TEST_JM_SNPRINTF) && (VSNPRINTF_TEST_JM_SNPRINTF != 0)
    {
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            fmtlen = jm_snprintf(fmtbuf1, jm_countof(fmtbuf1), jm_countof(fmtbuf1) - 1,
                                 "%d, %d, %d, %d, %04d, %5d, %08d\n"
                                 "%06.3d, %05.3d, %05.3d, %-06.3d, %-05.3d, %-05.3d, 05d\n"
                                 "%06.3d, %05.3d, %05.3d, %-06.3d, %-05.3d, %-05.3d, 05d",
                                 123, 123456789, -123, -123456789, 777, 8888, 99999,
                                  1,  1234,  12345678,  1,  1234,  12345678,
                                 -1, -1234, -12345678, -1, -1234, -12345678);
        }
        sw.stop();
        time = sw.getMillisec();

#if defined(VSNPRINTF_SHORT_DISPLAY) && (VSNPRINTF_SHORT_DISPLAY != 0)
        printf(">>> %-15s <<<  time = %-8.3f ms\n\n", "jm_snprintf()", time);
#else
        printf(">>> %-18s <<<\n\n", "jm_snprintf()");
        printf("result =\n%s\n\n", fmtbuf1);
        printf("len  = %d, strlen() = %d\n", fmtlen, jm_strlen(fmtbuf1));
        printf("time = %0.3f ms\n", time);
        printf("\n");
#endif // VSNPRINTF_SHORT_DISPLAY
    }
#endif // VSNPRINTF_TEST_JM_SNPRINTF

#if 1
    {
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
#ifdef _MSC_VER
            fmtlen = _snprintf_s(fmtbuf1, jm_countof(fmtbuf1), jm_countof(fmtbuf1) - 1,
#else
            fmtlen =    snprintf(fmtbuf1, sizeof(fmtbuf1),
#endif // _MSC_VER
                                 "%d, %d, %d, %d, %04d, %5d, %08d\n"
                                 "%06.3d, %05.3d, %05.3d, %-06.3d, %-05.3d, %-05.3d, 05d\n"
                                 "%06.3d, %05.3d, %05.3d, %-06.3d, %-05.3d, %-05.3d, 05d",
                                 123, 123456789, -123, -123456789, 777, 8888, 99999,
                                  1,  1234,  12345678,  1,  1234,  12345678,
                                 -1, -1234, -12345678, -1, -1234, -12345678);
        }
        sw.stop();
        time = sw.getMillisec();
        timeReference = time;

#if defined(VSNPRINTF_SHORT_DISPLAY) && (VSNPRINTF_SHORT_DISPLAY != 0)
  #ifdef _MSC_VER
        printf(">>> %-15s <<<  time = %-8.3f ms\n\n", "_snprintf_s()", time);
  #else
        printf(">>> %-15s <<<  time = %-8.3f ms\n\n", "snprintf()", time);
  #endif // _MSC_VER
#else
  #ifdef _MSC_VER
        printf(">>> %-18s <<<\n\n", "_snprintf_s()");
  #else
        printf(">>> %-18s <<<\n\n", "snprintf()");
  #endif // _MSC_VER
        printf("result = \n%s\n\n", fmtbuf1);
        printf("len  = %d, strlen() = %d\n", fmtlen, jm_strlen(fmtbuf1));
        printf("time = %0.3f ms\n", time);
        printf("\n");
#endif // VSNPRINTF_SHORT_DISPLAY
    }
#endif

#if 1
    {
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            fmtlen = jmc_snprintf(fmtbuf2, jm_countof(fmtbuf2), jm_countof(fmtbuf2) - 1,
                                  "%d, %d, %d, %d, %04d, %5d, %08d\n"
                                  "%06.3d, %05.3d, %05.3d, %-06.3d, %-05.3d, %-05.3d, 05d\n"
                                  "%06.3d, %05.3d, %05.3d, %-06.3d, %-05.3d, %-05.3d, 05d",
                                  123, 123456789, -123, -123456789, 777, 8888, 99999,
                                   1,  1234,  12345678,  1,  1234,  12345678,
                                  -1, -1234, -12345678, -1, -1234, -12345678);
        }
        sw.stop();
        time = sw.getMillisec();

#if defined(VSNPRINTF_SHORT_DISPLAY) && (VSNPRINTF_SHORT_DISPLAY != 0)
        printf(">>> %-15s <<<  time = %-8.3f ms\n\n", "jmc_snprintf()", time);
  #ifdef _MSC_VER
        printf("jmc_sprintf() preformance is _snprintf_s() %0.3f X times.\n\n", timeReference / time);
  #else
        printf("jmc_sprintf() preformance is snprintf() %0.3f X times.\n\n", timeReference / time);
  #endif // _MSC_VER
#else
        printf(">>> %-18s <<<\n\n", "jmc_snprintf()");
        printf("result = \n%s\n\n", fmtbuf2);
        printf("len  = %d, strlen() = %d\n", fmtlen, jm_strlen(fmtbuf2));
  #ifdef _MSC_VER
        printf("time = %0.3f ms, preformance is _snprintf_s(): %0.3f X (times)\n", time, timeReference / time);
  #else
        printf("time = %0.3f ms, preformance is snprintf(): %0.3f X (times)\n", time, timeReference / time);
  #endif // _MSC_VER
        printf("\n");
#endif // VSNPRINTF_SHORT_DISPLAY
    }
#endif
    printf("\n");
}

void Snprintf_Preformance_Test_Double1()
{
    int i;
    double time, timeReference;
    int loop_times = 9999999;
    jimi::stop_watch sw;

    char fmtbuf1[512] = { 0 };
    char fmtbuf2[512] = { 0 };
    int fmtlen;

#if !defined(VSNPRINTF_SHORT_DISPLAY) || (VSNPRINTF_SHORT_DISPLAY == 0)
    printf("******************************************************************************\n\n");
    printf("  Snprintf_Preformance_Test_Double1()\n\n");
    printf("******************************************************************************\n\n");
#else
    printf("==============================================================================\n\n");
    printf("  Snprintf_Preformance_Test_Double1()\n\n");
#endif

#ifndef _DEBUG
    loop_times = 499999;
#else
    loop_times = 4999;
#endif

    printf("==============================================================================\n\n");
    printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
    printf("        len = jmc_snprintf(buf, bufsize, count,\n"
           "                           \"%%f, %%f, %%f, %%f, %%f,\\n\"\n"
           "                           \"%%f, %%f, %%d, %%f, %%f\",\n"
           "                            12.345,  1234.5678,  123456.789,\n"
           "                            12345678.901,  1234567890.123456,\n"
           "                           -12.345, -1234.5678, -123456.789,\n"
           "                           -12345678.901, -1234567890.123456);\n");
    printf("    }\n\n");
    printf("==============================================================================\n\n");

#if defined(VSNPRINTF_TEST_JM_SNPRINTF) && (VSNPRINTF_TEST_JM_SNPRINTF != 0)
    {
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            fmtlen = jm_snprintf(fmtbuf1, jm_countof(fmtbuf1), jm_countof(fmtbuf1) - 1,
                                 "%f, %f, %f, %f, %f\n"
                                 "%f, %f, %f, %f, %f",
                                  12.345,  1234.5678,  123456.789,  12345678.901,  1234567890.123456,
                                 -12.345, -1234.5678, -123456.789, -12345678.901, -1234567890.123456);
        }
        sw.stop();
        time = sw.getMillisec();

#if defined(VSNPRINTF_SHORT_DISPLAY) && (VSNPRINTF_SHORT_DISPLAY != 0)
        printf(">>> %-15s <<<  time = %-8.3f ms\n\n", "jm_snprintf()", time);
#else
        printf(">>> %-18s <<<\n\n", "jm_snprintf()");
        printf("result =\n%s\n\n", fmtbuf1);
        printf("len  = %d, strlen() = %d\n", fmtlen, jm_strlen(fmtbuf1));
        printf("time = %0.3f ms\n", time);
        printf("\n");
#endif // VSNPRINTF_SHORT_DISPLAY
    }
#endif // VSNPRINTF_TEST_JM_SNPRINTF

#if 1
    {
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
#ifdef _MSC_VER
            fmtlen = _snprintf_s(fmtbuf1, jm_countof(fmtbuf1), jm_countof(fmtbuf1) - 1,
#else
            fmtlen =    snprintf(fmtbuf1, sizeof(fmtbuf1),
#endif // _MSC_VER
                                 "%f, %f, %f, %f, %f\n"
                                 "%f, %f, %f, %f, %f",
                                  12.345,  1234.5678,  123456.789,  12345678.901,  1234567890.123456,
                                 -12.345, -1234.5678, -123456.789, -12345678.901, -1234567890.123456);
        }
        sw.stop();
        time = sw.getMillisec();
        timeReference = time;

#if defined(VSNPRINTF_SHORT_DISPLAY) && (VSNPRINTF_SHORT_DISPLAY != 0)
  #ifdef _MSC_VER
        printf(">>> %-15s <<<  time = %-8.3f ms\n\n", "_snprintf_s()", time);
  #else
        printf(">>> %-15s <<<  time = %-8.3f ms\n\n", "snprintf()", time);
  #endif // _MSC_VER
#else
  #ifdef _MSC_VER
        printf(">>> %-18s <<<\n\n", "_snprintf_s()");
  #else
        printf(">>> %-18s <<<\n\n", "snprintf()");
  #endif // _MSC_VER
        printf("result = \n%s\n\n", fmtbuf1);
        printf("len  = %d, strlen() = %d\n", fmtlen, jm_strlen(fmtbuf1));
        printf("time = %0.3f ms\n", time);
        printf("\n");
#endif // VSNPRINTF_SHORT_DISPLAY
    }
#endif

#if 1
    {
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            fmtlen = jmc_snprintf(fmtbuf2, jm_countof(fmtbuf2), jm_countof(fmtbuf2) - 1,
                                  "%f, %f, %f, %f, %f\n"
                                  "%f, %f, %f, %f, %f",
                                   12.345,  1234.5678,  123456.789,  12345678.901,  1234567890.123456,
                                  -12.345, -1234.5678, -123456.789, -12345678.901, -1234567890.123456);
        }
        sw.stop();
        time = sw.getMillisec();

#if defined(VSNPRINTF_SHORT_DISPLAY) && (VSNPRINTF_SHORT_DISPLAY != 0)
        printf(">>> %-15s <<<  time = %-8.3f ms\n\n", "jmc_snprintf()", time);
  #ifdef _MSC_VER
        printf("jmc_sprintf() preformance is _snprintf_s() %0.3f X times.\n\n", timeReference / time);
  #else
        printf("jmc_sprintf() preformance is snprintf() %0.3f X times.\n\n", timeReference / time);
  #endif // _MSC_VER
#else
        printf(">>> %-18s <<<\n\n", "jmc_snprintf()");
        printf("result = \n%s\n\n", fmtbuf2);
        printf("len  = %d, strlen() = %d\n", fmtlen, jm_strlen(fmtbuf2));
  #ifdef _MSC_VER
        printf("time = %0.3f ms, preformance is _snprintf_s(): %0.3f X (times)\n", time, timeReference / time);
  #else
        printf("time = %0.3f ms, preformance is snprintf(): %0.3f X (times)\n", time, timeReference / time);
  #endif // _MSC_VER
        printf("\n");
#endif // VSNPRINTF_SHORT_DISPLAY
    }
#endif
    printf("\n");
}

void Snprintf_Preformance_Test_Double2()
{
    int i;
    double time, timeReference;
    int loop_times = 9999999;
    jimi::stop_watch sw;

    char fmtbuf1[512] = { 0 };
    char fmtbuf2[512] = { 0 };
    int fmtlen;

#if !defined(VSNPRINTF_SHORT_DISPLAY) || (VSNPRINTF_SHORT_DISPLAY == 0)
    printf("******************************************************************************\n\n");
    printf("  Snprintf_Preformance_Test_Double2()\n\n");
    printf("******************************************************************************\n\n");
#else
    printf("==============================================================================\n\n");
    printf("  Snprintf_Preformance_Test_Double2()\n\n");
#endif

#ifndef _DEBUG
    loop_times = 499999;
#else
    loop_times = 4999;
#endif

    printf("==============================================================================\n\n");
    printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
    printf("        len = jmc_snprintf(buf, bufsize, count,\n"
           "                           \"%%8.6f, %%8.6f, %%8.6f, %%8.6f, %%8.6f,\\n\"\n"
           "                           \"%%8.6f, %%8.6f, %%8.6f, %%8.6f, %%8.6f\",\n"
           "                            12.345,  1234.5678,  123456.789,\n"
           "                            12345678.901,  1234567890.123456,\n"
           "                           -12.345, -1234.5678, -123456.789,\n"
           "                           -12345678.901, -1234567890.123456);\n");
    printf("    }\n\n");
    printf("==============================================================================\n\n");

#if defined(VSNPRINTF_TEST_JM_SNPRINTF) && (VSNPRINTF_TEST_JM_SNPRINTF != 0)
    {
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            fmtlen = jm_snprintf(fmtbuf1, jm_countof(fmtbuf1), jm_countof(fmtbuf1) - 1,
                                 "%8.6f, %8.6f, %8.6f, %8.6f, %8.6f\n"
                                 "%8.6f, %8.6f, %8.6f, %8.6f, %8.6f",
                                  12.345,  1234.5678,  123456.789,  12345678.901,  1234567890.123456,
                                 -12.345, -1234.5678, -123456.789, -12345678.901, -1234567890.123456);
        }
        sw.stop();
        time = sw.getMillisec();

#if defined(VSNPRINTF_SHORT_DISPLAY) && (VSNPRINTF_SHORT_DISPLAY != 0)
        printf(">>> %-15s <<<  time = %-8.3f ms\n\n", "jm_snprintf()", time);
#else
        printf(">>> %-18s <<<\n\n", "jm_snprintf()");
        printf("result =\n%s\n\n", fmtbuf1);
        printf("len  = %d, strlen() = %d\n", fmtlen, jm_strlen(fmtbuf1));
        printf("time = %0.3f ms\n", time);
        printf("\n");
#endif // VSNPRINTF_SHORT_DISPLAY
    }
#endif // VSNPRINTF_TEST_JM_SNPRINTF

#if 1
    {
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
#ifdef _MSC_VER
            fmtlen = _snprintf_s(fmtbuf1, jm_countof(fmtbuf1), jm_countof(fmtbuf1) - 1,
#else
            fmtlen =    snprintf(fmtbuf1, sizeof(fmtbuf1),
#endif // _MSC_VER
                                 "%8.6f, %8.6f, %8.6f, %8.6f, %8.6f\n"
                                 "%8.6f, %8.6f, %8.6f, %8.6f, %8.6f",
                                  12.345,  1234.5678,  123456.789,  12345678.901,  1234567890.123456,
                                 -12.345, -1234.5678, -123456.789, -12345678.901, -1234567890.123456);
        }
        sw.stop();
        time = sw.getMillisec();
        timeReference = time;

#if defined(VSNPRINTF_SHORT_DISPLAY) && (VSNPRINTF_SHORT_DISPLAY != 0)
  #ifdef _MSC_VER
        printf(">>> %-15s <<<  time = %-8.3f ms\n\n", "_snprintf_s()", time);
  #else
        printf(">>> %-15s <<<  time = %-8.3f ms\n\n", "snprintf()", time);
  #endif // _MSC_VER
#else
  #ifdef _MSC_VER
        printf(">>> %-18s <<<\n\n", "_snprintf_s()");
  #else
        printf(">>> %-18s <<<\n\n", "snprintf()");
  #endif // _MSC_VER
        printf("result = \n%s\n\n", fmtbuf1);
        printf("len  = %d, strlen() = %d\n", fmtlen, jm_strlen(fmtbuf1));
        printf("time = %0.3f ms\n", time);
        printf("\n");
#endif // VSNPRINTF_SHORT_DISPLAY
    }
#endif

#if 1
    {
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            fmtlen = jmc_snprintf(fmtbuf2, jm_countof(fmtbuf2), jm_countof(fmtbuf2) - 1,
                                  "%8.6f, %8.6f, %8.6f, %8.6f, %8.6f\n"
                                  "%8.6f, %8.6f, %8.6f, %8.6f, %8.6f",
                                   12.345,  1234.5678,  123456.789,  12345678.901,  1234567890.123456,
                                  -12.345, -1234.5678, -123456.789, -12345678.901, -1234567890.123456);
        }
        sw.stop();
        time = sw.getMillisec();

#if defined(VSNPRINTF_SHORT_DISPLAY) && (VSNPRINTF_SHORT_DISPLAY != 0)
        printf(">>> %-15s <<<  time = %-8.3f ms\n\n", "jmc_snprintf()", time);
  #ifdef _MSC_VER
        printf("jmc_sprintf() preformance is _snprintf_s() %0.3f X times.\n\n", timeReference / time);
  #else
        printf("jmc_sprintf() preformance is snprintf() %0.3f X times.\n\n", timeReference / time);
  #endif // _MSC_VER
#else
        printf(">>> %-18s <<<\n\n", "jmc_snprintf()");
        printf("result = \n%s\n\n", fmtbuf2);
        printf("len  = %d, strlen() = %d\n", fmtlen, jm_strlen(fmtbuf2));
  #ifdef _MSC_VER
        printf("time = %0.3f ms, preformance is _snprintf_s(): %0.3f X (times)\n", time, timeReference / time);
  #else
        printf("time = %0.3f ms, preformance is snprintf(): %0.3f X (times)\n", time, timeReference / time);
  #endif // _MSC_VER
        printf("\n");
#endif // VSNPRINTF_SHORT_DISPLAY
    }
#endif
    printf("\n");
}

void OStringStream_Performance_Test()
{
    int i = 0;
    double time = 0.0;
    int loop_times = 9999999;
    jimi::stop_watch sw;

    char fmtbuf[512] = { 0 };
    int fmtlen = 0;

#ifndef _DEBUG
    loop_times = 999999;
#else
    loop_times = 999;
#endif

    // 下面这种方式比后面那一种方式还要慢, 所以放弃了

#if 0
    {
        std::string out_str;
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            std::ostringstream ostr;
            ostr << 123 << ", " << 123456789 << ", " << -123 << ", " << -123456789 << ", "
                 << std::right << std::setfill('0') << std::setw(4) << 777 << ", "
                 << std::right << std::setfill(' ') << std::setw(5) << 8888 << ", "
                 << std::right << std::setfill('0') << std::setw(8) << 99999 << ","
                 << std::endl;
            ostr << std::setfill('0') << std::setw(6) << 1  << ", "
                 << std::setfill('0') << std::setw(5) << 1234 << ", "
                 << std::setfill('0') << std::setw(5) << 12345678 << ", "
                 << std::left << std::setfill('0') << std::setw(6) << 1 << ", "
                 << std::left << std::setfill('0') << std::setw(5) << 1234 << ", "
                 << std::left << std::setfill('0') << std::setw(5) << 12345678;
            if (i == loop_times - 1)
                out_str = ostr.str();
        }
        sw.stop();
        time = sw.getMillisec();

        printf("==============================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        ostr.clear();\n"
               "        ostr << 123 << \", \" << 123456789 << \", \" << -123 << \", \" << -123456789 << \", \"\n"
               "             << std::right << std::setfill('0') << std::setw(4) << 777 << \", \"\n"
               "             << std::right << std::setfill(' ') << std::setw(5) << 8888 << \", \"\n"
               "             << std::right << std::setfill('0') << std::setw(8) << 99999 << \",\"\n"
               "             << std::endl;\n"
               "        ostr << std::setfill('0') << std::setw(6) << 1  << \", \"\n"
               "             << std::setfill('0') << std::setw(5) << 1234 << \", \"\n"
               "             << std::setfill('0') << std::setw(5) << 12345678 << \", \"\n"
               "             << std::left << std::setfill('0') << std::setw(6) << 1 << \", \"\n"
               "             << std::left << std::setfill('0') << std::setw(5) << 1234 << \", \"\n"
               "             << std::left << std::setfill('0') << std::setw(5) << 12345678;");
        printf("    }\n\n");
        printf("==============================================================================\n\n");

        printf("result = \n%s\n\n", out_str.c_str());
        printf("len  = %d, strlen() = %d\n", fmtlen, jm_strlen(out_str.c_str()));
        printf("time = %0.3f ms\n", time);
        printf("\n");
    }
#endif

#if 1
    //
    // How to reuse an ostringstream?
    //
    // Reference: http://stackoverflow.com/questions/624260/how-to-reuse-an-ostringstream
    //
    {
        std::ostringstream ostr;
        ostr.str().reserve(512);
        // 要使用ostr.clear(); ostr.seekp(0); 来重置ostr的状态, 必须先给它写点东西, 才能正常工作
        ostr << 11 << std::endl;
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            ostr.clear();
            //ostr.str("");
            ostr.seekp(0);  // for outputs: seek put ptr to start
//          ostr.seekg(0);  // for inputs: seek get ptr to start
            ostr << 123 << ", " << 123456789 << ", " << -123 << ", " << -123456789 << ", "
                 << std::right << std::setfill('0') << std::setw(4) << 777 << ", "
                 << std::right << std::setfill(' ') << std::setw(5) << 8888 << ", "
                 << std::right << std::setfill('0') << std::setw(8) << 99999 << ","
                 << std::endl;
            ostr << std::setfill('0') << std::setw(6) << 1  << ", "
                 << std::setfill('0') << std::setw(5) << 1234 << ", "
                 << std::setfill('0') << std::setw(5) << 12345678 << ", "
                 << std::left << std::setfill('0') << std::setw(6) << 1 << ", "
                 << std::left << std::setfill('0') << std::setw(5) << 1234 << ", "
                 << std::left << std::setfill('0') << std::setw(5) << 12345678;
        }
        sw.stop();
        time = sw.getMillisec();

        printf("==============================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        ostr.clear();\n"
               "        ostr << 123 << \", \" << 123456789 << \", \" << -123 << \", \" << -123456789 << \", \"\n"
               "             << std::right << std::setfill('0') << std::setw(4) << 777 << \", \"\n"
               "             << std::right << std::setfill(' ') << std::setw(5) << 8888 << \", \"\n"
               "             << std::right << std::setfill('0') << std::setw(8) << 99999 << \",\"\n"
               "             << std::endl;\n"
               "        ostr << std::setfill('0') << std::setw(6) << 1  << \", \"\n"
               "             << std::setfill('0') << std::setw(5) << 1234 << \", \"\n"
               "             << std::setfill('0') << std::setw(5) << 12345678 << \", \"\n"
               "             << std::left << std::setfill('0') << std::setw(6) << 1 << \", \"\n"
               "             << std::left << std::setfill('0') << std::setw(5) << 1234 << \", \"\n"
               "             << std::left << std::setfill('0') << std::setw(5) << 12345678;");
        printf("    }\n\n");
        printf("==============================================================================\n\n");

        printf("result = \n%s\n\n", ostr.str().c_str());
        printf("len  = %d, strlen() = %d\n", fmtlen, jm_strlen(ostr.str().c_str()));
        printf("time = %0.3f ms\n", time);
        printf("\n");
    }
#endif

#if 0
    {
        std::ostringstream ostr;
        ostr.str().reserve(512);
        // 要使用ostr.clear(); ostr.seekp(0); 来重置ostr的状态, 必须先给它写点东西, 才能正常工作
        ostr << 11 << std::endl;
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            ostr.clear();
            //ostr.str("");
            ostr.seekp(0);  // for outputs: seek put ptr to start
//          ostr.seekg(0);  // for inputs: seek get ptr to start
            ostr << 123 << ", " << 123456789 << ", " << -123 << ", " << -123456789 << ", "
                 << 777 << ", "
                 << 8888 << ", "
                 << 99999 << ","
                 << std::endl;
            ostr << 1  << ", "
                 << 1234 << ", "
                 << 12345678 << ", "
                 << 1 << ", "
                 << 1234 << ", "
                 << 12345678;
        }
        sw.stop();
        time = sw.getMillisec();

        printf("==============================================================================\n\n");
        printf("    for (i = 0; i < %d; ++i) {\n", loop_times);
        printf("        ostr.clear();\n"
               "        ostr << 123 << \", \" << 123456789 << \", \" << -123 << \", \" << -123456789 << \", \"\n"
               "             << 777 << \", \"\n"
               "             << 8888 << \", \"\n"
               "             << 99999 << \",\"\n"
               "             << std::endl;\n"
               "        ostr << 1  << \", \"\n"
               "             << 1234 << \", \"\n"
               "             << 12345678 << \", \"\n"
               "             << 1 << \", \"\n"
               "             << 1234 << \", \"\n"
               "             << 12345678;");
        printf("    }\n\n");
        printf("==============================================================================\n\n");

        printf("result = \n%s\n\n", ostr.str().c_str());
        printf("len  = %d, strlen() = %d\n", fmtlen, jm_strlen(ostr.str().c_str()));
        printf("time = %0.3f ms\n", time);
        printf("\n");
    }
#endif
    printf("\n");
}

void OStringStream_Test()
{
    std::ostringstream ostr;
    ostr.str().reserve(512);
    ostr << 11 << std::endl;

    ostr.clear();
    ostr.seekp(0);

    ostr << 123 << ", " << 123456789 << ", " << -123 << ", " << -123456789 << ", "
         << std::setfill('0') << std::setw(4) << 777 << ", "
         << std::setfill(' ') << std::setw(5) << 8888 << ", "
         << std::setfill('0') << std::setw(8) << 99999 << ","
         << std::endl;
    ostr << std::setfill('0') << std::setw(6) << 1  << ", "
         << std::setfill('0') << std::setw(5) << 1234 << ", "
         << std::setfill('0') << std::setw(5) << 12345678 << ", "
         << std::left << std::setfill('0') << std::setw(6) << std::skipws << 1 << ", "
         << std::left << std::setfill('0') << std::setw(5) << std::skipws << 1234 << ", "
         << std::left << std::setfill('0') << std::setw(5) << std::skipws << 12345678;

    ostr.clear();
    ostr.seekp(0);

    ostr << 123 << ", " << 123456789 << ", " << -123 << ", " << -123456789 << ", "
         << std::setfill('0') << std::setw(4) << 777 << ", "
         << std::setfill(' ') << std::setw(5) << 8888 << ", "
         << std::setfill('0') << std::setw(8) << 99999 << ","
         << std::endl;
    ostr << std::setfill('0') << std::setw(6) << 1  << ", "
         << std::setfill('0') << std::setw(5) << 1234 << ", "
         << std::setfill('0') << std::setw(5) << 12345678 << ", "
         << std::left << std::setfill('0') << std::setw(6) << std::skipws << 1 << ", "
         << std::left << std::setfill('0') << std::setw(5) << std::skipws << 1234 << ", "
         << std::left << std::setfill('0') << std::setw(5) << std::skipws << 12345678;

    printf("std::ostringstream ostr.str() = %s\n\nostr.size() = %d\n", ostr.str().c_str(), ostr.str().size());
    printf("\n");
}
