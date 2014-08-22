
#include <jimic/system/console.h>
#include <jimic/system/sys_timer.h>

#include <stdio.h>

/* 预热时间至少要大于500毫秒, 如果还不够, 可以自行增加最小预热时间 */

void JIMIC_API jimi_cpu_warmup(int delayTime)
{
#if defined(NDEBUG) || !defined(_DEBUG)
    jmc_timestamp startTime, stopTime;
    volatile int sum = 0;
    jmc_timestamp elapsedTime = 0;
    jmc_timestamp delayTimeLimit = (jmc_timestamp)delayTime;
    printf("CPU warm up start ...\n");
    fflush(stdout);
    startTime = jmc_now_time();
    do {
        // 如果有聪明的编译器能发现这是一个固定值就NB了, 应该没有
        for (int i = 0; i < 10000; ++i) {
            sum += i;
            // 循环顺序故意颠倒过来的
            for (int j = 5000; j >= 0; --j) {
                sum -= j;
            }
        }
        stopTime = jmc_now_time();
        elapsedTime += stopTime - startTime;
    } while (elapsedTime < delayTimeLimit);

    // 输出sum的值只是为了防止编译器把循环优化掉
    printf("sum = %u, time: %I64d ms\n", sum, elapsedTime);
    printf("CPU warm up done  ... \n\n");
    fflush(stdout);
#endif  /* !_DEBUG */
}
