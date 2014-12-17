
#ifndef _JIMIC_SYSTEM_SYS_TIMER_H_
#define _JIMIC_SYSTEM_SYS_TIMER_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimic/core/jimic_def.h"
#include "jimic/core/jimic_export.h"

#ifndef JMC_INLINE
#ifdef _MSC_VER
#define JMC_INLINE  __inline
#else
#define JMC_INLINE  inline
#endif // _MSC_VER
#endif // JMC_INLINE

#if _WIN32 || _WIN64
#include <time.h>
#include <windows.h>
#elif __linux__
  #ifdef __cplusplus
    #include <ctime>
  #else
    #include <time.h>   // for clock_gettime()
  #endif
#else  /* generic Unix */
#include <sys/time.h>
#endif  /* (choice of OS) */

#ifdef __cplusplus
extern "C" {
#endif

typedef int64_t jmc_timestamp;
typedef double  jmc_timefloat;

////////////////////////////////////////////////////////////////////////////////

static JMC_INLINE jmc_timestamp jmc_get_timestamp(void);

static JMC_INLINE jmc_timestamp jmc_get_nanosec(void);
static JMC_INLINE jmc_timestamp jmc_get_millisec(void);

static JMC_INLINE jmc_timefloat jmc_get_secondf(void);
static JMC_INLINE jmc_timefloat jmc_get_millisecf(void);

////////////////////////////////////////////////////////////////////////////////

static JMC_INLINE jmc_timestamp jmc_get_interval_millisec(jmc_timestamp time_interval);
static JMC_INLINE jmc_timefloat jmc_get_interval_millisecf(jmc_timestamp time_interval);
static JMC_INLINE jmc_timefloat jmc_get_interval_secondf(jmc_timestamp time_interval);

////////////////////////////////////////////////////////////////////////////////

/* ��С��λ: ������ϵͳ����Сʱ�������λ, Windows: CPU TSC����; Linux: ����(nsec); Unix: ΢��(usec). */
static
JMC_INLINE
jmc_timestamp jmc_get_timestamp(void)
{
    jmc_timestamp result;

#if _WIN32 || _WIN64
    LARGE_INTEGER qp_cnt;
    QueryPerformanceCounter(&qp_cnt);
    result = (jmc_timestamp)qp_cnt.QuadPart;
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
    result = (jmc_timestamp)((int64_t)(1000000UL) * (int64_t)(tv.tv_sec) + (int64_t)(tv.tv_usec);
#endif  /*(choice of OS) */

    return result;
}

/* ��С��λ: ����(nanosecond), ����ֵ: int64_t. */
static
JMC_INLINE
jmc_timestamp jmc_get_nanosec(void)
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

/* ��λ: ����(millisecond), ����ֵ: int64_t. */
static
JMC_INLINE
jmc_timestamp jmc_get_millisec(void)
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

/* ��λ: ��(second), ����ֵ: ����ֵ */
static
JMC_INLINE
jmc_timefloat jmc_get_secondf(void)
{
    jmc_timefloat result;

    jmc_timestamp time_usecs;
    time_usecs = jmc_get_nanosec();

#if _WIN32 || _WIN64
    result = (jmc_timefloat)time_usecs * 1E-9;
#elif __linux__
    result = (jmc_timefloat)time_usecs * 1E-9;
#else  /* generic Unix */
    result = (jmc_timefloat)time_usecs * 1E-6;
#endif  /*(choice of OS) */

    return result;
}

/* ��λ: ����(millisecond), ����ֵ. */
static
JMC_INLINE
jmc_timefloat jmc_get_millisecf(void)
{
    jmc_timefloat result;

    jmc_timestamp time_usecs;
    time_usecs = jmc_get_nanosec();

#if _WIN32 || _WIN64
    result = (jmc_timefloat)time_usecs * 1E-6;
#elif __linux__
    result = (jmc_timefloat)time_usecs * 1E-6;
#else  /* generic Unix */
    result = (jmc_timefloat)time_usecs * 1E-3;
#endif  /*(choice of OS) */

    return result;
}

/* ����timestamp���ֵ�ó�����ʱ��ĺ���ֵ, ��λ: ����(millisecond), ����ֵ: int64_t. */
static
JMC_INLINE
jmc_timestamp jmc_get_interval_millisec(jmc_timestamp time_interval)
{
    jmc_timestamp result;

#if _WIN32 || _WIN64
    LARGE_INTEGER qp_freq;
    QueryPerformanceFrequency(&qp_freq);
    result = (jmc_timestamp)(((double)time_interval / (double)qp_freq.QuadPart) * 1000.0);
#elif __linux__
    result = (jmc_timestamp)time_interval / 1000000LL;
#else  /* generic Unix */
    result = (jmc_timestamp)time_interval / 1000LL;
#endif  /*(choice of OS) */

    return result;
}

/* ����timestamp���ֵ�ó�����ʱ��ĺ���ֵ, ��λ: ����(millisecond), ����ֵ: ����ֵ. */
static
JMC_INLINE
jmc_timefloat jmc_get_interval_millisecf(jmc_timestamp time_interval)
{
    jmc_timefloat result;

#if _WIN32 || _WIN64
    LARGE_INTEGER qp_freq;
    QueryPerformanceFrequency(&qp_freq);
    result = (jmc_timefloat)(((double)time_interval / (double)qp_freq.QuadPart) * 1000.0);
#elif __linux__
    result = (jmc_timefloat)time_interval * 1E-6;
#else  /* generic Unix */
    result = (jmc_timefloat)time_interval * 1E-3;
#endif  /*(choice of OS) */

    return result;
}

/* ����timestamp���ֵ�ó�����ʱ�������, ��λ: ��(second), ����ֵ: ����ֵ. */
static
JMC_INLINE
jmc_timefloat jmc_get_interval_secondf(jmc_timestamp time_interval)
{
    jmc_timefloat result;

#if _WIN32 || _WIN64
    LARGE_INTEGER qp_freq;
    QueryPerformanceFrequency(&qp_freq);
    result = (jmc_timefloat)(((double)time_interval / (double)qp_freq.QuadPart));
#elif __linux__
    result = (jmc_timefloat)time_interval * 1E-9;
#else  /* generic Unix */
    result = (jmc_timefloat)time_interval * 1E-6;
#endif  /*(choice of OS) */

    return result;
}

////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif  /* !_JIMIC_SYSTEM_SYS_TIMER_H_ */
