
#ifndef _JIMIC_STRING_ITOA_FAST_INL_H_
#define _JIMIC_STRING_ITOA_FAST_INL_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef _JIMIC_STRING_ITOA_FAST_H_
#error "Don't include the internal file <jimic/string/itoa_fast.inl.h> directly; include <jimic/string/itoa_fast.h> instead."
#endif

#include "jimic/stdio/sprintf_def.h"
#include "jimic/libc/int64.h"
#include "jimic/libc/endian.h"

#include "jimic/math/int_log10.h"
#include "jimic/string/string.h"
#include "jimic/string/jm_strings.h"

#include "jimic/basic/assert.h"

#include <stdarg.h>
#include <math.h>       // for isnan(), isinf()
#include <float.h>
#include <limits.h>     // for UINT_MAX

static const unsigned int s_power10[16] = {
    9, 99, 999, 9999, 99999,
    999999, 9999999, 99999999,
    999999999, 4294967295,
    // Only fill for aligned to 64 bytes
    0, 0, 0, 0, 0, 0
};

static const uint64_t s_power10_64[16] = {
    9999999999,                 // 10
    99999999999,                // 11
    999999999999,               // 12
    9999999999999,              // 13
    99999999999999,             // 14
    999999999999999,            // 15
    9999999999999999,           // 16
    99999999999999999,          // 17
    999999999999999999,         // 18
    18446744073709551615,       // 19
    // Only fill for aligned to 64 bytes
    0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL
};

JMC_INLINE_NONSTD(int)
jmc_utoa_r10_fast(char *buf, unsigned int val)
{
    unsigned int digit_val, num_digits;
    unsigned int exponent, exp10 = 0;
    unsigned char isNonzero;
    char *cur;

    jimic_assert(buf != NULL);

#if 0
    num_digits = jmc_uint_log10(val) + 1;
#elif 0
    //
    // _BitScanReverse, _BitScanReverse64
    // Reference: http://msdn.microsoft.com/en-us/library/fbxyd7zd.aspx
    //
    isNonzero = (unsigned char)_BitScanReverse((unsigned long *)&exponent, (unsigned long)val);
    if (isNonzero == 0) {
        num_digits = 1;
    }
    else {
        // Must be 2,525,222 < 4,194,304 ( 2^32 / 1024)
        // exp10 = exponent * 2525222UL;
        num_digits = exponent * 2525222UL;
        // exp10 = exp10 / 131072 / 64;
        num_digits = (num_digits >> 23);

        if (val > s_power10[num_digits])
            num_digits++;
        num_digits++;
    }
#elif 0
    //
    // _BitScanReverse, _BitScanReverse64
    // Reference: http://msdn.microsoft.com/en-us/library/fbxyd7zd.aspx
    //
    num_digits = 1;

    isNonzero = (unsigned char)_BitScanReverse((unsigned long *)&exponent, (unsigned long)val);
    if (isNonzero != 0) {
        // Must be 2,525,222 < 4,194,304 ( 2^32 / 1024)
        // exp10 = exponent * 2525222UL;
        exp10 = exponent * 2525222UL;
        // exp10 = exp10 / 131072 / 64;
        exp10 = (num_digits >> 23);

        if (val > s_power10[exp10])
            exp10++;
        num_digits += exp10;
    }
#elif 0
    //
    // _BitScanReverse, _BitScanReverse64
    // Reference: http://msdn.microsoft.com/en-us/library/fbxyd7zd.aspx
    //
    num_digits = 1;

    if (val < 100) {
        if (val >= 10)
            num_digits++;
    }
    else {
        isNonzero = (unsigned char)_BitScanReverse((unsigned long *)&exponent, (unsigned long)val);

        // Must be 2,525,222 < 4,194,304 ( 2^32 / 1024)
        // exp10 = exponent * 2525222UL;
        exp10 = exponent * 2525222UL;
        // exp10 = exp10 / 131072 / 64;
        exp10 = (num_digits >> 23);

        if (val > s_power10[exp10])
            exp10++;
        num_digits += exp10;
    }
#else
    //
    // _BitScanReverse, _BitScanReverse64
    // Reference: http://msdn.microsoft.com/en-us/library/fbxyd7zd.aspx
    //
    if (val < 100) {
        num_digits = 1;
        if (val >= 10)
            num_digits++;
    }
    else {
        isNonzero = (unsigned char)_BitScanReverse((unsigned long *)&exponent, (unsigned long)val);

        // Must be 2,525,222 < 4,194,304 ( 2^32 / 1024)
        // exp10 = exponent * 2525222UL;
        num_digits = exponent * 2525222UL;
        // exp10 = exp10 / 131072 / 64;
        num_digits = (num_digits >> 23);

        if (val > s_power10[num_digits])
            num_digits++;
        num_digits++;
    }
#endif

    cur = (char *)buf + num_digits;
    *cur = '\0';

    do {
        digit_val = val % 10;
        val /= 10;

        cur--;
        *cur = (char)(digit_val + '0');

    } while (val != 0);

    return num_digits;
}

JMC_INLINE_NONSTD(int)
jmc_utoa_r10_ultra(char *buf, unsigned int val)
{
    unsigned int digit_val, num_digits;
    unsigned int exponent, exp10 = 0;
    unsigned char isNonzero;
    char *cur;

    unsigned int offset;
    unsigned int decimalOrig;
    unsigned int decimalInt, decimalHigh = 0;

    jimic_assert(buf != NULL);

#if 0
    num_digits = jmc_uint_log10(val) + 1;
#elif 0
    //
    // _BitScanReverse, _BitScanReverse64
    // Reference: http://msdn.microsoft.com/en-us/library/fbxyd7zd.aspx
    //
    num_digits = 1;

    isNonzero = (unsigned char)_BitScanReverse((unsigned long *)&exponent, (unsigned long)val);
    if (isNonzero != 0) {
        // Must be 2,525,222 < 4,194,304 ( 2^32 / 1024)
        // exp10 = exponent * 2525222UL;
        exp10 = exponent * 2525222UL;
        // exp10 = exp10 / 131072 / 64;
        exp10 = (num_digits >> 23);

        if (val > s_power10[exp10])
            exp10++;
        num_digits += exp10;
    }
#else
    //
    // _BitScanReverse, _BitScanReverse64
    // Reference: http://msdn.microsoft.com/en-us/library/fbxyd7zd.aspx
    //
    if (val < 100) {
        num_digits = 1;
        if (val >= 10)
            num_digits++;
    }
    else {
        isNonzero = (unsigned char)_BitScanReverse((unsigned long *)&exponent, (unsigned long)val);

        // Must be 2,525,222 < 4,194,304 ( 2^32 / 1024)
        // exp10 = exponent * 2525222UL;
        num_digits = exponent * 2525222UL;
        // exp10 = exp10 / 131072 / 64;
        num_digits = (num_digits >> 23);

        if (val > s_power10[num_digits])
            num_digits++;
        num_digits++;
    }
#endif

#if JIMIC_BYTE_ORDER == JIMIC_BIG_ENDIAN
    // Calc decimal low 1-4 digits
    do {
        digit_val = val % 10;
        val /= 10;

        decimalInt = (digit_val + (unsigned int)'0');

        digit_val = val % 10;
        val /= 10;

        decimalInt |= (digit_val + (unsigned int)'0') << 8;

        digit_val = val % 10;
        val /= 10;

        decimalInt |= (digit_val + (unsigned int)'0') << 16;

        digit_val = val % 10;
        val /= 10;

        decimalInt |= (digit_val + (unsigned int)'0') << 24;
    } while (0);

#if 0
    cur = (char *)buf + num_digits;
    offset = ((unsigned int)cur) & 3U;

    if (offset == 0) {  // The address of the null terminator is aligned to 0
        // Example:
        //      0123
        //  ABCD

        // Write the null terminator
        *cur = '\0';

        decimalHigh = 0;

        // Write the decimalInt(uint32_t) to buffer directly
        cur -= 0 + sizeof(unsigned int);
        *((unsigned int *)cur) = decimalInt;
    }
    else if (offset == 1) {   // The address of the null terminator is aligned to 1
        // Example:
        //     0123
        //  ABCD

        decimalHigh = decimalInt >> 8;
        decimalOrig = *((unsigned int *)((unsigned int)cur - offset));

        // Write the decimalInt(uint32_t) to buffer directly
        cur -= 1;
        // Mask = 0x0000FFFF
        *((unsigned int *)cur) = (decimalInt << 24) | (decimalOrig & 0x0000FFFFU);
    }
    else if (offset == 2) {   // The address of the null terminator is aligned to 2
        // Example:
        //    0123
        //  ABCD

        decimalHigh = decimalInt >> 16;
        decimalOrig = *((unsigned int *)((unsigned int)cur - offset));

        // Write the decimalInt(uint32_t) to buffer directly
        cur -= 2;
        *((unsigned int *)cur) = (decimalInt << 16) | (decimalOrig & 0x000000FFU);
    }
    else if (offset == 3) {   // The address of the null terminator is aligned to 3
        // Example:
        //   0123
        //  ABCD

        decimalHigh = decimalInt >> 24;

        // Write the decimalInt(uint32_t) to buffer directly
        cur -= 3;
        *((unsigned int *)cur) = (decimalInt << 8);
    }
#else
    cur = (char *)buf + num_digits;
    offset = ((unsigned int)cur) & 3U;

    if (offset == 0) {  // The address of the null terminator is aligned to 0
        // Example:
        //      0123
        //  ABCD

        // Write the null terminator
        *cur = '\0';

        decimalHigh = 0;

        // Write the decimalInt(uint32_t) to buffer directly
        cur -= 0 + sizeof(unsigned int);
        *((unsigned int *)cur) = decimalInt;
    }
    else if (offset == 3) {   // The address of the null terminator is aligned to 3
        // Example:
        //   0123
        //  ABCD

        decimalHigh = decimalInt >> 24;

        // Write the decimalInt(uint32_t) to buffer directly
        cur -= 3;
        *((unsigned int *)cur) = (decimalInt << 8);
    }
    else {   // The address of the null terminator is aligned to 1 or 2
        // Example:
        //     0123
        //  ABCD
        //
        // Or:
        //     0123
        //   ABCD
        unsigned int decimalMask;

        // offsetLast = offset - 1
        //unsigned int offsetLast = (offset + 3) & 3U;

        decimalOrig = *((unsigned int *)((unsigned int)cur - offset));
        decimalMask = 0xFFFFFFFFU >> (offset * 8 + 8);

        //decimalHigh = decimalInt >> ((offsetLast + 1) * 8);
        //decimalInt  = (decimalInt << (32 - (offsetLast + 1) * 8)) | (decimalOrig & decimalMask);

        decimalHigh = decimalInt >> (offset * 8);
        decimalInt  = (decimalInt << (32 - offset * 8)) | (decimalOrig & decimalMask);

        // Write the decimalInt(uint32_t) to buffer directly
        cur -= offset;
        *((unsigned int *)cur) = decimalInt;
    }
#endif

#else  /* JIMIC_BYTE_ORDER == JIMIC_LITTLE_ENDIAN */

    cur = (char *)buf + num_digits;
    offset = ((unsigned int)cur) & 3U;

#if 0
    // Calc the decimal low 1-4 digits
    do {
        int digits_cnt;
        decimalInt = 0;
        digits_cnt = offset;
        while (digits_cnt > 0) {
            digit_val = val % 10;
            val /= 10;

            decimalInt <<= 8;
            decimalInt += digit_val;
            digits_cnt--;
        }
    } while (0);
#endif

#if 1
    if (offset == 0) {  // The address of the null terminator is aligned to 0
        // Example:
        //      0123
        //  ABCD

        // Write the null terminator
        *cur = '\0';
    }
    else if (offset == 1) {   // The address of the null terminator is aligned to 1
        // Example:
        //     0123
        //  ABCD

        // Calc the decimal low 1 digits
        do {
            digit_val = val % 10;
            val /= 10;

            decimalInt = digit_val;
        } while (0);

        cur -= 1;
        decimalOrig = *((unsigned int *)cur);

        // Write the decimalInt(uint32_t) to buffer directly
        // Mask = 0xFFFF0000
        *((unsigned int *)cur) = (decimalInt | 0x00000030U) | (decimalOrig & 0xFFFF0000U);
    }
    else if (offset == 2) {   // The address of the null terminator is aligned to 2
        // Example:
        //    0123
        //  ABCD

        // Calc the decimal low 2 digits
        do {
            digit_val = val % 10;
            val /= 10;

            decimalInt = digit_val << 8;

            digit_val = val % 10;
            val /= 10;

            decimalInt |= digit_val;
        } while (0);

        cur -= 2;
        decimalOrig = *((unsigned int *)cur);

        // Write the decimalInt(uint32_t) to buffer directly
        // Mask = 0xFF000000
        *((unsigned int *)cur) = (decimalInt | 0x00003030U) | (decimalOrig & 0xFF000000U);
    }
    else if (offset == 3) {   // The address of the null terminator is aligned to 3
        // Example:
        //   0123
        //  ABCD

        // Calc the decimal low 3 digits
        do {
            digit_val = val % 10;
            val /= 10;

            decimalInt = digit_val << 16;

            digit_val = val % 10;
            val /= 10;

            decimalInt |= digit_val << 8;

            digit_val = val % 10;
            val /= 10;

            decimalInt |= digit_val;
        } while (0);

        // Write the decimalInt(uint32_t) to buffer directly
        cur -= 3;
        *((unsigned int *)cur) = (decimalInt | 0x00303030U);
    }
#else
    if (offset == 0) {  // The address of the null terminator is aligned to 0
        // Example:
        //      0123
        //  ABCD

        // Write the null terminator
        *cur = '\0';

        decimalHigh = 0;

        // Write the decimalInt(uint32_t) to buffer directly
        cur -= 0 + sizeof(unsigned int);
        *((unsigned int *)cur) = decimalInt;
    }
    else if (offset == 3) {   // The address of the null terminator is aligned to 3
        // Example:
        //   0123
        //  ABCD

        decimalHigh = decimalInt << 24;

        // Write the decimalInt(uint32_t) to buffer directly
        cur -= 3;
        *((unsigned int *)cur) = (decimalInt >> 8);
    }
    else {   // The address of the null terminator is aligned to 1 or 2
        // Example:
        //     0123
        //  ABCD
        //
        // Or:
        //     0123
        //   ABCD
        unsigned int decimalMask;

        // offsetLast = offset - 1
        //unsigned int offsetLast = (offset + 3) & 3U;

        decimalOrig = *((unsigned int *)((unsigned int)cur - offset));
        decimalMask = 0xFFFFFFFFU << (offset * 8 + 8);

        //decimalHigh = decimalInt << ((offsetLast + 1) * 8);
        //decimalInt  = (decimalInt >> (32 - (offsetLast + 1) * 8)) | (decimalOrig & decimalMask);

        decimalHigh = decimalInt << (offset * 8);
        decimalInt  = (decimalInt >> (32 - offset * 8)) | (decimalOrig & decimalMask);

        // Write the decimalInt(uint32_t) to buffer directly
        cur -= offset;
        *((unsigned int *)cur) = decimalInt;
    }
#endif

calc_digits_main:

    cur -= sizeof(unsigned int);
    if (cur >= buf) {
        // Calc the decimal middle 4 digits
        do {
            digit_val = val % 10;
            val /= 10;

            decimalInt = digit_val << 24;

            digit_val = val % 10;
            val /= 10;

            decimalInt |= digit_val << 16;

            digit_val = val % 10;
            val /= 10;

            decimalInt |= digit_val << 8;

            digit_val = val % 10;
            val /= 10;

            decimalInt |= digit_val;
        } while (0);

        *((unsigned int *)cur) = (decimalInt | 0x30303030U);

        // Calc the next 4 digits
        goto calc_digits_main;
    }
    else {
        // Get the offset of the buff aligned to 4 byte
        offset = ((unsigned int)buf) & 3U;
        ///*
        if (offset == 0)
            return num_digits;
        //*/
        jimic_assert(offset != 0);

#if 0
        // Calc the decimal low 1-4 digits
        do {
            int digits_cnt;
            decimalInt = 0;
            digits_cnt = (int)(offset ^ 3U);
            while (digits_cnt >= 0) {
                digit_val = val % 10;
                val /= 10;

                decimalInt <<= 8;
                decimalInt += digit_val;
                digits_cnt--;
            }
        } while (0);
#endif

        //decimalOrig = *((unsigned int *)((unsigned int)buf - offset));

        if (offset == 1) {   // The address of the buff is aligned to 1
            // Example:
            //  0123
            //   ABCD
            jimic_assert(offset == 1);

            // Calc the decimal high 3 digits
            do {
                digit_val = val % 10;
                val /= 10;

                decimalInt = digit_val << 16;

                digit_val = val % 10;
                val /= 10;

                decimalInt |= digit_val << 8;

                digit_val = val % 10;
                val /= 10;

                decimalInt |= digit_val;
            } while (0);

            decimalOrig = *((unsigned int *)((unsigned int)buf - 1));

            // Write the decimalInt(uint32_t) to buffer directly
            // Mask = 0x000000FF
            *((unsigned int *)cur) = ((decimalInt << 8) | 0x30303000U) | (decimalOrig & 0x000000FFU);
        }
        else if (offset == 2) {   // The address of the buff is aligned to 2
            // Example:
            //  0123
            //    ABCD
            jimic_assert(offset == 2);

            // Calc the decimal high 2 digits
            do {
                digit_val = val % 10;
                val /= 10;

                decimalInt = digit_val << 8;

                digit_val = val % 10;
                val /= 10;

                decimalInt |= digit_val;
            } while (0);

            decimalOrig = *((unsigned int *)((unsigned int)buf - 2));

            // Write the decimalInt(uint32_t) to buffer directly
            // Mask = 0x0000FFFF
            *((unsigned int *)cur) = ((decimalInt << 16) | 0x30300000U) | (decimalOrig & 0x0000FFFFU);
        }
        else {  // The address of the buff is aligned to 3
            // Example:
            //  0123
            //     ABCD
            jimic_assert(offset == 3);

            // Calc the decimal high 1 digits
            do {
                digit_val = val % 10;
                val /= 10;

                decimalInt = digit_val;
            } while (0);

            *buf = (char)(decimalInt + '0');

            // Write the decimalInt(uint32_t) to buffer directly
            //*((unsigned int *)cur) = ((decimalInt << 24) | 0x30000000U) | (decimalOrig & 0x00FFFFFFU);
        }

    }

#endif  /* JIMIC_BYTE_ORDER == JIMIC_BIG_ENDIAN */

    return num_digits;
}

JMC_INLINE_NONSTD(int)
jmc_utoa_r10_fast_v1(char *buf, unsigned int val)
{
    unsigned int digit_val, num_digits;
    const char *end;
    char *cur;
    char digits[16];    // 实际最多只会用到10个bytes

    jimic_assert(buf != NULL);

    end = digits + jm_countof(digits) - 1;
    cur = (char *)end;
    do {
        digit_val = val % 10;
        val /= 10;

        *cur-- = (char)(digit_val + '0');
    } while (val != 0);

    num_digits = (unsigned int)(end - cur);

#if 0
    do {
        ++cur;
        *buf++ = *cur;
    } while (cur != end);
#else
    cur++;
    while (cur <= end)
        *buf++ = *cur++;
#endif
    *buf = '\0';

    return num_digits;
}

JMC_INLINE_NONSTD(int)
jmc_utoa_r10_fast_v2(char *buf, unsigned int val)
{
    unsigned int digit_val, num_digits;
    char *cur;
    char digits[16];    // 实际最多只会用到10个bytes

    jimic_assert(buf != NULL);

    cur = digits;
    do {
        digit_val = val % 10;
        val /= 10;

        *cur++ = (char)digit_val + '0';
    } while (val != 0);

    num_digits = (unsigned int)(cur - digits);

#if 0
    do {
        --cur;
        *buf++ = *cur;
    } while (cur != digits);
#else
    cur--;
    while (cur >= digits)
        *buf++ = *cur--;
#endif
    *buf = '\0';

    return num_digits;
}

JMC_INLINE_NONSTD(int)
jmc_itoa_r10_fast(char *buf, int val)
{
#if 1
    if (val >= 0) {
        return jmc_utoa_r10_fast(buf, (unsigned int)val);
    }
    else {
        *buf++ = '-';
        val = -val;
        return jmc_utoa_r10_fast(buf, (unsigned int)val) + 1;
    }
#elif 1
    int sign;
    if (val < 0) {
        *buf++ = '-';
        val = -val;
        sign = 1;
    }
    else {
        sign = 0;
    }
    return jmc_utoa_r10_fast(buf, (unsigned int)val) + sign;
#elif 0
    if (val < 0) {
        *buf++ = '-';
        val = -val;
        return jmc_utoa_r10_fast(buf, (unsigned int)val) + 1;
    }
    else
        return jmc_utoa_r10_fast(buf, (unsigned int)val);
#else
    register int sign;
    sign = (val < 0);
    if (val < 0) {
        *buf++ = '-';
        val = -val;
    }
    return jmc_utoa_r10_fast(buf, (unsigned int)val) + sign;
#endif
}

JMC_INLINE_NONSTD(int)
jmc_itoa_r10_ultra(char *buf, int val)
{
    if (val >= 0) {
        return jmc_utoa_r10_ultra(buf, (unsigned int)val);
    }
    else {
        *buf++ = '-';
        val = -val;
        return jmc_utoa_r10_ultra(buf, (unsigned int)val) + 1;
    }
}

JMC_INLINE_NONSTD(int)
jmc_ultoa_r10_fast(char *buf, unsigned long val)
{
    unsigned long digit_val, num_digits;
    char *cur;
    char digits[16];    // 实际最多只会用到10个bytes

    cur = digits;
    do {
        digit_val = val % 10;
        val /= 10;

        *cur++ = (char)digit_val + '0';
    } while (val != 0);

    num_digits = (unsigned long)(cur - digits);

#if 0
    do {
        --cur;
        *buf++ = *cur;
    } while (cur != digits);
#else
    cur--;
    while (cur >= digits)
        *buf++ = *cur--;
#endif
    *buf = '\0';

    return num_digits;
}

JMC_INLINE_NONSTD(int)
jmc_ltoa_r10_fast(char *buf, long val)
{
#if 1
    if (val >= 0) {
        return jmc_ultoa_r10_fast(buf, val);
    }
    else {
        *buf++ = '-';
        val = -val;
        return jmc_ultoa_r10_fast(buf, val) + 1;
    }        
#else
    int sign;
    sign = (val < 0);
    if (val < 0) {
        *buf++ = '-';
        val = -val;
    }
    return jmc_ultoa_r10_fast(buf, val) + sign;
#endif
}

JMC_INLINE_NONSTD(int)
jmc_u64toa_r10_fast(char *buf, uint64_t val)
{
    unsigned int digit_val, num_digits;
    unsigned int digits_cnt;
    unsigned int exponent;
    unsigned char isNonzero;
    uint32_t val32, val32_high;
    char *cur;
    jmc_uint64_t *u64;

#if 0
    if (val <= (uint64_t)JIMIC_UINT_MAX64) {
        val32 = (uint32_t)val;
#else
    u64 = (jmc_uint64_t *)&val;
    if (u64->dwords.high == 0) {
        // Get the low dword in uint64_t
        val32 = (uint32_t)u64->dwords.low;
#endif

#if 0
        num_digits = jmc_uint_log10(val32) + 1;
#else
        //
        // _BitScanReverse, _BitScanReverse64
        // Reference: http://msdn.microsoft.com/en-us/library/fbxyd7zd.aspx
        //
        isNonzero = (unsigned char)_BitScanReverse((unsigned long *)&exponent, (unsigned long)val32);
        if (isNonzero == 0) {
            num_digits = 1;
        }
        else {
            // Must be 2,525,222 < 4,194,304 ( 2^32 / 1024)
            // exp10 = exponent * 2525222UL;
            num_digits = exponent * 2525222UL;
            // exp10 = exp10 / 131072 / 64;
            num_digits = (num_digits >> 23);

            if (val > s_power10[num_digits])
                num_digits++;
            num_digits++;
        }

        cur = (char *)buf + num_digits;
        *cur = '\0';

        do {
            digit_val = val32 % 10;
            val32 /= 10;

            cur--;
            *cur = (char)(digit_val + '0');
        } while (val32 != 0);
#endif
    }
    else {
        val32_high = (uint32_t)u64->dwords.high;

#if 0
        num_digits = jmc_uint64_log10(val32) + 1;
#else
        if (val < 10000000000ULL) {
            num_digits = 10;
        }
        else {
            //
            // _BitScanReverse, _BitScanReverse64
            // Reference: http://msdn.microsoft.com/en-us/library/fbxyd7zd.aspx
            //
            isNonzero = (unsigned char)_BitScanReverse((unsigned long *)&exponent, (unsigned long)val32_high);

            exponent += 32;

            // Must be 2,525,222 < 4,194,304 ( 2^32 / 1024)
            // exp10 = exponent * 2525222UL;
            num_digits = exponent * 2525222UL;
            // exp10 = exp10 / 131072 / 64;
            num_digits = (num_digits >> 23);

            if (val > s_power10_64[num_digits - 10])
                num_digits++;
            num_digits++;
        }

        // At least 9 digits
        digits_cnt = num_digits - 9;

        cur = (char *)buf + num_digits;
        *cur = '\0';

        do {
            digit_val = val % 10;
            val /= 10;

            cur--;
            *cur = (char)(digit_val + '0');

            digits_cnt--;
            jimic_assert(digits_cnt > num_digits);
        } while (digits_cnt > 0 && val != 0);

        jimic_assert(val <= 4294967295ULL);
        val32 = (uint32_t)val;

        do {
            digit_val = val32 % 10;
            val32 /= 10;

            cur--;
            *cur = (char)(digit_val + '0');
        } while (val32 != 0);
#endif
    }

    return num_digits;
}

JMC_INLINE_NONSTD(int)
jmc_i64toa_r10_fast(char *buf, int64_t val)
{
#if 1
    if (val >= 0) {
        return jmc_u64toa_r10_fast(buf, val);
    }
    else {
        *buf++ = '-';
        val = -val;
        return jmc_u64toa_r10_fast(buf, val) + 1;
    }        
#else
    int sign;
    sign = (val < 0);
    if (val < 0) {
        *buf++ = '-';
        val = -val;
    }
    return jmc_u64toa_r10_fast(buf, val) + sign;
#endif
}

JMC_INLINE_NONSTD(int)
jmc_utoa_r10_fast_ex(char *buf, size_t count, unsigned int val, unsigned int flag,
                     unsigned int fill, unsigned int field_width, int length)
{
    unsigned int digit_val, num_digits;
    unsigned int exponent;
    char *cur;
    unsigned char isNonzero;
    int sign_char;
    int fill_cnt;
    int padding;

    jimic_assert(buf != NULL);
    jimic_assert(count != 0);

#if 0
    num_digits = jmc_uint_log10(val) + 1;
#elif 0
    //
    // _BitScanReverse, _BitScanReverse64
    // Reference: http://msdn.microsoft.com/en-us/library/fbxyd7zd.aspx
    //
    isNonzero = (unsigned char)_BitScanReverse((unsigned long *)&exponent, (unsigned long)val);
    if (isNonzero == 0) {
        num_digits = 1;
    }
    else {
        // Must be 2,525,222 < 4,194,304 ( 2^32 / 1024)
        // exp10 = exponent * 2525222UL;
        num_digits = exponent * 2525222UL;
        // exp10 = exp10 / 131072 / 64;
        num_digits = (num_digits >> 23);

        if (val > s_power10[num_digits])
            num_digits++;
        num_digits++;
    }
#else
    //
    // _BitScanReverse, _BitScanReverse64
    // Reference: http://msdn.microsoft.com/en-us/library/fbxyd7zd.aspx
    //
    if (val < 100) {
        num_digits = 1;
        if (val >= 10)
            num_digits++;
    }
    else {
        isNonzero = (unsigned char)_BitScanReverse((unsigned long *)&exponent, (unsigned long)val);

        // Must be 2,525,222 < 4,194,304 ( 2^32 / 1024)
        // exp10 = exponent * 2525222UL;
        num_digits = exponent * 2525222UL;
        // exp10 = exp10 / 131072 / 64;
        num_digits = (num_digits >> 23);

        if (val > s_power10[num_digits])
            num_digits++;
        num_digits++;
    }
#endif

    if ((flag & (FMT_SIGN_MASK | FMT_SPACE_FLAG | FMT_PLUS_FLAG)) == 0) {
        sign_char = '\0';
        fill_cnt = field_width - num_digits;
    }
    else {
        if ((flag & FMT_SPACE_FLAG) == 0) {
            // '+' is 0x2B, '-' is 0x2D
            sign_char = '+' + (flag & FMT_SIGN_MASK);
            fill_cnt = field_width - num_digits - 1;
        }
        else {
            // ' ' is 0x20, '-' is 0x2D
#if 0
            sign_char = ((flag & FMT_SIGN_MASK) == 0) ? ' ' : '-';
#else
            if ((flag & FMT_SIGN_MASK) == 0)
                sign_char = ' ';
            else
                sign_char = '-';
#endif
            fill_cnt = field_width - num_digits - 1;
        }
    }

    if (fill_cnt <= 0) {
        // Add sign
        if (sign_char != '\0')
            *buf++ = (char)sign_char;

        // Start to write the digitals
        cur = (char *)buf + num_digits;
        *cur = '\0';

        do {
            digit_val = val % 10;
            val /= 10;

            cur--;
            *cur = (char)(digit_val + '0');
        } while (val != 0);

        if (sign_char == '\0')
            return num_digits;
        else
            return num_digits + 1;
    }
    else {
        // When legnth <= 0 || legnth >= witdh, align to right or left is same
        if (length <= 0 || length >= (int)field_width) {
            if (fill == FMT_FILL_SPACE) {
                // Fill normal
                while (fill_cnt > 0) {
                    *buf++ = fill;
                    fill_cnt--;
                }

                // Add sign
                if (sign_char != '\0')
                    *buf++ = (char)sign_char;
            }
            else {
                // Add sign
                if (sign_char != '\0')
                    *buf++ = (char)sign_char;

                // Fill normal
                while (fill_cnt > 0) {
                    *buf++ = fill;
                    fill_cnt--;
                }
            }
        }
        else {
            if ((flag & FMT_ALIGN_LEFT) == 0) {
                // Align to right, when (length < field_width)
                jimic_assert(length < (int)field_width);

                padding = length - num_digits;
                if (padding > 0) {
                    // Fill right padding space
                    while (fill_cnt > padding) {
                        *buf++ = ' ';
                        fill_cnt--;
                    }

                    // Add sign
                    if (sign_char != '\0')
                        *buf++ = (char)sign_char;

                    // Fill normal
                    while (fill_cnt > 0) {
                        *buf++ = fill;
                        fill_cnt--;
                    }
                }
                else {
                    // Fill right padding space
                    while (fill_cnt > 0) {
                        *buf++ = ' ';
                        fill_cnt--;
                    }

                    // Add sign
                    if (sign_char != '\0')
                        *buf++ = (char)sign_char;
                }
            }
            else {
                // Align to left, when (length < field_width)
                jimic_assert(length < (int)field_width);

                // Add sign
                if (sign_char != '\0')
                    *buf++ = (char)sign_char;

                // Fill normal
                padding = length - num_digits;
                if (padding > 0) {
                    fill_cnt -= padding;
                    while (padding > 0) {
                        *buf++ = fill;
                        padding--;
                    }
                }

                // Start to write the digitals
                cur = (char *)buf + num_digits;

                do {
                    digit_val = val % 10;
                    val /= 10;

                    cur--;
                    *cur = (char)(digit_val + '0');
                } while (val != 0);

                buf += num_digits;

                // Fill left padding space
                while (fill_cnt > 0) {
                    *buf++ = ' ';
                    fill_cnt--;
                }

                goto utoa_r10_ex_exit;
            }
        }
    }

    // Start to write the digitals
    cur = (char *)buf + num_digits;

    do {
        digit_val = val % 10;
        val /= 10;

        cur--;
        *cur = (char)(digit_val + '0');
    } while (val != 0);

    buf += num_digits;

utoa_r10_ex_exit:
    *buf = '\0';

    return field_width;
}

JMC_INLINE_NONSTD(int)
jmc_itoa_r10_fast_ex(char *buf, size_t count, int val, unsigned int flag,
                     unsigned int fill, unsigned int field_width, int length)
{
#if 1
    if (val < 0) {
        flag |= FMT_SIGN_MASK;
        val = -val;
    }

    return jmc_utoa_r10_fast_ex(buf, count, val, flag, fill, field_width, length);
#elif 0
    if ((flag & FMT_SPACE_FLAG) == 0) {
        if (val >= 0) {
            return jmc_utoa_r10_fast_ex(buf, count, val, flag, fill, field_width, length);
        }
        else {
            *buf++ = '-';
            val = -val;
            flag |= FMT_SIGN_MASK;
            return jmc_utoa_r10_fast_ex(buf, count, val, flag, fill, field_width, length) + 1;
        }
    }
    else {
        if (val >= 0) {
            *buf++ = '+';
        }
        else {
            *buf++ = '-';
            val = -val;
        }
        flag |= FMT_SIGN_MASK;
        return jmc_utoa_r10_fast_ex(buf, count, val, flag, fill, field_width, length) + 1;
    }
#elif 0
    if ((flag & FMT_SPACE_FLAG) == 0) {
        if (val < 0) {
            *buf++ = '-';
            val = -val;
            return jmc_utoa_r10_fast_ex(buf, count, val, flag, fill, field_width, length) + 1;
        }
        else
            return jmc_utoa_r10_fast_ex(buf, count, val, flag, fill, field_width, length);
    }
    else {
        if (val >= 0) {
            *buf++ = '+';
        }
        else {
            *buf++ = '-';
            val = -val;
        }
        return jmc_utoa_r10_fast_ex(buf, count, val, flag, fill, field_width, length) + 1;
    }
#else
    int sign;
    if ((flag & FMT_SPACE_FLAG) == 0) {
        sign = (val < 0);
        if (val < 0) {
            *buf++ = '-';
            val = -val;
        }
        return jmc_utoa_r10_fast_ex(buf, count, val, flag, fill, field_width, length) + sign;
    }
    else {
        if (val < 0) {
            *buf++ = '-';
            val = -val;
        }
        else {
            *buf++ = '+';
        }
        return jmc_utoa_r10_fast_ex(buf, count, val, flag, fill, field_width, length) + 1;
    }
#endif
}

JMC_INLINE_NONSTD(int)
jmc_u64toa_r10_fast_ex(char *buf, size_t count, uint64_t val, unsigned int flag,
                       unsigned int fill, unsigned int field_width, int length)
{
    unsigned int digit_val, num_digits;
    uint32_t val32;
    char *end, *cur;
    int sign_char;
    int fill_cnt;
    int padding;
    jmc_uint64_t *u64;
    char digits[32];    // 实际最多只会用到20个bytes

    end = digits + jm_countof(digits) - 1;
    cur = end;
#if 0
    if (val <= (uint64_t)JIMIC_UINT_MAX64) {
        val32 = (uint32_t)val;
#else
    u64 = (jmc_uint64_t *)&val;
    if (u64->dwords.high == 0) {
        val32 = (uint32_t)u64->dwords.low;
#endif
        do {
            digit_val = val32 % 10;
            val32 /= 10;

            *cur-- = (char)(digit_val + '0');
        } while (val32 != 0);
    }
    else {
        do {
            digit_val = val % 10;
            val /= 10;

            *cur-- = (char)(digit_val + '0');
        } while (val != 0);
    }

    num_digits = (unsigned int)(end - cur);

    if ((flag & (FMT_SIGN_MASK | FMT_SPACE_FLAG | FMT_PLUS_FLAG)) == 0) {
        sign_char = '\0';
        fill_cnt = field_width - num_digits;
    }
    else {
        if ((flag & FMT_SPACE_FLAG) == 0) {
            // '+' is 0x2B, '-' is 0x2D
            sign_char = '+' + (flag & FMT_SIGN_MASK);
            fill_cnt = field_width - num_digits - 1;
        }
        else {
            // ' ' is 0x20, '-' is 0x2D
#if 0
            sign_char = ((flag & FMT_SIGN_MASK) == 0) ? ' ' : '-';
#else
            if ((flag & FMT_SIGN_MASK) == 0)
                sign_char = ' ';
            else
                sign_char = '-';
#endif
            fill_cnt = field_width - num_digits - 1;
        }
    }

    if (fill_cnt <= 0) {
        // Add sign
        if (sign_char != '\0')
            *buf++ = (char)sign_char;
#if 0
        do {
            ++cur;
            *buf++ = *cur;
        } while (cur != end);
#else
        cur++;
        while (cur <= end)
            *buf++ = *cur++;
#endif
        *buf = '\0';

        if (sign_char == '\0')
            return num_digits;
        else
            return num_digits + 1;
    }
    else {
        // When legnth <= 0 || legnth >= witdh, align to right or left is same
        if (length <= 0 || length >= (int)field_width) {
            if (fill == FMT_FILL_SPACE) {
                // Fill normal
                while (fill_cnt > 0) {
                    *buf++ = fill;
                    fill_cnt--;
                }

                // Add sign
                if (sign_char != '\0')
                    *buf++ = (char)sign_char;
            }
            else {
                // Add sign
                if (sign_char != '\0')
                    *buf++ = (char)sign_char;

                // Fill normal
                while (fill_cnt > 0) {
                    *buf++ = fill;
                    fill_cnt--;
                }
            }
        }
        else {
            if ((flag & FMT_ALIGN_LEFT) == 0) {
                // Align to right, when (length < field_width)
                jimic_assert(length < (int)field_width);

                padding = length - num_digits;
                if (padding > 0) {
                    // Fill right padding space
                    while (fill_cnt > padding) {
                        *buf++ = ' ';
                        fill_cnt--;
                    }

                    // Add sign
                    if (sign_char != '\0')
                        *buf++ = (char)sign_char;

                    // Fill normal
                    while (fill_cnt > 0) {
                        *buf++ = fill;
                        fill_cnt--;
                    }
                }
                else {
                    // Fill right padding space
                    while (fill_cnt > 0) {
                        *buf++ = ' ';
                        fill_cnt--;
                    }

                    // Add sign
                    if (sign_char != '\0')
                        *buf++ = (char)sign_char;
                }
            }
            else {
                // Align to left, when (length < field_width)
                jimic_assert(length < (int)field_width);

                // Add sign
                if (sign_char != '\0')
                    *buf++ = (char)sign_char;

                // Fill normal
                padding = length - num_digits;
                if (padding > 0) {
                    fill_cnt -= padding;
                    while (padding > 0) {
                        *buf++ = fill;
                        padding--;
                    }
                }

#if 0
                do {
                    ++cur;
                    *buf++ = *cur;
                } while (cur != end);
#else
                cur++;
                while (cur <= end)
                    *buf++ = *cur++;
#endif
                // Fill left padding space
                while (fill_cnt > 0) {
                    *buf++ = ' ';
                    fill_cnt--;
                }

                goto u64toa_r10_ex_exit;
            }
        }
    }

#if 0
    do {
        ++cur;
        *buf++ = *cur;
    } while (cur != end);
#else
    cur++;
    while (cur <= end)
        *buf++ = *cur++;
#endif

u64toa_r10_ex_exit:
    *buf = '\0';

    return field_width;
}

JMC_INLINE_NONSTD(int)
jmc_i64toa_r10_fast_ex(char *buf, size_t count, int64_t val, unsigned int flag,
                       unsigned int fill, unsigned int field_width, int length)
{
    if (val < 0) {
        flag |= FMT_SIGN_MASK;
        val = -val;
    }
    return jmc_u64toa_r10_fast_ex(buf, count, val, flag, fill, field_width, length);
}

#endif  /* _JIMIC_STRING_ITOA_FAST_INL_H_ */
