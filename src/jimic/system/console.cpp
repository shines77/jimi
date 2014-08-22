
#include <jimic/system/console.h>
#include <jimic/system/sys_timer.h>

#include <stdio.h>

/* Ԥ��ʱ������Ҫ����500����, ���������, ��������������СԤ��ʱ�� */

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
        // ����д����ı������ܷ�������һ���̶�ֵ��NB��, Ӧ��û��
        for (int i = 0; i < 10000; ++i) {
            sum += i;
            // ѭ��˳�����ߵ�������
            for (int j = 5000; j >= 0; --j) {
                sum -= j;
            }
        }
        stopTime = jmc_now_time();
        elapsedTime += stopTime - startTime;
    } while (elapsedTime < delayTimeLimit);

    // ���sum��ֵֻ��Ϊ�˷�ֹ��������ѭ���Ż���
    printf("sum = %u, time: %I64d ms\n", sum, elapsedTime);
    printf("CPU warm up done  ... \n\n");
    fflush(stdout);
#endif  /* !_DEBUG */
}
