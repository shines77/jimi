
#ifndef _JIMIC_SYSTEM_SYS_TIMER_H_
#define _JIMIC_SYSTEM_SYS_TIMER_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimic/core/jimic_def.h>
#include <jimic/core/jimic_export.h>

#if _WIN32 || _WIN64
#include <time.h>
#include <windows.h>
#elif __linux__
#include <ctime>
#else /* generic Unix */
#include <sys/time.h>
#endif /* (choice of OS) */

#ifdef __cplusplus
extern "C" {
#endif

typedef int64_t jmc_timestamp;
typedef double  jmc_timefloat;

////////////////////////////////////////////////////////////////////////////////

JMC_INLINE jmc_timestamp jmc_get_now(void);
JMC_INLINE jmc_timefloat jmc_get_nowf(void);
JMC_INLINE jmc_timestamp jmc_get_now_ms(void);

////////////////////////////////////////////////////////////////////////////////

jmc_timestamp JIMIC_API jmc_get_timestamp(void);
jmc_timestamp JIMIC_API jmc_get_millisec(void);

jmc_timestamp JIMIC_API jmc_now_time(void);
jmc_timefloat JIMIC_API jmc_now_timef(void);

////////////////////////////////////////////////////////////////////////////////

/* 单位: 纳秒, nsec */
JMC_INLINE jmc_timestamp jmc_get_now(void)
{
    jmc_timestamp result;

#if _WIN32 || _WIN64
    LARGE_INTEGER qp_cnt, qp_freq;
    QueryPerformanceCounter(&qp_cnt);
    QueryPerformanceFrequency(&qp_freq);
    result = (jmc_timestamp)(((double)qp_cnt.QuadPart / (double)qp_freq.QuadPart) * 1000000000.0);
#elif __linux__
    struct timespec ts;
#if JIMIC_USE_ASSERT
    int status =
#endif  /* JIMIC_USE_ASSERT */
        clock_gettime(CLOCK_REALTIME, &ts);
    JIMIC_ASSERT_EX(status == 0, "CLOCK_REALTIME not supported");
    result = (jmc_timestamp)((int64_t)(1000000000UL) * (int64_t)(ts.tv_sec) + (int64_t)(ts.tv_nsec));
#else  /* generic Unix */
    struct timeval tv;
#if JIMIC_USE_ASSERT
    int status =
#endif  /* JIMIC_USE_ASSERT */
        gettimeofday(&tv, NULL);
    JIMIC_ASSERT_EX(status == 0, "gettimeofday failed");
    result = (jmc_timestamp)((int64_t)(1000000000UL) * (int64_t)(tv.tv_sec) + (int64_t)(1000UL) * (int64_t)(tv.tv_usec));
#endif  /*(choice of OS) */

    return result;
}

/* 单位: 秒, second (浮点) */
JMC_INLINE jmc_timefloat jmc_get_nowf(void)
{
    jmc_timefloat result;

    jmc_timestamp time_usecs;
    time_usecs = jmc_get_now();

#if _WIN32 || _WIN64
    result = (jmc_timefloat)time_usecs * 1E-9;
#elif __linux__
    result = (jmc_timefloat)time_usecs * 1E-9;
#else  /* generic Unix */
    result = (jmc_timefloat)time_usecs * 1E-6;
#endif  /*(choice of OS) */

    return result;
}

/* 单位: 毫秒, msec */
JMC_INLINE jmc_timestamp jmc_get_now_ms(void)
{
    jmc_timestamp result;

#if _WIN32 || _WIN64
    LARGE_INTEGER qp_cnt, qp_freq;
    QueryPerformanceCounter(&qp_cnt);
    QueryPerformanceFrequency(&qp_freq);
    result = (jmc_timestamp)(((double)qp_cnt.QuadPart / (double)qp_freq.QuadPart) * 1000.0);
#elif __linux__
    struct timespec ts;
#if JIMI_USE_ASSERT
    int status =
#endif /* JIMIC_USE_ASSERT */
        clock_gettime(CLOCK_REALTIME, &ts);
    JIMI_ASSERT_EX(status == 0, "CLOCK_REALTIME not supported");
    result = (jmc_timestamp)((int64_t)(1000UL) * (int64_t)(ts.tv_sec) + (int64_t)(ts.tv_nsec) / (int64_t)(1000000UL));
#else /* generic Unix */
    struct timeval tv;
#if JIMI_USE_ASSERT
    int status =
#endif /* JIMIC_USE_ASSERT */
        gettimeofday(&tv, NULL);
    JIMI_ASSERT_EX(status == 0, "gettimeofday failed");
    result = (jmc_timestamp)((int64_t)(1000UL) * (int64_t)(tv.tv_sec) + (int64_t)(tv.tv_usec) / (int64_t)(1000UL));
#endif /*(choice of OS) */

    return result;
}

////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif  /* !_JIMIC_SYSTEM_SYS_TIMER_H_ */
