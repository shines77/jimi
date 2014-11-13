
#include "jimic/system/console.h"
#include "jimic/system/sys_timer.h"

#include <stdio.h>

/* 预热时间至少要大于500毫秒, 如果还不够, 可以自行增加最小预热时间 */

void JIMIC_DLL jimi_cpu_warmup(int delayTime)
{
#if defined(NDEBUG) || !defined(_DEBUG)
    jmc_timestamp startTime, stopTime;
    int i, j;
    volatile int sum = 0;
    jmc_timefloat elapsedTime = 0.0;
    jmc_timefloat delayTimeLimit = (jmc_timefloat)delayTime;
    printf("CPU warm up start ...\n");
    fflush(stdout);
    startTime = jmc_get_timestamp();
    do {
        // 这个循环的总和sum其实是一个固定值, 但应该没有编译器能够发现或优化掉(发现了也不会优化)
        for (i = 0; i < 10000; ++i) {
            sum += i;
            // 循环顺序故意颠倒过来的
            for (j = 5000; j >= 0; --j) {
                sum -= j;
            }
        }
        stopTime = jmc_get_timestamp();
        elapsedTime += jmc_get_interval_millisecf(stopTime - startTime);
    } while (elapsedTime < delayTimeLimit);

    // 输出sum的值只是为了防止编译器把循环优化掉
    printf("sum = %u, time: %0.3f ms\n", sum, elapsedTime);
    printf("CPU warm up done  ... \n\n");
    fflush(stdout);
#endif  /* !_DEBUG */
}
