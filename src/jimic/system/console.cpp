
#include <jimic/system/console.h>
#include <jimi/system/stop_watch.h>

#include <stdio.h>

/* 预热时间至少要大于500毫秒, 如果还不够, 可以自行增加最小预热时间 */

void JIMI_API jimi_cpu_warmup(int delayTime)
{
#if defined(NDEBUG) || !defined(_DEBUG)
    jimi::stop_watch sw;
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
#endif  /* !_DEBUG */
}
