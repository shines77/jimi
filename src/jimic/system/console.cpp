
#include <jimic/system/console.h>
#include <jimi/system/stop_watch.h>

#include <stdio.h>

/* Ԥ��ʱ������Ҫ����500����, ���������, ��������������СԤ��ʱ�� */

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
        // ����д����ı������ܷ�������һ���̶�ֵ��NB��, Ӧ��û��
        for (int i = 0; i < 10000; ++i) {
            sum += i;
            // ѭ��˳�����ߵ�������
            for (int j = 5000; j >= 0; --j) {
                sum -= j;
            }
        }
        sw.stop();
        elapsedTime += sw.getMillisec();
    } while (elapsedTime < delayTimeLimit);

    // ���sum��ֵֻ��Ϊ�˷�ֹ��������ѭ���Ż���
    printf("sum = %u, time: %0.3f ms\n", sum, elapsedTime);
    printf("CPU warm up done  ... \n\n");
    fflush(stdout);
#endif  /* !_DEBUG */
}
