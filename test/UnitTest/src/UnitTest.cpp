//
// UnitTest.cpp : 定义控制台应用程序的入口点。
//

// whether use crtdbg check?
#ifndef USE_CRTDBG_CHECK
#define USE_CRTDBG_CHECK    1
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

#include <asmlib/asmlib.h>
#include "SampleThread.h"
#include "FastMemcpy.h"

#include <jimic/platform/win/fast_memcpy.h>
#include <jimic/string/jmf_strings.h>

#include <stdlib.h>
#include <string>

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#pragma warning(disable: 4996)

/**
 * for asmlib
 */
#pragma comment(lib, "libacof32.lib")

/* 基于CRT的内存泄漏检测 */
#if USE_CRTDBG_CHECK

//
// C++中基于CRT的内存泄漏检测
//
// Reference: http://www.cnblogs.com/weiym/archive/2013/02/25/2932810.html
//

#ifdef _DEBUG
  #define DEBUG_CLIENTBLOCK  new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
  #define DEBUG_CLIENTBLOCK
#endif

// 如果不定义这个宏, C方式的malloc泄露不会被记录下来
#define _CRTDBG_MAP_ALLOC

// crtdbg.h must be behind the stdlib.h
#include <crtdbg.h>

#ifdef _DEBUG
  #define new DEBUG_CLIENTBLOCK
#endif

#endif  /* USE_CRTDBG_CHECK */

/**********************************************************
 *
 *  Use Visual Leak Detector(vld) for Visual C++,
 *  Homepage: http://vld.codeplex.com/
 *
 **********************************************************/
#ifdef _MSC_VER
#ifdef _DEBUG
// 如果你没有安装vld(Visual Leak Detector), 请注释掉这一句.
//#include <vld.h>
#endif  /* _DEBUG */
#endif  /* _MSC_VER */

using namespace std;

USING_NS_JIMI;
USING_NS_JIMI_LOG;
USING_NS_JIMI_SYSTEM;

USING_NS_UNITEST;

NS_UNITEST_BEGIN

/* 设置CRTDBG的环境(Debug模式下, 检查内存越界和内存泄漏问题) */

void set_crtdbg_env()
{
/* 使用CRTDBG将会检查内存越界问题, 如果你使用了vld, 内存泄漏信息可关闭 */
#if USE_CRTDBG_CHECK

    // 设置 CRT 报告模式
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);

    // 如果已经引用vld.h, 则不显示crtdbg的内存泄漏信息
#ifndef VLD_RPTHOOK_INSTALL
    // 进程退出时, 显示内存泄漏信息
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif  /* VLD_RPTHOOK_INSTALL */

#endif  /* USE_CRTDBG_CHECK */
}

/* 预热时间至少要大于500毫秒, 如果还不够, 可以自行增加最小预热时间 */

void jimi_cpu_warmup()
{
#ifndef _DEBUG
    stop_watch sw;
    volatile int sum = 0;
    double elapsedTime = 0.0;
    printf("CPU warm up start ...\n");
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
    } while (elapsedTime < 500.0);
    // 输出sum的值只是为了防止编译器把循环优化掉
    printf("sum = %u, time: %0.3f ms\n", sum, elapsedTime);
    printf("CPU warm up done ... \n\n");
#endif
}

/* 从size转换成KB, MB, GB, TB */

int get_bytes_display(char *buffer, size_t buf_size, size_t size)
{
    int result;
    if (size <= 8192)
        result = jm_sprintf(buffer, buf_size, "%d", size);
    else if (size < 1024 * 1024)
        result = jm_sprintf(buffer, buf_size, "%d K", size / 1024);
    else if (size < 1024 * 1024 * 1024)
        result = jm_sprintf(buffer, buf_size, "%d M", size / (1024 * 1024));
    else
        result = jm_sprintf(buffer, buf_size, "%d G", size / (1024 * 1024 * 1024));
    return result;
}

/* 从size转换成KB, MB, GB, TB */

int get_bytes_display64(char *buffer, size_t buf_size, uint64_t size)
{
    int result;
    if (size <= 8192ULL)
        result = jm_sprintf(buffer, buf_size, "%d", size);
    else if (size < 0x0000000000100000ULL)
        result = jm_sprintf(buffer, buf_size, "%d K", size / 1024ULL);
    else if (size < 0x0000000040000000ULL)
        result = jm_sprintf(buffer, buf_size, "%d M", size / 0x0000000000100000ULL);
    else if (size < 0x0000010000000000ULL)
        result = jm_sprintf(buffer, buf_size, "%d G", size / 0x0000000040000000ULL);
    else
        result = jm_sprintf(buffer, buf_size, "%d T", size / 0x0000010000000000ULL);
    return result;
}

/* 测试std::string是否使用了COW(Copy On Write) */

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

static char jabberwocky[] =
        "'Twas brillig, and the slithy toves\n"
        "Did gyre and gimble in the wabe:\n"
        "All mimsy were the borogoves,\n"
        "And the mome raths outgrabe.\n"

        "Beware the Jabberwock, my son!\n"
        "The jaws that bite, the claws that catch!\n"
        "Beware the Jubjub bird, and shun\n"
        "The frumious Bandersnatch!\n"

        "He took his vorpal sword in hand:\n"
        "Long time the manxome foe he sought -\n"
        "So rested he by the Tumturn tree,\n"
        "And stood awhile in thought.\n"

        "And, as in uffish thought he stood;\n"
        "The Jabberwock, with eyes of flame,\n"
        "Came whiffling through the tulgey wood,\n"
        "And burbled as it came!\n"

        "One, two! One, two! And through and through\n"
        "The vorpal blade went snicker-snackl\n"
        "He left it dead, and with its head\n"
        "He went galumphing back.\n"

        "And hast thou slain the Jabberwock?\n"
        "Come to my arms, my beamish boy!\n"
        "O frabjous day! Callooh! Callay!\n"
        "He chortled in his joy.\n"

        "'Twas brillig, and the slithy toves\n"
        "Did gyre and gimble in the wabe:\n"
        "All mimsy were the borogoves,\n"
        "And the mome raths outgrabe.";

/* String类的基础测试 */

void String_Base_Test()
{
    jimi::string str1 = "abcdefg";
    jimi::string str2 = "hijklmnop";
    printf("str1.c_str() = %s\n", str1.c_str());
    printf("str2.c_str() = %s\n\n", str2.c_str());
    printf("? (str1 == str2) = %d\n", (str1 == str2));
    printf("? (str1 == str1) = %d\n\n", (str1 == str1));

    str1.swap(str2);

    printf("str1.swap(str2);\n\n");
    printf("str1.c_str() = %s\n", str1.c_str());
    printf("str2.c_str() = %s\n\n", str2.c_str());

    printf("sizeof(str1) = %d bytes\n\n", sizeof(str1));

    str1 = str2;
    printf("str1 = str2;\n");
    printf("str1.c_str() = %s\n\n", str1.c_str());

    str1 = "i21938219";
    printf("str1 = \"i21938219\";\n");
    printf("str1.c_str() = %s\n", str1.c_str());
    printf("\n");

    str2.append("00abcd");
    str2.append(str1);

    printf("str2.c_str() = %s\n", str2.c_str());
    printf("\n");

    jimi::string str3('a');
    printf("str3.c_str() = %s\n", str3.c_str());
    printf("\n");

    jimi::string str4;
    str4 = jabberwocky;
    printf("str4.c_str() = \n%s\n\n", str4.c_str());
    printf("str4.size()  = %d bytes\n", str4.size());
    printf("\n");
}

void StrLwr_Test(int nTestLen)
{
    int i;
#if 1
    int loop_times = 200000;
#else
    int loop_times = 4;
#endif
    int nBufLen, nStrLen;
    char *tolower_test1, *tolower_test2, *tolower_test3;
    char *result_str;
    double time1, time2, time3;
    stop_watch sw;

    nStrLen = ::jm_strlen(jabberwocky);
    nBufLen = nTestLen + 1;
    if (nBufLen < 64)
        nBufLen = 64;

    tolower_test1 = (char *)::_aligned_malloc(nBufLen * sizeof(char), 64);
    if (tolower_test1) {
        ::jm_strncpy(tolower_test1, nBufLen, jabberwocky, nTestLen);
        tolower_test1[nTestLen] = '\0';
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            result_str = ::strlwr(tolower_test1);
        }
        sw.stop();
        time1 = sw.getMillisec();
        //printf("tolower_test1.c_str() = \n%s\n\ntolower_test1.size() = %d bytes\n", tolower_test1, ::jm_strlen(tolower_test1));
    }
    //printf("\n");

    tolower_test2 = (char *)::_aligned_malloc(nBufLen * sizeof(char), 64);
    if (tolower_test1) {
        ::jm_strncpy(tolower_test2, nBufLen, jabberwocky, nTestLen);
        tolower_test2[nTestLen] = '\0';
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            result_str = ::jm_strlwr(tolower_test2, nBufLen);
        }
        sw.stop();
        time2 = sw.getMillisec();
        //printf("tolower_test2.c_str() = \n%s\n\ntolower_test2.size() = %d bytes\n", tolower_test2, ::jm_strlen(tolower_test2));
    }
    //printf("\n");

    tolower_test3 = (char *)::_aligned_malloc(nBufLen * sizeof(char), 64);
    if (tolower_test3) {
        size_t size;
        ::jm_strncpy(tolower_test3, nBufLen, jabberwocky, nTestLen);
        tolower_test3[nTestLen] = '\0';
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
            size = ::jmf_strlwr(tolower_test3);
        }
        sw.stop();
        time3 = sw.getMillisec();
        //printf("tolower_test3.c_str() = \n%s\n\ntolower_test3.size() = %d bytes\n", tolower_test3, size);
    }
    //printf("\n");

    printf("str               size = %d bytes\n\n", ::jm_strlen(tolower_test3));

    printf("strlwr(str)       time = %-9.6f ms\n", time1);
    printf("strlwr_s(str)     time = %-9.6f ms\n", time2);
    printf("jmf_strlwr(str)   time = %-9.6f ms\n", time3);
    printf("\n");

    if (tolower_test1)
        ::_aligned_free(tolower_test1);
    if (tolower_test2)
        ::_aligned_free(tolower_test2);
    if (tolower_test3)
        ::_aligned_free(tolower_test3);
}

void String_Performance_Test()
{
#ifndef _DEBUG
    const int LOOP_TIMES = 1000000;
#else
    const int LOOP_TIMES = 200000;
#endif
    //char *str1, *str2;
    size_t len1, len2;
    char buffer1[512];
    char buffer2[512];
    double time1, time2, time3, time4;
    double time5, time6, time7, time8;
    int i, j = 0, loop_times = 0;
    stop_watch sw;

    sw.restart();
    for (i = 0; i < (LOOP_TIMES >> 0); ++i) {
        strcpy(buffer1, "abcdefghijk");
        //strcpy(buffer1 + (i & 2), "abcdefghijk");
        /*
        strcpy(buffer1 + 0, "abcdefghijk");
        strcpy(buffer1 + 4, "abcdefghijk");
        strcpy(buffer1 + 0, "abcdefghijk");
        strcpy(buffer1 + 4, "abcdefghijk");
        //*/
        len1 = strlen(buffer1);
    }
    sw.stop();
    time1 = sw.getMillisec();

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        strcpy_s(buffer2, 32, "abcdefghijk");
        len2 = strlen("abcdefghijk");
    }
    sw.stop();
    time2 = sw.getMillisec();

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        //jimi::string str = "abcdefghijk";
        jimi::string str("abcdefghijk", sizeof("abcdefghijk") - 1);
    }
    sw.stop();
    time3 = sw.getMillisec();

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        //std::string str = "abcdefghijk";
        std::string str("abcdefghijk", sizeof("abcdefghijk") - 1);
    }
    sw.stop();
    time4 = sw.getMillisec();

    printf("buffer1 = %s, len1 = %d, len2 = %d\n\n", buffer1, len1, len2);

    sw.restart();
    for (i = 0; i < (LOOP_TIMES >> 0); ++i) {
        strcpy(buffer1, "abcdefghijklmnopqrstuvwxyz");
        //strcpy(buffer1 + (i & 2), "abcdefghijklmnopqrstuvwxyz");
        /*
        strcpy(buffer1 + 0, "abcdefghijklmnopqrstuvwxyz");
        strcpy(buffer1 + 4, "abcdefghijklmnopqrstuvwxyz");
        strcpy(buffer1 + 0, "abcdefghijklmnopqrstuvwxyz");
        strcpy(buffer1 + 4, "abcdefghijklmnopqrstuvwxyz");
        //*/
        len1 = strlen(buffer1);
    }
    sw.stop();
    time5 = sw.getMillisec();

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        strcpy_s(buffer2, 32, "abcdefghijklmnopqrstuvwxyz");
        len2 = strlen(buffer2);
    }
    sw.stop();
    time6 = sw.getMillisec();

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        //jimi::string str = "abcdefghijklmnopqrstuvwxyz";
        jimi::string str("abcdefghijklmnopqrstuvwxyz", sizeof("abcdefghijklmnopqrstuvwxyz") - 1);
    }
    sw.stop();
    time7 = sw.getMillisec();

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        //std::string str = "abcdefghijklmnopqrstuvwxyz";
        std::string str("abcdefghijklmnopqrstuvwxyz", sizeof("abcdefghijklmnopqrstuvwxyz") - 1);
    }
    sw.stop();
    time8 = sw.getMillisec();

    printf("buffer1 = %s, len1 = %d, len2 = %d\n\n", buffer1, len1, len2);

    printf("%-30s time = %0.5f ms.\n", "strcpy()     str = \"abcdefghijk\";",   time1);
    printf("%-30s time = %0.5f ms.\n", "strcpy_s()   str = \"abcdefghijk\";",   time2);
    printf("%-30s time = %0.5f ms.\n", "std::string  str = \"abcdefghijk\";",   time4);
    printf("%-30s time = %0.5f ms.\n", "jimi::string str = \"abcdefghijk\";",   time3);
    printf("\n");
    printf("%-30s time = %0.5f ms.\n", "strcpy()     str = \"abcdefg...xyz\";", time5);
    printf("%-30s time = %0.5f ms.\n", "strcpy_s()   str = \"abcdefg...xyz\";", time6);
    printf("%-30s time = %0.5f ms.\n", "std::string  str = \"abcdefg...xyz\";", time8);
    printf("%-30s time = %0.5f ms.\n", "jimi::string str = \"abcdefg...xyz\";", time7);

    printf("\n");
}

//
// Faster strlen function. Warning: in rare cases it may
// cause page faults (if the string is at the end of
// the page). To avoid this, you must align the start
// of the string by 4 bytes, or you must add 3 extra
// bytes to the end of the string.
// E.g., instead of char* str = (char*)malloc(size); use
//                  char* str = (char*)malloc(size + 3);
//
int strlen_my(const char *s) {
	int len = 0;
	for(;;) {
		unsigned x = *(unsigned*)s;
		if ((x & 0xFF) == 0) return len;
		if ((x & 0xFF00) == 0) return len + 1;
		if ((x & 0xFF0000) == 0) return len + 2;
		if ((x & 0xFF000000) == 0) return len + 3;
		s += 4, len += 4;
	}
}

int strlen_AgnerFog(const char* s) {
	__asm {
        mov     eax, [s]               ; get pointer s 
        lea     edx, [eax+3]           ; pointer+3 used in the end 
l1:     
		mov     ebx, [eax]             ; read 4 bytes of string 
        add     eax, 4                 ; increment pointer 
        lea     ecx, [ebx-01010101H]   ; subtract 1 from each byte 
        not     ebx                    ; invert all bytes 
        and     ecx, ebx               ; and these two 
        and     ecx, 80808080H         ; test all sign bits 
        jz      l1                     ; no zero bytes, continue loop 

        mov     ebx, ecx 
        shr     ebx, 16 
        test    ecx, 00008080H         ; test first two bytes 
        cmovz   ecx, ebx               ; shift if not in first 2 bytes 
        lea     ebx, [eax+2]           ; .. and increment pointer by 2 
        cmovz   eax, ebx 
        add     cl,  cl                ; test first byte 
        sbb     eax, edx               ; compute length 
	}
}

__declspec(naked)
inline int __cdecl __builtin_ctz(int bitmask)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        bsf eax, eax
        ret
    }
}

//
// From: http://www.strchr.com/strcmp_and_strlen_using_sse_4.2
//
size_t sse2_strlen(const char* s)
{
    __m128i zero = _mm_set1_epi8( 0 );
    __m128i *s_aligned = (__m128i*) (((long)s) & -0x10L);
    uint8_t misbits = ((long)s) & 0x0f;
    __m128i s16cs = _mm_load_si128( s_aligned );
    __m128i bytemask = _mm_cmpeq_epi8( s16cs, zero );
    int bitmask = _mm_movemask_epi8( bytemask );
    bitmask = (bitmask >> misbits) << misbits;

    // Alternative: use TEST instead of BSF, then BSF at end (only). Much better on older CPUs
    // TEST has latency 1, while BSF has 3!
    while (bitmask == 0) {
        s16cs = _mm_load_si128( ++s_aligned );
        bytemask = _mm_cmpeq_epi8( s16cs, zero );
        bitmask = _mm_movemask_epi8( bytemask );
    }

    // bsf only takes 1 clock cycle on modern cpus
    return ((( const char* ) s_aligned ) - s) + __builtin_ctz(bitmask);
}

void Fast_StrLen_Test()
{
#ifndef _DEBUG
    const int LOOP_TIMES = 1000000;
#else
    const int LOOP_TIMES = 200000;
#endif
    char *str1, *str2, *str3;
    char *pbuffer;
    size_t len1, len2, len3, len4;
    size_t len5, len6, len7, len8;
    char buffer1[1024], buffer2[1024], buffer3[1024], buffer4[1024];
    double time1, time2, time3, time4;
    double time5, time6, time7, time8;
    int i, j = 0, loop_times = 0;
    stop_watch sw;

    ////////////////////////////////////////////////////////////////////////////////////////////////////

    typedef size_t (*pfn_strlen) (const char *_Str);
    pfn_strlen pstrlen = (pfn_strlen)&strlen;
    pbuffer = &buffer1[0];

    // 32 bytes
    strcpy(buffer1, "abcdefghijklmnopqrstuvwxyz|ab1");

    sw.restart();
#if 0
    for (i = 0; i < LOOP_TIMES; ++i) {
        len1 = strlen(buffer1);
    }
#else
    __asm {
        push    ebx
        push    edx
        mov     ebx, LOOP_TIMES
        align   16
strlen_loop1_1:
        mov     edx, pbuffer
        push    edx
        call    dword ptr [pstrlen]
        add     esp, 4
        mov     len1, eax
        dec     ebx
        jnz     strlen_loop1_1
        pop     edx
        pop     ebx
    }
#endif
    sw.stop();
    time1 = sw.getMillisec();

    // 32 bytes
    strcpy(buffer2, "abcdefghijklmnopqrstuvwxyz|ab2");

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len2 = jimi::char_traits<char>::strlen(buffer2);
    }
    sw.stop();
    time2 = sw.getMillisec();

    // 32 bytes
    strcpy(buffer3, "abcdefghijklmnopqrstuvwxyz|ab3");

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len3 = A_strlen(buffer3);
    }
    sw.stop();
    time3 = sw.getMillisec();

    // 32 bytes
    strcpy(buffer4, "abcdefghijklmnopqrstuvwxyz|ab4");

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len4 = jmf_strlen(buffer4);
    }
    sw.stop();
    time4 = sw.getMillisec();

    // 64 bytes
    strcpy(buffer1, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijk1");

    sw.restart();
#if 0
    for (i = 0; i < LOOP_TIMES; ++i) {
        len5 = strlen(buffer1);
    }
#else
    __asm {
        push    ebx
        push    edx
        mov     ebx, LOOP_TIMES
        align   16
strlen_loop1_5:
        mov     edx, pbuffer
        push    edx
        call    dword ptr [pstrlen]
        add     esp, 4
        mov     len5, eax
        dec     ebx
        jnz     strlen_loop1_5
        pop     edx
        pop     ebx
    }
#endif
    sw.stop();
    time5 = sw.getMillisec();

    // 64 bytes
    strcpy(buffer2, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijk2");

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len6 = jimi::char_traits<char>::strlen(buffer2);
    }
    sw.stop();
    time6 = sw.getMillisec();

    // 64 bytes
    strcpy(buffer3, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijk3");

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len7 = A_strlen(buffer3);
    }
    sw.stop();
    time7 = sw.getMillisec();

    // 64 bytes
    strcpy(buffer4, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijk4");

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len8 = jmf_strlen(buffer4);
    }
    sw.stop();
    time8 = sw.getMillisec();

    buffer1[20] = '\0';
    printf("buffer1 = %s, len1 = %d, len2 = %d, len3 = %d, len4 = %d\n\n", buffer1, len1, len2, len3, len4);

    printf("%-30s bytes = %d, time = %0.5f ms.\n", "::strlen()",                    len1, time1);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "char_traits<char>::strlen()",   len2, time2);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "asmlib::A_strlen()",            len3, time3);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "jimi::jmf_strlen()",            len4, time4);
    printf("\n");
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "::strlen()",                    len5, time5);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "char_traits<char>::strlen()",   len6, time6);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "asmlib::A_strlen()",            len7, time7);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "jimi::jmf_strlen()",            len8, time8);
    printf("\n");

    ////////////////////////////////////////////////////////////////////////////////////////////////////

    // 128 bytes
    strcpy(buffer1, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrs1");

    sw.restart();
#if 0
    for (i = 0; i < LOOP_TIMES; ++i) {
        len1 = strlen(buffer1);
    }
#else
    __asm {
        push    ebx
        push    edx
        mov     ebx, LOOP_TIMES
        align   16
strlen_loop2_1:
        mov     edx, pbuffer
        push    edx
        call    dword ptr [pstrlen]
        add     esp, 4
        mov     len1, eax
        dec     ebx
        jnz     strlen_loop2_1

#if 0
		// SSE 指令
		addss	xmm1, xmmword ptr [eax]
		// AVX 指令
		vaddss	xmm1, xmmword ptr [eax]
#endif

        pop     edx
        pop     ebx
    }
#endif
    sw.stop();
    time1 = sw.getMillisec();

    // 128 bytes
    strcpy(buffer2, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrs2");

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len2 = jimi::char_traits<char>::strlen(buffer2);
    }
    sw.stop();
    time2 = sw.getMillisec();

    // 128 bytes
    strcpy(buffer3, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrs3");

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len3 = A_strlen(buffer3);
    }
    sw.stop();
    time3 = sw.getMillisec();

    // 128 bytes
    strcpy(buffer4, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrs4");

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len4 = jmf_strlen(buffer4);
    }
    sw.stop();
    time4 = sw.getMillisec();

    // 256 bytes
    strcpy(buffer1, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
                    "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrs1");

    sw.restart();
#if 0
    for (i = 0; i < LOOP_TIMES; ++i) {
        len5 = strlen(buffer1);
    }
#else
    __asm {
        push    ebx
        push    edx
        mov     ebx, LOOP_TIMES
        align   16
strlen_loop2_5:
        mov     edx, pbuffer
        push    edx
        call    dword ptr [pstrlen]
        add     esp, 4
        mov     len5, eax
        dec     ebx
        jnz     strlen_loop2_5
        pop     edx
        pop     ebx
    }
#endif
    sw.stop();
    time5 = sw.getMillisec();

    // 256 bytes
    strcpy(buffer2, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
                    "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrs2");

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len6 = jimi::char_traits<char>::strlen(buffer2);
    }
    sw.stop();
    time6 = sw.getMillisec();

    // 256 bytes
    strcpy(buffer3, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
                    "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrs3");

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len7 = A_strlen(buffer3);
    }
    sw.stop();
    time7 = sw.getMillisec();

    // 256 bytes
    strcpy(buffer4, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
                    "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrs4");

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len8 = jmf_strlen(buffer4);
    }
    sw.stop();
    time8 = sw.getMillisec();

    buffer1[20] = '\0';
    printf("buffer1 = %s, len1 = %d, len2 = %d, len3 = %d, len4 = %d\n\n", buffer1, len1, len2, len3, len4);

    printf("%-30s bytes = %d, time = %0.5f ms.\n", "::strlen()",                    len1, time1);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "char_traits<char>::strlen()",   len2, time2);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "asmlib::A_strlen()",            len3, time3);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "jimi::jmf_strlen()",            len4, time4);
    printf("\n");
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "::strlen()",                    len5, time5);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "char_traits<char>::strlen()",   len6, time6);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "asmlib::A_strlen()",            len7, time7);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "jimi::jmf_strlen()",            len8, time8);
    printf("\n");

    ///////////////////////////////////////////////////////////////////////

    // 511 bytes
    strcpy(buffer1, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
        "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
        "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
        "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqr1"
        );

    sw.restart();
#if 0
    for (i = 0; i < LOOP_TIMES; ++i) {
        len5 = strlen(buffer1);
    }
#else
    __asm {
        push    ebx
        push    edx
        mov     ebx, LOOP_TIMES
        align   16
strlen_loop3_5:
        mov     edx, pbuffer
        push    edx
        call    dword ptr [pstrlen]
        add     esp, 4
        mov     len5, eax
        dec     ebx
        jnz     strlen_loop3_5
        pop     edx
        pop     ebx
    }
#endif
    sw.stop();
    time5 = sw.getMillisec();

    // 511 bytes
    strcpy(buffer2, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
        "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
        "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
        "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqr2"
        );

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len6 = jimi::char_traits<char>::strlen(buffer2);
    }
    sw.stop();
    time6 = sw.getMillisec();

    // 511 bytes
    strcpy(buffer3, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
        "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
        "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
        "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqr3"
        );

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len7 = A_strlen(buffer3);
    }
    sw.stop();
    time7 = sw.getMillisec();

    // 511 bytes
    strcpy(buffer4, "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
        "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
        "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrst"
        "abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqr4"
        );

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len8 = jmf_strlen(buffer4);
    }
    sw.stop();
    time8 = sw.getMillisec();

    buffer1[20] = '\0';
    printf("buffer1 = %s, len1 = %d, len2 = %d, len3 = %d, len4 = %d\n\n", buffer1, len5, len6, len7, len8);

    printf("%-30s bytes = %d, time = %0.5f ms.\n", "::strlen()",                    len5, time5);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "char_traits<char>::strlen()",   len6, time6);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "asmlib::A_strlen()",            len7, time7);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "jimi::jmf_strlen()",            len8, time8);
    printf("\n");

    ///////////////////////////////////////////////////////////////////////

    // long string jabberwocky
    strcpy(buffer1, jabberwocky);

    sw.restart();
#if 0
    for (i = 0; i < LOOP_TIMES; ++i) {
        len5 = strlen(buffer1);
    }
#else
    __asm {
        push    ebx
        push    edx
        mov     ebx, LOOP_TIMES
        align   16
strlen_loop4_5:
        mov     edx, pbuffer
        push    edx
        call    dword ptr [pstrlen]
        add     esp, 4
        mov     len5, eax
        dec     ebx
        jnz     strlen_loop4_5
        pop     edx
        pop     ebx
    }
#endif
    sw.stop();
    time5 = sw.getMillisec();

    // long string jabberwocky
    strcpy(buffer2, jabberwocky);

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len6 = jimi::char_traits<char>::strlen(buffer2);
    }
    sw.stop();
    time6 = sw.getMillisec();

    // long string jabberwocky
    strcpy(buffer3, jabberwocky);

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len7 = A_strlen(buffer3);
    }
    sw.stop();
    time7 = sw.getMillisec();

    // long string jabberwocky
    strcpy(buffer4, jabberwocky);

    sw.restart();
    for (i = 0; i < LOOP_TIMES; ++i) {
        len8 = jmf_strlen(buffer4);
    }
    sw.stop();
    time8 = sw.getMillisec();

    buffer1[20] = '\0';
    printf("buffer1 = %s, len1 = %d, len2 = %d, len3 = %d, len4 = %d\n\n", buffer1, len5, len6, len7, len8);

    printf("%-30s bytes = %d, time = %0.5f ms.\n", "::strlen()",                    len5, time5);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "char_traits<char>::strlen()",   len6, time6);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "asmlib::A_strlen()",            len7, time7);
    printf("%-30s bytes = %d, time = %0.5f ms.\n", "jimi::jmf_strlen()",            len8, time8);
    printf("\n");

    size_t buf_size = 2 * 1024 * 1024 * sizeof(char);

    str1 = (char *)::malloc(buf_size);
    if (str1) {
        memset(str1, 'A', buf_size - 1);
        str1[buf_size - 1] = '\0';

        sw.restart();
#if 0
        for (i = 0; i < 1000; ++i) {
            len1 = ::strlen(str1);
        }
#else
            __asm {
                push    ebx
                push    edx
                mov     ebx, 1000
                align   16
strlen_loop4_1:
                mov     edx, str1
                push    edx
                call    dword ptr [pstrlen]
                add     esp, 4
                mov     len1, eax
                dec     ebx
                jnz     strlen_loop4_1
                pop     edx
                pop     ebx
            }
#endif
        sw.stop();
        time1 = sw.getMillisec();
    }

    str2 = (char *)::malloc(buf_size);
    if (str2) {
        memset(str2, 'A', buf_size - 1);
        str2[buf_size - 1] = '\0';

        sw.restart();
        for (i = 0; i < 1000; ++i) {
            len2 = ::A_strlen(str2);
        }
        sw.stop();
        time2 = sw.getMillisec();
    }

    str3 = (char *)::malloc(buf_size);
    if (str3) {
        memset(str3, 'A', buf_size - 1);
        str3[buf_size - 1] = '\0';

        sw.restart();
        for (i = 0; i < 1000; ++i) {
            len3 = ::jmf_strlen(str3);
        }
        sw.stop();
        time3 = sw.getMillisec();
    }

    char bufsize_text1[128], bufsize_text2[128], bufsize_text3[128];
    bufsize_text1[0] = '\0';
    get_bytes_display(bufsize_text1, jm_countof(bufsize_text1), len1);
    bufsize_text2[0] = '\0';
    get_bytes_display(bufsize_text2, jm_countof(bufsize_text2), len2);
    bufsize_text3[0] = '\0';
    get_bytes_display(bufsize_text3, jm_countof(bufsize_text3), len3);

    printf("%-30s bytes = %5s, time = %0.5f ms.\n", "::strlen()",         bufsize_text1, time1);
    printf("%-30s bytes = %5s, time = %0.5f ms.\n", "asmlib::A_strlen()", bufsize_text2, time2);
    printf("%-30s bytes = %5s, time = %0.5f ms.\n", "jimi::jmf_strlen()", bufsize_text3, time3);
    printf("\n");

    printf("jimi::jmf_strlen() use time is ::strlen()         use time : %0.3f %%.\n", time3 / time1 * 100.0);
    printf("jimi::jmf_strlen() use time is asmlib::A_strlen() use time : %0.3f %%.\n", time3 / time2 * 100.0);
    printf("\n");

    if (str1) ::free(str1);
    if (str2) ::free(str2);
    if (str3) ::free(str3);
}

#define USE_BUFFER_VERIFY   0

void Memcpy_Test()
{
#ifndef _DEBUG
    const int LOOP_TIMES = 16;
#else
    const int LOOP_TIMES = 4;
#endif
    const unsigned int BUFFER_SIZE = 64 * 1024 * 1024;
    unsigned int buf_size;
    void *buffer1, *buffer2, *buffer3, *buffer4;
    void *buffer5, *buffer6, *buffer7, *buffer8;
    double time1, time2, time3, time4;
    double time5, time6, time7, time8;
    int i, j, loop_times;
    stop_watch sw;
#if USE_BUFFER_VERIFY
    bool verify_ok1, verify_ok2;
#endif

    char bufsize_text[256];
    unsigned int bufsize_list[] = {
        // 16, 32,
        64, 128, 256, 512,
        //
        1024 * 1, 1024 * 2, 1024 * 4, 1024 * 8, 1024 * 16,
        1024 * 32, 1024 * 64, 1024 * 128, 1024 * 256,
        //
        1048576 * 1, 1048576 * 2, 1048576 * 4, 1048576 * 8,
        1048576 * 16, 1048576 * 32, 1048576 * 64
    };

    buffer1 = ::_aligned_malloc(BUFFER_SIZE, 64);
    buffer2 = ::_aligned_malloc(BUFFER_SIZE, 64);
    buffer3 = ::_aligned_malloc(BUFFER_SIZE, 64);
    buffer4 = ::_aligned_malloc(BUFFER_SIZE, 64);

    buffer5 = ::_aligned_malloc(BUFFER_SIZE, 64);
    buffer6 = ::_aligned_malloc(BUFFER_SIZE, 64);
    buffer7 = ::_aligned_malloc(BUFFER_SIZE, 64);
    buffer8 = ::_aligned_malloc(BUFFER_SIZE, 64);

#if 1
    for (j = 0; j < jm_countof(bufsize_list); ++j) {
        buf_size = bufsize_list[j];

        if (buf_size <= 1024)
            loop_times = (8 * 1024 * 1024 / buf_size) * LOOP_TIMES;
        else if (buf_size <= 64 * 1024)
            loop_times = (4 * 1024 * 1024 / buf_size) * LOOP_TIMES;
        else
            loop_times = LOOP_TIMES;

        sw.restart();
        ::memcpy(buffer1, buffer2, buf_size);
        sw.stop();
        time1 = sw.getMillisec();

        sw.restart();
        for (i = 0; i < loop_times; ++i)
            ::memcpy(buffer1, buffer2, buf_size);
        sw.stop();
        time5 = sw.getMillisec();

        sw.restart();
        A_memcpy(buffer3, buffer4, buf_size);
        sw.stop();
        time2 = sw.getMillisec();

        sw.restart();
        for (i = 0; i < loop_times; ++i)
            A_memcpy(buffer3, buffer4, buf_size);
        sw.stop();
        time6 = sw.getMillisec();

#if USE_BUFFER_VERIFY
        buffer_set_val(buffer1, 0, buf_size);
        buffer_set_val(buffer2, 1, buf_size);
#endif
        sw.restart();
        fast_memcpy(buffer5, buffer6, buf_size);
        sw.stop();
        time3 = sw.getMillisec();

        sw.restart();
        for (i = 0; i < loop_times; ++i)
            fast_memcpy(buffer5, buffer6, buf_size);
        sw.stop();
        time7 = sw.getMillisec();

#if USE_BUFFER_VERIFY
        verify_ok1 = verify_buffer_val(buffer5, buf_size, 1);

        buffer_set_val(buffer3, 0, buf_size);
        buffer_set_val(buffer4, 1, buf_size);
#endif
        sw.restart();
        memcpy_mmx4(buffer7, buffer8, buf_size);
        sw.stop();
        time4 = sw.getMillisec();

        sw.restart();
        for (i = 0; i < loop_times; ++i)
            memcpy_mmx4(buffer7, buffer8, buf_size);
        sw.stop();
        time8 = sw.getMillisec();

#if USE_BUFFER_VERIFY
        verify_ok2 = verify_buffer_val(buffer7, buf_size, 1);
#endif

        bufsize_text[0] = '\0';
        get_bytes_display(bufsize_text, jm_countof(bufsize_text), buf_size);

        printf("%-16s size = %5s, time = %0.5f ms\n", "::memcpy()",     bufsize_text, time1);
        printf("%-16s size = %5s, time = %0.5f ms\n", "A_memcpy()",     bufsize_text, time2);
        printf("%-16s size = %5s, time = %0.5f ms\n", "fast_memcpy()",  bufsize_text, time3);
        printf("%-16s size = %5s, time = %0.5f ms\n", "memcpy_mmx4()",  bufsize_text, time4);
#if USE_BUFFER_VERIFY
        printf("\n");
        printf("verify_ok1 = %d, verify_ok2 = %d\n", verify_ok1 , verify_ok2);
#endif

        printf("\n");

        printf("%-16s size = %5s, time = %0.5f ms\n", "::memcpy()",     bufsize_text, time5 / (double)loop_times);
        printf("%-16s size = %5s, time = %0.5f ms\n", "A_memcpy()",     bufsize_text, time6 / (double)loop_times);
        printf("%-16s size = %5s, time = %0.5f ms\n", "fast_memcpy()",  bufsize_text, time7 / (double)loop_times);
        printf("%-16s size = %5s, time = %0.5f ms\n", "memcpy_mmx4()",  bufsize_text, time8 / (double)loop_times);

        printf("\n");
    }
#else

    buf_size = 16 * 1024;

    sw.restart();
    fast_memcpy(buffer1, buffer2, buf_size);
    sw.stop();
    time1 = sw.getMillisec();

    sw.restart();
    memcpy_mmx4(buffer3, buffer4, buf_size);
    sw.stop();
    time2 = sw.getMillisec();

    sw.restart();
    A_memcpy(buffer5, buffer6, buf_size);
    sw.stop();
    time3 = sw.getMillisec();

    sw.restart();
    ::memcpy(buffer7, buffer8, buf_size);
    sw.stop();
    time4 = sw.getMillisec();

    printf("%-16s size = %d K, time = %0.5f ms\n", "fast_memcpy()",   buf_size / 1024, time1);
    printf("%-16s size = %d K, time = %0.5f ms\n", "memcpy_mmx4()",   buf_size / 1024, time2);
    printf("%-16s size = %d K, time = %0.5f ms\n", "A_memcpy()",      buf_size / 1024, time3);
    printf("%-16s size = %d K, time = %0.5f ms\n", "::memcpy()",      buf_size / 1024, time4);

    printf("\n");

    buf_size = 64 * 1024;

    sw.restart();
    fast_memcpy(buffer1, buffer2, buf_size);
    sw.stop();
    time1 = sw.getMillisec();

    sw.restart();
    memcpy_mmx4(buffer3, buffer4, buf_size);
    sw.stop();
    time2 = sw.getMillisec();

    sw.restart();
    A_memcpy(buffer5, buffer6, buf_size);
    sw.stop();
    time3 = sw.getMillisec();

    sw.restart();
    ::memcpy(buffer7, buffer8, buf_size);
    sw.stop();
    time4 = sw.getMillisec();

    printf("%-16s size = %d K, time = %0.5f ms\n", "fast_memcpy()",   buf_size / 1024, time1);
    printf("%-16s size = %d K, time = %0.5f ms\n", "memcpy_mmx4()",   buf_size / 1024, time2);
    printf("%-16s size = %d K, time = %0.5f ms\n", "A_memcpy()",      buf_size / 1024, time3);
    printf("%-16s size = %d K, time = %0.5f ms\n", "::memcpy()",      buf_size / 1024, time4);

    printf("\n");

    buf_size = 16 * 1024 * 1024;

    sw.restart();
    fast_memcpy(buffer1, buffer2, buf_size);
    sw.stop();
    time1 = sw.getMillisec();

    sw.restart();
    memcpy_mmx4(buffer3, buffer4, buf_size);
    sw.stop();
    time2 = sw.getMillisec();

    sw.restart();
    A_memcpy(buffer5, buffer6, buf_size);
    sw.stop();
    time3 = sw.getMillisec();

    sw.restart();
    ::memcpy(buffer7, buffer8, buf_size);
    sw.stop();
    time4 = sw.getMillisec();

    printf("%-16s size = %d M, time = %0.5f ms\n", "fast_memcpy()",   buf_size / (1024 * 1024), time1);
    printf("%-16s size = %d M, time = %0.5f ms\n", "memcpy_mmx4()",   buf_size / (1024 * 1024), time2);
    printf("%-16s size = %d M, time = %0.5f ms\n", "A_memcpy()",      buf_size / (1024 * 1024), time3);
    printf("%-16s size = %d M, time = %0.5f ms\n", "::memcpy()",      buf_size / (1024 * 1024), time4);

    printf("\n");

    buf_size = BUFFER_SIZE;

    sw.restart();
    fast_memcpy(buffer1, buffer2, buf_size);
    sw.stop();
    time1 = sw.getMillisec();

    sw.restart();
    memcpy_mmx4(buffer3, buffer4, buf_size);
    sw.stop();
    time2 = sw.getMillisec();

    sw.restart();
    A_memcpy(buffer5, buffer6, buf_size);
    sw.stop();
    time3 = sw.getMillisec();

    sw.restart();
    ::memcpy(buffer7, buffer8, buf_size);
    sw.stop();
    time4 = sw.getMillisec();

    printf("%-16s size = %d M, time = %0.5f ms\n", "fast_memcpy()",   buf_size / (1024 * 1024), time1);
    printf("%-16s size = %d M, time = %0.5f ms\n", "memcpy_mmx4()",   buf_size / (1024 * 1024), time2);
    printf("%-16s size = %d M, time = %0.5f ms\n", "A_memcpy()",      buf_size / (1024 * 1024), time3);
    printf("%-16s size = %d M, time = %0.5f ms\n", "::memcpy()",      buf_size / (1024 * 1024), time4);

    printf("\n");
#endif

    if (buffer1) _aligned_free(buffer1);
    if (buffer2) _aligned_free(buffer2);
    if (buffer3) _aligned_free(buffer3);
    if (buffer4) _aligned_free(buffer4);

    if (buffer5) _aligned_free(buffer5);
    if (buffer6) _aligned_free(buffer6);
    if (buffer7) _aligned_free(buffer7);
    if (buffer8) _aligned_free(buffer8);
}

/* char_traits<T>相关字符串函数的测试 */

void Char_Traits_Test()
{
#ifndef _DEBUG
    const int LOOP_TIMES = 1000000;
#else
    const int LOOP_TIMES = 50000;
#endif
    double time1, time2, time3;
    stop_watch sw;
    int i, j;

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
            pstr = jimi::char_traits<char>::strncpy_unsafe(pstr3, pstr1, str_len);
            pstr3[str_len] = '\0';
        }
        sw.stop();
        time2 = sw.getMillisec();

        sw.restart();
        for (i = 0; i < LOOP_TIMES; ++i) {
            pstr = jimi::char_traits<char>::strncpy_unsafe2(pstr4, pstr1, str_len);
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
        printf("char_traits<char>::strncpy_u2(), time = %0.5f ms\n", time3);

        printf("\n");
    }

    if (pstr1) free(pstr1);
    if (pstr2) free(pstr2);
    if (pstr3) free(pstr3);
    if (pstr4) free(pstr4);
}

int UnitTest_Main(int argc, char *argv[])
{
    // 设置CRTDBG的环境(Debug模式下, 检查内存越界和内存泄漏问题)
    set_crtdbg_env();

    sLog.log_begin();

    util::CommandLine cmdLine;
    int cnt;
    if ((cnt = cmdLine.parse(argc, argv)) >= 0) {
        std::string &strCmdLine = cmdLine.getCmdLine();
        sLog.info(strCmdLine.c_str());
    }

    printf("\n");

    // CPU 唤醒/预热
    jimi_cpu_warmup();

    // 测试std::string是否使用了COW(Copy On Write)
    //String_Copy_On_Write_Test();

    // Memcpy 内存复制测试
    //Memcpy_Test();

    String_Base_Test();

    String_Performance_Test();

    Fast_StrLen_Test();

    StrLwr_Test(16);
    StrLwr_Test(32);
    StrLwr_Test(64);
    StrLwr_Test(128);
    StrLwr_Test(256);
    StrLwr_Test(1024);

#if 0
    // char_traits<T>相关字符串函数的测试
    Char_Traits_Test();
#endif

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

#if 0
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
#endif

#if 0
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
#endif

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
