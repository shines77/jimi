

#include "String_StrLwr_Test.h"

#include "jimi/basic/stddef.h"
#include "jimi/system/stop_watch.h"

#include "jimic/string/jm_strings.h"
#include "jimic/string/jmf_strings.h"
#include "jimic/system/get_char.h"

#include <memory.h>

#include <cctype>       // for std::tolower(ch);
#include <locale>       // for std::tolower(ch , loc);
#include <algorithm>    // for std::for_each
#include <functional>   // for std::placeholders;

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

static const unsigned char s_toUpper[] =
{
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,
    16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
    32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
    48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
    64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
    80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
    96,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
    80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  123, 124, 125, 126, 127,
    128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
    144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
    160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
    176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
    192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
    208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
};

template <typename T>
void strupr_table(T *str)
{
    while (*str != NULL) {
        *str = static_cast<T>(s_toUpper[static_cast<unsigned char>(*str)]);
        ++str;
    }
}

static const unsigned char s_toLower[] =
{
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,
    16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
    32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
    48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
    64,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
    112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 91,  92,  93,  94,  95,
    96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
    112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
    128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
    144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
    160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
    176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
    192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
    208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
};

//
// C++ (coverting lowercase to uppercase without the 'toupper' function)??!
//
// From: http://arstechnica.com/civis/viewtopic.php?f=20&t=696922
//

#define ngx_tolower(c)      (unsigned char) ((c >= 'A' && c <= 'Z') ? (c |  0x20) : c)
#define ngx_toupper(c)      (unsigned char) ((c >= 'a' && c <= 'z') ? (c & ~0x20) : c)

void ngx_strlwr(unsigned char *str)
{
    while (*str != NULL) {
        *str = ngx_tolower(*str);
        str++;
    }
}

/*
template <class T>
void stl_tolower_(T *begin, T *end)
{
    std::for_each(begin, end, std::tolower);
}

template <class T, class Func>
void stl_tolower(std::basic_string<T> &str)
{
    std::for_each(str.begin(), str.end(), std::tolower);
}

template <class T, class Func>
void stl_toupper(std::string& s)
{
    std::for_each(s.begin(), s.end(), std::toupper);
}
//*/

template <typename T>
void strlwr_std(T *str)
{
    T c;
    while ((c = *str) != NULL) {
        if (c >= static_cast<T>('A') && c <= static_cast<T>('Z')) {
            c |= 0x20;
            *str = c;
        }
        ++str;
    }
}

template <typename T>
void strlwr_table(T *str)
{
    while (*str != NULL) {
        *str = static_cast<T>(s_toLower[static_cast<unsigned char>(*str)]);
        ++str;
    }
}

template <class T>
inline T my_tolower(T _ch)
{
    static std::locale _loc;
    return std::tolower(_ch, _loc);
}

//template <class T>
inline char my_stl_tolower(char _ch, const std::locale _loc)
{
    return std::tolower(_ch, _loc);
}

void ngx_strlow(unsigned char *dest, unsigned char *src, size_t n)
{
    while (n) {
        *dest = ngx_tolower(*src);
        dest++;
        src++;
        n--;
    }
}

void Jimi_StrLwr_Verify()
{
    int i, l = 0;
    int ch, errors = 0;
    size_t size, bufSize, str_len;
    int alignment, offset;
    char *buffer1, *buffer2, *buffer3, *buffer4;
    char *strTest1, *strTest2, *temp;
    jimi::stop_watch sw;
    char checkCharList[] = { '@', 'A', 'Q', 'Z', '[', 'a', 'q', 'z', '0', ' ', '\b' };
    int sizeCharList = jm_countof(checkCharList);

    printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\n");
    printf("Jimi_StrLwr_Verify() start ...\n\n");

    sw.restart();
#if 1
    for (ch = 1; ch < 256; ++ch) {
#else
    for (l = 0; l < sizeCharList; ++l) {
        ch = checkCharList[l];
#endif
        // 校验头部32字节
        alignment = 4;
        bufSize = 256 * sizeof(char);
        buffer1 = (char *)_aligned_offset_malloc(bufSize, alignment, 0);
        buffer2 = (char *)_aligned_offset_malloc(bufSize, alignment, 0);
        buffer3 = (char *)_aligned_offset_malloc(bufSize, alignment, 0);
        buffer4 = (char *)_aligned_offset_malloc(bufSize, alignment, 0);
        for (i = 0; i < (int)bufSize; ++i) {
            //buffer1[i] = ngx_toupper(ch);
            //buffer2[i] = ngx_toupper(ch);
            buffer1[i] = 'A' + (ch & 15);
            buffer2[i] = 'A' + (ch & 15);
        }
        buffer1[bufSize - 1] = '\0';
        buffer2[bufSize - 1] = '\0';
        for (offset = 0; offset < 64; ++offset) {
            for (str_len = 0; str_len < 128; ++str_len) {
                memcpy(buffer3, buffer1, bufSize);
                memcpy(buffer4, buffer2, bufSize);

                strTest1 = buffer3 + offset;
                memset((void *)strTest1, ch, str_len);
                strTest1[str_len] = '\0';
                temp = jm_strlwr(strTest1, str_len + 1);
                //temp = strlwr(strTest1);

                if (ch == 1 && offset == 0 && str_len == 20) {
                    //_getch();
                    ch = ch;
                }

                if (ch == 'A' && offset == 28 && str_len == 24) {
                    //_getch();
                    ch = ch;
                }

                if (ch == 'A' && offset == 33 && str_len == 25) {
                    ch = ch;
                }

                if (ch == 'A' && offset == 32 && str_len == 24) {
                    ch = ch;
                }

                strTest2 = buffer4 + offset;
                memset((void *)strTest2, ch, str_len);
                strTest2[str_len] = '\0';
                size = jmf_strlwr(strTest2);

                if (memcmp(buffer3, buffer4, bufSize) != 0) {
                    // got some errors
                    printf("Error at char of: \'%c\', ch = %d, offset = %d, strlen = %d\n", ch, ch, offset, str_len);
                    printf("strTest1 = %s\n", strTest1);
                    printf("strTest2 = %s\n", strTest2);
                    errors++;
                    jimi_getch();
                    printf("\n");
                }
            }
        }
        if (buffer1) _aligned_free(buffer1);
        if (buffer2) _aligned_free(buffer2);
        if (buffer3) _aligned_free(buffer3);
        if (buffer4) _aligned_free(buffer4);
    }
    sw.stop();

    if (errors == 0)
        printf("All verify is passed ... [time = %0.3f ms]\n", sw.getMillisec());

    printf("\n");
    printf("Jimi_StrLwr_Verify() done ...\n\n");
    printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\n");
}

/* 每次进行strlwr转换时是否重新还原字符串到初始状态? */
#define STRLWR_RECOPY_EVERY_TIME        0

void String_StrLwr_Test(int nTestLen)
{
    int i;
    static const int alignment = 8;
#if defined(_DEBUG) || 0
    static const int loop_times = 8;
#else
    static const int loop_times = 200000;
#endif
    int nBufLen, nStrLen;
    char *tolower_test1, *tolower_test2, *tolower_test3;
    char *tolower_test4, *tolower_test5, *tolower_test6, *tolower_test7;
    char *result_str = NULL;
    double time1, time2, time3, time4, time5, time6, time7;
    jimi::stop_watch_ex sw;

    tolower_test1 = NULL;
    tolower_test2 = NULL;
    tolower_test3 = NULL;
    tolower_test4 = NULL;
    tolower_test5 = NULL;
    tolower_test6 = NULL;

    time1 = 0.0;
    time2 = 0.0;
    time3 = 0.0;
    time4 = 0.0;
    time5 = 0.0;
    time6 = 0.0;

    nStrLen = ::jm_strlen(jabberwocky);
    if (nTestLen > nStrLen)
        nTestLen = nStrLen;
    nBufLen = nTestLen + 32;
    if (nBufLen < 64)
        nBufLen = 64;

#if 1
    tolower_test1 = (char *)::_aligned_malloc(nBufLen * sizeof(char), alignment);
    if (tolower_test1) {
        ::jm_strncpy(tolower_test1, nBufLen, jabberwocky, nTestLen);
        tolower_test1[nTestLen] = '\0';
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
#if defined(STRLWR_RECOPY_EVERY_TIME) && (STRLWR_RECOPY_EVERY_TIME != 0)
            sw.suspend();
            ::jm_strncpy(tolower_test1, nBufLen, jabberwocky, nTestLen);
            tolower_test1[nTestLen] = '\0';
            sw.resume();
#endif
            result_str = ::strlwr(tolower_test1);
        }
        sw.stop();
        time1 = sw.getMillisec();
        //time1 = sw.getTotalMillisec();
        //printf("tolower_test1.c_str() = \n%s\n\ntolower_test1.size() = %d bytes\n", tolower_test1, ::jm_strlen(tolower_test1));
    }
    //printf("\n");

    tolower_test2 = (char *)::_aligned_malloc(nBufLen * sizeof(char), alignment);
    if (tolower_test2) {
        ::jm_strncpy(tolower_test2, nBufLen, jabberwocky, nTestLen);
        tolower_test2[nTestLen] = '\0';
        int str_len = ::jm_strlen(tolower_test2) + 1;
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
#if defined(STRLWR_RECOPY_EVERY_TIME) && (STRLWR_RECOPY_EVERY_TIME != 0)
            sw.suspend();
            ::jm_strncpy(tolower_test2, nBufLen, jabberwocky, nTestLen);
            tolower_test2[nTestLen] = '\0';
            sw.resume();
#endif
            result_str = ::jm_strlwr(tolower_test2, str_len);
        }
        sw.stop();
        time2 = sw.getMillisec();
        //time2 = sw.getTotalMillisec();
        //printf("tolower_test2.c_str() = \n%s\n\ntolower_test2.size() = %d bytes\n", tolower_test2, ::jm_strlen(tolower_test2));
    }
    //printf("\n");

    tolower_test3 = (char *)::_aligned_malloc(nBufLen * sizeof(char), alignment);
    if (tolower_test3) {
        ::jm_strncpy(tolower_test3, nBufLen, jabberwocky, nTestLen);
        tolower_test3[nTestLen] = '\0';
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
#if defined(STRLWR_RECOPY_EVERY_TIME) && (STRLWR_RECOPY_EVERY_TIME != 0)
            sw.suspend();
            ::jm_strncpy(tolower_test3, nBufLen, jabberwocky, nTestLen);
            tolower_test3[nTestLen] = '\0';
            sw.resume();
#endif
            ngx_strlwr((unsigned char *)tolower_test3);
        }
        sw.stop();
        time3 = sw.getMillisec();
        //time3 = sw.getTotalMillisec();
        //printf("tolower_test3.c_str() = \n%s\n\tolower_test3.size() = %d bytes\n", tolower_test3, ::jm_strlen(tolower_test3));
    }
    //printf("\n");

    tolower_test4 = (char *)::_aligned_malloc(nBufLen * sizeof(char), alignment);
    if (tolower_test4) {
        ::jm_strncpy(tolower_test4, nBufLen, jabberwocky, nTestLen);
        tolower_test4[nTestLen] = '\0';
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
#if defined(STRLWR_RECOPY_EVERY_TIME) && (STRLWR_RECOPY_EVERY_TIME != 0)
            sw.suspend();
            ::jm_strncpy(tolower_test4, nBufLen, jabberwocky, nTestLen);
            tolower_test4[nTestLen] = '\0';
            sw.resume();
#endif
            strlwr_std(tolower_test4);
        }
        sw.stop();
        time4 = sw.getMillisec();
        //time4 = sw.getTotalMillisec();
        //printf("tolower_test4.c_str() = \n%s\n\tolower_test4.size() = %d bytes\n", tolower_test4, ::jm_strlen(tolower_test4));
    }
    //printf("\n");

    tolower_test5 = (char *)::_aligned_malloc(nBufLen * sizeof(char), alignment);
    if (tolower_test5) {
        ::jm_strncpy(tolower_test5, nBufLen, jabberwocky, nTestLen);
        tolower_test5[nTestLen] = '\0';
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
#if defined(STRLWR_RECOPY_EVERY_TIME) && (STRLWR_RECOPY_EVERY_TIME != 0)
            sw.suspend();
            ::jm_strncpy(tolower_test5, nBufLen, jabberwocky, nTestLen);
            tolower_test5[nTestLen] = '\0';
            sw.resume();
#endif
            strlwr_table(tolower_test5);
        }
        sw.stop();
        time5 = sw.getMillisec();
        //time5 = sw.getTotalMillisec();
        //printf("tolower_test5.c_str() = \n%s\n\tolower_test5.size() = %d bytes\n", tolower_test5, ::jm_strlen(tolower_test5));
    }
    //printf("\n");

#if !defined(_MSC_VER) || (_MSC_VER >= 1600)
    using namespace std::placeholders;
    tolower_test6 = (char *)::_aligned_malloc(nBufLen * sizeof(char), alignment);
    if (tolower_test6) {
        char *begin, *end;
        ::jm_strncpy(tolower_test6, nBufLen, jabberwocky, nTestLen);
        tolower_test6[nTestLen] = '\0';
        begin = tolower_test6;
        end = tolower_test6 + jm_strlen(tolower_test6);
        std::string strlow = tolower_test6;
        std::locale loc;
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
#if defined(STRLWR_RECOPY_EVERY_TIME) && (STRLWR_RECOPY_EVERY_TIME != 0)
            sw.suspend();
            ::jm_strncpy(tolower_test6, nBufLen, jabberwocky, nTestLen);
            tolower_test6[nTestLen] = '\0';
            sw.resume();
#endif
            //std::for_each(strlow.begin(), strlow.end(), std::bind(std::tolower<char>, _1, loc));
            //std::for_each(strlow.begin(), strlow.end(), std::bind(std::toupper<char>, _1, loc));
            std::for_each(strlow.begin(), strlow.end(), std::tolower);
            //std::transform(strlow.begin(), strlow.end(), strlow.begin(), ::tolower);
        }
        sw.stop();
        time6 = sw.getMillisec();
        //time5 = sw.getTotalMillisec();
        //printf("tolower_test6.c_str() = \n%s\n\tolower_test6.size() = %d bytes\n", tolower_test6, ::jm_strlen(tolower_test6));
#if 0
            //stl_tolower<char>(begin, end);
            typedef char (*fn_tolower)(char _Ch, const std::locale& _Loc);
            typedef char (*fn_tolower2)(char _Ch);
            typedef std::function<char (char, const std::locale&)> fcn_tolower;
            //fn_tolower pfn_tolower = &std::tolower;
            //stl_tolower<char, fn_tolower>(strlow);
            //fn_tolower2 pfn_tolower = &my_tolower;
            //std::for_each<std::string::const_iterator, fn_tolower2>(strlow.begin(), strlow.end(), pfn_tolower);
            //std::for_each<std::string::const_iterator, fcn_tolower>(strlow.begin(), strlow.end(), std::bind(my_stl_tolower, _1, loc));
            //std::for_each(strlow.begin(), strlow.end(), std::bind(my_stl_tolower, _1, loc));
            std::for_each(strlow.begin(), strlow.end(), std::bind(std::tolower<char>, _1, loc));
            //std::transform(strlow.begin(), strlow.end(), strlow.begin(), ::tolower);
            //std::transform(strlow.begin(), strlow.end(), strlow.begin(), std::tolower);
#endif
    }
    //printf("\n");
#endif  /* !defined(_MSC_VER) || (_MSC_VER >= 1600) */

#endif

#if 0
    tolower_test7 = (char *)::_aligned_offset_malloc(nBufLen * sizeof(char), alignment, (alignment - 4));
#else
    tolower_test7 = (char *)::_aligned_malloc(nBufLen * sizeof(char), alignment);
#endif
    if (tolower_test7) {
        size_t size;
        ::jm_strncpy(tolower_test7, nBufLen - 1, jabberwocky, nTestLen);
        tolower_test7[nTestLen] = '\0';
        sw.restart();
        for (i = 0; i < loop_times; ++i) {
#if defined(STRLWR_RECOPY_EVERY_TIME) && (STRLWR_RECOPY_EVERY_TIME != 0)
            sw.suspend();
            if (::jm_strncpy(tolower_test7, nBufLen - 1, jabberwocky, nTestLen))
                tolower_test7[nTestLen] = '\0';
            sw.resume();
#endif
            size = ::jmf_strlwr(tolower_test7);
            //size = ::jmf_strlen(tolower_test7);
        }
        sw.stop();
        time7 = sw.getMillisec();
        //time6 = sw.getTotalMillisec();
        //printf("tolower_test7.c_str() = \n%s\n\tolower_test7.size() = %d bytes\n", tolower_test7, size);
    }
    //printf("\n");

    printf("string            size = %d bytes\n\n", nTestLen);

    if (loop_times > 100) {
        printf("strlwr(str)       time = %-7.3f ms\n", time1);
        printf("strlwr_s(str)     time = %-7.3f ms\n", time2);
        printf("ngx_strlwr(str)   time = %-7.3f ms\n", time3);
        printf("stl_tolower(str)  time = %-7.3f ms\n", time6);
        printf("strlwr_std(str)   time = %-7.3f ms\n", time4);
        printf("strlwr_table(str) time = %-7.3f ms\n", time5);
        printf("jmf_strlwr(str)   time = %-7.3f ms\n", time7);
        //printf("jmf_strlwr(str)   time = %-7.3f ms, size = %d\n", time7, jm_strlen(tolower_test7));
    }
    else {
        printf("strlwr(str)       time = %-9.6f ms\n", time1);
        printf("strlwr_s(str)     time = %-9.6f ms\n", time2);
        printf("ngx_strlwr(str)   time = %-9.6f ms\n", time3);
        printf("stl_tolower(str)  time = %-7.3f ms\n", time6);
        printf("strlwr_std(str)   time = %-9.6f ms\n", time4);
        printf("strlwr_table(str) time = %-9.6f ms\n", time5);
        printf("jmf_strlwr(str)   time = %-9.6f ms\n", time7);
    }
    printf("\n");

    if (tolower_test1) {
        ::_aligned_free(tolower_test1);
        tolower_test1 = NULL;
    }
    if (tolower_test2) {
        ::_aligned_free(tolower_test2);
        tolower_test2 = NULL;
    }
    if (tolower_test3) {
        ::_aligned_free(tolower_test3);
        tolower_test3 = NULL;
    }
    if (tolower_test4) {
        ::_aligned_free(tolower_test4);
        tolower_test4 = NULL;
    }
    if (tolower_test5) {
        ::_aligned_free(tolower_test5);
        tolower_test5 = NULL;
    }
    if (tolower_test6) {
        ::_aligned_free(tolower_test6);
        tolower_test6 = NULL;
    }
    if (tolower_test7) {
#if 0
        uintptr_t pv = (uintptr_t)tolower_test7;
        pv = (pv + (sizeof(uintptr_t) - 1)) & ~(sizeof(uintptr_t) - 1);
        uintptr_t *pvReal = (uintptr_t *)pv - 1;
#ifdef _DEBUG
        pvReal--;
#endif
        printf("pv = 0x%08X, pvReal = 0x%08X, *pvReal = 0x%08X\n\n", pv, pvReal, *pvReal);
        ::system("pause");
#endif
        ::_aligned_free(tolower_test7);
        tolower_test7 = NULL;
    }
}
