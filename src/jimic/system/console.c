
#include "jimic/system/console.h"
#include "jimic/system/sys_timer.h"
#include "jimic/system/get_char.h"

#include <stdio.h>

/* Ԥ��ʱ������Ҫ����500����, ���������, ��������������СԤ��ʱ�� */

void jimi_cpu_warmup(int delayTime)
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
        // ���ѭ�����ܺ�sum��ʵ��һ���̶�ֵ, ��Ӧ��û�б������ܹ����ֻ��Ż���(������Ҳ�����Ż�)
        for (i = 0; i < 10000; ++i) {
            sum += i;
            // ѭ��˳�����ߵ�������
            for (j = 5000; j >= 0; --j) {
                sum -= j;
            }
        }
        stopTime = jmc_get_timestamp();
        elapsedTime += jmc_get_interval_millisecf(stopTime - startTime);
    } while (elapsedTime < delayTimeLimit);

    // ���sum��ֵֻ��Ϊ�˷�ֹ��������ѭ���Ż���
    printf("sum = %u, time: %0.3f ms\n", sum, elapsedTime);
    printf("CPU warm up done  ... \n\n");
    fflush(stdout);
#endif  /* !_DEBUG */
}

int jimi_console_readkey(bool enabledCpuWarmup, bool displayTips,
                         bool echoInput)
{
    int keyCode;
    if (displayTips) {
#if 1
        printf("Press any key to continue ...");
#else
        printf("�밴��������� ...");
#endif
        keyCode = jimi_getch();
        printf("\n");
    }
    else {
        keyCode = jimi_getch();
        if (echoInput) {
            if (keyCode != EOF)
                printf("%c", (char)keyCode);
            else
                printf("EOF: (%d)", keyCode);
        }
    }

    // ��ͣ��, Ԥ��/����һ��CPU, ����500����
    if (enabledCpuWarmup) {
        jimi_cpu_warmup(500);
        //printf("\n");
    }
    return keyCode;
}

int jimi_console_readkeyln(bool enabledCpuWarmup, bool displayTips,
                           bool echoInput)
{
    int keyCode;
    keyCode = jimi_console_readkey(false, displayTips, echoInput);
    printf("\n");

    // ��ͣ��, Ԥ��/����һ��CPU, ����500����
    if (enabledCpuWarmup) {
        jimi_cpu_warmup(500);
        //printf("\n");
    }
    return keyCode;
}
