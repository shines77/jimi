

#include "String_StrLen_Test.h"

#include <jimi/core/jimi_def.h>
#include <jimi/system/stop_watch.h>

#include <jimic/string/jm_strings.h>
#include <jimic/string/jmf_strings.h>

#include <jimi/lang/CharTraits.h>

#if _MSC_VER
#if !defined(JIMIC_MSVC_CLANG) || (JIMIC_MSVC_CLANG == 0)
#include <intrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#endif
#else
#include <xmmintrin.h>
#include <emmintrin.h>
#endif  /* _MSC_VER */

#if !defined(JIMIC_MSVC_CLANG) || (JIMIC_MSVC_CLANG == 0)

/**
 * for asmlib header file
 */
#include <asmlib/asmlib.h>

/**
 * for asmlib lib import
 */
#pragma comment(lib, "libacof32.lib")

#else

size_t A_strlen (const char * str)
{
    return 0;
}

#endif

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

/* 从size转换成KB, MB, GB, TB */

int get_bytes_display(char *buf, size_t buf_size, uint32_t size)
{
    int result;
    if (size <= 8192)
        result = jm_sprintf(buf, buf_size, "%d", size);
    else if (size < 1024 * 1024)
        result = jm_sprintf(buf, buf_size, "%d K", size / 1024);
    else if (size < 1024 * 1024 * 1024)
        result = jm_sprintf(buf, buf_size, "%d M", size / (1024 * 1024));
    else
        result = jm_sprintf(buf, buf_size, "%d G", size / (1024 * 1024 * 1024));
    return result;
}

/* 从size转换成KB, MB, GB, TB */

int get_bytes_display64(char *buf, size_t buf_size, uint64_t size)
{
    int result;
    if (size <= 8192ULL)
        result = jm_sprintf(buf, buf_size, "%d", size);
    else if (size < 0x0000000000100000ULL)
        result = jm_sprintf(buf, buf_size, "%d K", size / 1024ULL);
    else if (size < 0x0000000040000000ULL)
        result = jm_sprintf(buf, buf_size, "%d M", size / 0x0000000000100000ULL);
    else if (size < 0x0000010000000000ULL)
        result = jm_sprintf(buf, buf_size, "%d G", size / 0x0000000040000000ULL);
    else
        result = jm_sprintf(buf, buf_size, "%d T", size / 0x0000010000000000ULL);
    return result;
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

//
// From: http://www.strchr.com/optimized_strlen_function
//

int strlen_AgnerFog(const char* s) {

#if !defined(JIMIC_MSVC_CLANG) || (JIMIC_MSVC_CLANG == 0)

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

#endif
}

__declspec(naked)
inline int __cdecl __builtin_clz(int bitmask)
{
#if !defined(JIMIC_MSVC_CLANG) || (JIMIC_MSVC_CLANG == 0)
    __asm {
        mov eax, dword ptr [esp + 4]
        bsr eax, eax
        neg eax
        add eax, 31
        ret
    }
#endif
}

__declspec(naked)
inline int __cdecl __builtin_ctz(int bitmask)
{
#if !defined(JIMIC_MSVC_CLANG) || (JIMIC_MSVC_CLANG == 0)
    __asm {
        mov eax, dword ptr [esp + 4]
        bsf eax, eax
        ret
    }
#endif
}

//
// From: http://www.strchr.com/strcmp_and_strlen_using_sse_4.2
//
size_t sse2_strlen(const char* s)
{
#if (!defined(JIMIC_MSVC_CLANG) || (JIMIC_MSVC_CLANG == 0))
    // && (defined(_MSC_VER) && (_MSC_VER >= 1600))
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
#else
    return 0;
#endif
}

//
// From: http://www.strchr.com/optimized_strlen_function?allcomments=1#comment_634
//

#define EQUAL_ANY	        0x00            // 0000b
#define RANGES		        0x04            // 0100b
#define EQUAL_EACH	        0x08            // 1000b
#define EQUAL_ORDERED	    (0x8+0x04)      // 1100b
#define NEGATIVE_POLARITY   0x10            // 010000b
#define BYTE_MASK	        0x40            // 1000000b

size_t sse4_strlen(const char *str)
{
   size_t len = 0;
   int idx = 0;
#if !defined(JIMIC_MSVC_CLANG) || (JIMIC_MSVC_CLANG == 0)

   __m128i z, c;

   z = _mm_setzero_si128();

   while (true) {
      c = _mm_loadu_si128( (const __m128i*)(str) ); 
      idx = _mm_cmpistri(z, c, EQUAL_EACH);   //returns index of the first byte that matches
      if (idx < sizeof(__m128i)) break;
      len += sizeof(__m128i);
      str += sizeof(__m128i); 
   }
#endif
   return len + idx;
}

void String_StrLen_Test()
{
#if !defined(JIMIC_MSVC_CLANG) || (JIMIC_MSVC_CLANG == 0)

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
    jimi::stop_watch sw;

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

#endif
}
