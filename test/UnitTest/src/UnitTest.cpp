//
// UnitTest.cpp : �������̨Ӧ�ó������ڵ㡣
//

// whether use crtdbg check?
#ifdef _DEBUG
#ifndef USE_CRTDBG_CHECK
#define USE_CRTDBG_CHECK    1
#endif
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

#if USE_CRTDBG_CHECK
#ifdef _DEBUG
// crtdbg.h must be behind the stdlib.h
#include <crtdbg.h>
#endif
#endif

#ifdef _DEBUG
#ifndef DEBUG_CLIENTBLOCK
#define DEBUG_CLIENTBLOCK   new(_CLIENT_BLOCK, __FILE__, __LINE__)
#endif
#define new                 DEBUG_CLIENTBLOCK
#else
#undef  DEBUG_CLIENTBLOCK
#define DEBUG_CLIENTBLOCK
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

#include "SampleThread.h"

/**********************************************************
 *
 *  Use Visual Leak Detector(vld) for Visual C++,
 *  Homepage: http://vld.codeplex.com/
 *
 **********************************************************/
#ifdef _MSC_VER
#ifdef _DEBUG
// �����û�а�װvld(Visual Leak Detector), ��ע�͵���һ��.
//#include <vld.h>
#endif  /* _DEBUG */
#endif  /* _MSC_VER */

#include <string>
using namespace std;

USING_NS_JIMI;
USING_NS_JIMI_LOG;
USING_NS_JIMI_SYSTEM;

USING_NS_UNITEST;

NS_UNITEST_BEGIN

/* ����CRTDBG�Ļ���(Debugģʽ��, ����ڴ�Խ����ڴ�й©����) */

void set_crtdbg_env()
{
/* ʹ��CRTDBG�������ڴ�Խ������, �����ʹ����vld, �ڴ�й©��Ϣ�ɹر� */
#if USE_CRTDBG_CHECK
    // ���� CRT ����ģʽ
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);

    // ����Ѿ�����vld.h, ����ʾcrtdbg���ڴ�й©��Ϣ
#ifndef VLD_RPTHOOK_INSTALL
    // �����˳�ʱ, ��ʾ�ڴ�й©��Ϣ
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif  /* VLD_RPTHOOK_INSTALL */
#endif  /* USE_CRTDBG_CHECK */
}

/* Ԥ��ʱ������Ҫ����500����, ���������, ��������������СԤ��ʱ�� */
void jimi_cpu_warmup()
{
#ifndef _DEBUG
    stop_watch sw;
    volatile int sum = 0;
    double elapsedTime = 0.0;
    printf("CPU warm up start ...\n");
    do {
        sw.restart();
        // ����д����ı������ܷ�������һ���̶�ֵ��ţ����, Ӧ��û��
        for (int i = 0; i < 10000; ++i) {
            sum += i;
            // ѭ��˳�����ߵ�������
            for (int j = 5000; j >= 0; --j) {
                sum -= j;
            }
        }
        sw.stop();
        elapsedTime += sw.getMillisec();
    } while (elapsedTime < 500.0);
    // ���sum��ֵֻ��Ϊ�˷�ֹ��������ѭ���Ż���
    printf("sum = %u, time: %0.3f ms\n", sum, elapsedTime);
    printf("CPU warm up done ... \n\n");
#endif
}

/* ����std::string�Ƿ�ʹ����COW(Copy On Write) */

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

/* char_traits<T>����ַ��������Ĳ��� */

void Char_Traits_Test()
{
    const int LOOP_TIMES = 10000000;
    stop_watch sw;
    double time1, time2, time3;
    int i, j;

    String str = "abcdefg";
    printf("str.c_str() = %s\n\n", str.c_str());

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
        printf("char_traits<char>::strncpy_u2(), time = %0.3f ms\n", time3);

        printf("\n");
    }

    if (pstr1) free(pstr1);
    if (pstr2) free(pstr2);
    if (pstr3) free(pstr3);
    if (pstr4) free(pstr4);
}

int UnitTest_Main(int argc, char *argv[])
{
    // ����CRTDBG�Ļ���(Debugģʽ��, ����ڴ�Խ����ڴ�й©����)
    set_crtdbg_env();

    sLog.log_begin();

    util::CommandLine cmdLine;
    int cnt;
    if ((cnt = cmdLine.parse(argc, argv)) >= 0) {
        string &strCmdLine = cmdLine.getCmdLine();
        sLog.info(strCmdLine.c_str());
    }

    printf("\n");

    // CPU ����/Ԥ��
    jimi_cpu_warmup();

    // ����std::string�Ƿ�ʹ����COW(Copy On Write)
    String_Copy_On_Write_Test();

    // char_traits<T>����ַ��������Ĳ���
    Char_Traits_Test();

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
