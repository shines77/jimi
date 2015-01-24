
#include "jimic/stdio/sprintf_lite.h"

#include "jimic/string/itoa.h"
#include "jimic/string/dtos.h"
#include "jimic/string/string.h"

#include "jimic/basic/jimic_assert.h"

#if defined(_MSC_VER) && (_MSC_VER != 0)
#pragma warning(push)
#pragma warning(disable : 4102) // ½ûÖ¹warning C4102: "Î´ÒýÓÃµÄ±êÇ©"
#endif  /* _MSC_VER */

static const char s_twodigit_table[][2] = {
    // 0 - 9
    { '0', '0' },
    { '1', '0' },
    { '2', '0' },
    { '3', '0' },
    { '4', '0' },
    { '5', '0' },
    { '6', '0' },
    { '7', '0' },
    { '8', '0' },
    { '9', '0' },
    // 10 - 19
    { '0', '1' },
    { '1', '1' },
    { '2', '1' },
    { '3', '1' },
    { '4', '1' },
    { '5', '1' },
    { '6', '1' },
    { '7', '1' },
    { '8', '1' },
    { '9', '1' },
    // 20 - 29
    { '0', '2' },
    { '1', '2' },
    { '2', '2' },
    { '3', '2' },
    { '4', '2' },
    { '5', '2' },
    { '6', '2' },
    { '7', '2' },
    { '8', '2' },
    { '9', '2' },
    // 30 - 39
    { '0', '3' },
    { '1', '3' },
    { '2', '3' },
    { '3', '3' },
    { '4', '3' },
    { '5', '3' },
    { '6', '3' },
    { '7', '3' },
    { '8', '3' },
    { '9', '3' },
    // 40 - 49
    { '0', '4' },
    { '1', '4' },
    { '2', '4' },
    { '3', '4' },
    { '4', '4' },
    { '5', '4' },
    { '6', '4' },
    { '7', '4' },
    { '8', '4' },
    { '9', '4' },
    // 50 - 59
    { '0', '5' },
    { '1', '5' },
    { '2', '5' },
    { '3', '5' },
    { '4', '5' },
    { '5', '5' },
    { '6', '5' },
    { '7', '5' },
    { '8', '5' },
    { '9', '5' },
    // 60 - 69
    { '0', '6' },
    { '1', '6' },
    { '2', '6' },
    { '3', '6' },
    { '4', '6' },
    { '5', '6' },
    { '6', '6' },
    { '7', '6' },
    { '8', '6' },
    { '9', '6' },
    // 70 - 79
    { '0', '7' },
    { '1', '7' },
    { '2', '7' },
    { '3', '7' },
    { '4', '7' },
    { '5', '7' },
    { '6', '7' },
    { '7', '7' },
    { '8', '7' },
    { '9', '7' },
    // 80 - 89
    { '0', '8' },
    { '1', '8' },
    { '2', '8' },
    { '3', '8' },
    { '4', '8' },
    { '5', '8' },
    { '6', '8' },
    { '7', '8' },
    { '8', '8' },
    { '9', '8' },
    // 90 - 99
    { '0', '9' },
    { '1', '9' },
    { '2', '9' },
    { '3', '9' },
    { '4', '9' },
    { '5', '9' },
    { '6', '9' },
    { '7', '9' },
    { '8', '9' },
    { '9', '9' },
};

JMC_DECLARE_NONSTD(int)
sprintf_lite(char * JMC_RESTRICT buf, const char * JMC_RESTRICT format, ...)
{
    int     len;
    va_list args;

    va_start(args, format);
    len = vsnprintf_lite(buf, (size_t)-1, (size_t)-1, format, args);
    va_end(args);
    return len;
}

JMC_DECLARE_NONSTD(int)
snprintf_lite(char * JMC_RESTRICT buf, size_t count_max, size_t count,
              const char * JMC_RESTRICT format, ...)
{
    int     len;
    va_list args;

    va_start(args, format);
    len = vsnprintf_lite(buf, count_max, count, format, args);
    va_end(args);
    return len;
}

JMC_DECLARE_NONSTD(char *)
slprintf_lite(char * JMC_RESTRICT buf, size_t count_max, size_t count,
              const char * JMC_RESTRICT format, ...)
{
    char   *last;
    va_list args;

    va_start(args, format);
    last = vslprintf_lite(buf, count_max, count, format, args);
    va_end(args);
    return last;
}

JMC_DECLARE_NONSTD(int)
vsprintf_lite(char * JMC_RESTRICT buf, const char * JMC_RESTRICT format,
              va_list args)
{
    return vsnprintf_lite(buf, (size_t)-1, (size_t)-1, format, args);
}

JMC_DECLARE_NONSTD(int)
vsnprintf_lite(char * JMC_RESTRICT buf, size_t count_max, size_t count,
               const char * JMC_RESTRICT format, va_list args)
{
    int     len;
    char   *last;

    last = vslprintf_lite(buf, count_max, count, format, args);
    len = (int)(last - buf);
    return len;
}

JMC_DECLARE_NONSTD(char *)
vslprintf_lite(char * JMC_RESTRICT buf, size_t count_max, size_t count,
               const char * JMC_RESTRICT format, va_list args)
{
    unsigned int base;
    unsigned int u32;
    unsigned int num_digits, digit_val;
    char c;
    const char *cur = format;
    char *last;
    char digits[16];
    base = 0;
    while ((c = *cur) != '\0') {
        if (c != '%') {
            // is not a "%" sign
            *buf++ = *cur++;
        }
        else {
            // is a "%" sign start
            ++cur;
            c = *cur;
            switch (c) {
            default:
                *buf++ = *cur++;
                break;

            case '%':
                // "%%" output a '%' char only
                *buf++ = c;
                cur += 2;
                break;

            case 'o':
            case 'O':
                base = 8;
                num_digits = 0;
                last = digits + sizeof(digits) / sizeof(char) - 1;
                u32 = va_arg(args, unsigned int);
                do {
                    digit_val = u32 % base;
                    u32 /= base;
                    *last-- = digit_val + '0';
                    num_digits++;
                } while (u32 != 0);
                last++;
                while (num_digits > 0) {
                    *buf++ = *last++;
                    num_digits--;
                }
                cur++;
                break;

            case 'd':
            case 'D':
                base = 10;
                num_digits = 0;
                last = digits + sizeof(digits) / sizeof(char) - 1;
                u32 = va_arg(args, unsigned int);
                if ((int)u32 < 0)
                    u32 = (unsigned int)(-(int)u32);
                do {
                    digit_val = u32 % base;
                    u32 /= base;
                    *last-- = digit_val + '0';
                    num_digits++;
                } while (u32 != 0);
                last++;
                if ((int)u32 < 0)
                    *buf++ = '-';
                while (num_digits > 0) {
                    *buf++ = *last++;
                    num_digits--;
                }
                cur++;
                break;

            case 'x':
            case 'X':
                base = 16;
                num_digits = 0;
                last = digits + sizeof(digits) / sizeof(char) - 1;
                u32 = va_arg(args, unsigned int);
                do {
                    digit_val = u32 % base;
                    u32 /= base;
                    if (digit_val >= 10)
                        digit_val += 'A' - '0' - 10;
                    *last-- = digit_val + '0';
                    num_digits++;
                } while (u32 != 0);
                last++;
                while (num_digits > 0) {
                    *buf++ = *last++;
                    num_digits--;
                }
                cur++;
                break;

            case 't':
                num_digits = 0;
                last = digits + sizeof(digits) / sizeof(char) - 1;
                u32 = va_arg(args, unsigned int);
                do {
                    digit_val = u32 % 10UL;
                    u32 /= 10UL;
                    *last-- = digit_val + '0';
                    num_digits++;
                } while (u32 != 0);
                last++;
                while (num_digits > 0) {
                    *buf++ = *last++;
                    num_digits--;
                }
                cur++;
                break;

            case 'T':
                num_digits = 0;
                last = digits + sizeof(digits) / sizeof(char) - 1;
                u32 = va_arg(args, unsigned int);
                do {
                    digit_val = u32 % 100UL;
                    u32 /= 100UL;
                    if (digit_val > 10) {
                        *last-- = s_twodigit_table[digit_val][0];
                        *last-- = s_twodigit_table[digit_val][1];
                        num_digits += 2;
                    }
                    else {
                        *last-- = s_twodigit_table[digit_val][0];
                        num_digits++;
                    }
                } while (u32 != 0);
                last++;
                while (num_digits > 0) {
                    *buf++ = *last++;
                    num_digits--;
                }
                cur++;
                break;

            case 'K':
                num_digits = 0;
                last = digits + sizeof(digits) / sizeof(char) - 1;
                u32 = va_arg(args, unsigned int);
                do {
                    digit_val = u32 % 1000UL;
                    u32 /= 1000UL;
                    if (digit_val >= 100) {
                        unsigned int knum = digit_val / 100;
                        digit_val = digit_val % 100;
                        *last-- = s_twodigit_table[digit_val][0];
                        *last-- = s_twodigit_table[digit_val][1];
                        *last-- = s_twodigit_table[knum][0];
                        num_digits += 3;
                    }
                    else if (digit_val >= 10) {
                        *last-- = s_twodigit_table[digit_val][0];
                        *last-- = s_twodigit_table[digit_val][1];
                        num_digits += 2;
                    }
                    else {
                        *last-- = s_twodigit_table[digit_val][0];
                        num_digits++;
                    }
                } while (u32 != 0);
                last++;
                while (num_digits > 0) {
                    *buf++ = *last++;
                    num_digits--;
                }
                cur++;
                break;

            case 'M':
                num_digits = 0;
                last = digits + sizeof(digits) / sizeof(char) - 1;
                u32 = va_arg(args, unsigned int);
#if 1
                do {
                    digit_val = u32 % 10000UL;
                    u32 /= 10000UL;
                    if (digit_val >= 100) {
                        unsigned int knum = digit_val / 100;
                        digit_val = digit_val % 100;
                        *last-- = s_twodigit_table[digit_val][0];
                        *last-- = s_twodigit_table[digit_val][1];
                        *last-- = s_twodigit_table[knum][0];
                        if (digit_val >= 1000) {
                            *last-- = s_twodigit_table[knum][1];
                            num_digits++;
                        }
                        num_digits += 3;
                    }
                    else {
                        *last-- = s_twodigit_table[digit_val][0];
                        if (digit_val >= 10) {
                            *last-- = s_twodigit_table[digit_val][1];
                            num_digits++;
                        }
                        num_digits++;
                    }
                } while (u32 != 0);
#else
                do {
                    digit_val = u32 % 10000UL;
                    u32 /= 10000UL;
                    if (digit_val > 1000) {
                        unsigned int mknum = digit_val / 100;
                        digit_val = digit_val % 100;
                        *last-- = s_twodigit_table[digit_val][0];
                        *last-- = s_twodigit_table[digit_val][1];
                        *last-- = s_twodigit_table[mknum][0];
                        *last-- = s_twodigit_table[mknum][1];
                        num_digits += 4;
                    }
                    else if (digit_val > 100) {
                        unsigned int knum = digit_val / 100;
                        digit_val = digit_val % 100;
                        *last-- = s_twodigit_table[digit_val][0];
                        *last-- = s_twodigit_table[digit_val][1];
                        *last-- = s_twodigit_table[knum][0];
                        num_digits += 3;
                    }
                    else if (digit_val > 10) {
                        *last-- = s_twodigit_table[digit_val][0];
                        *last-- = s_twodigit_table[digit_val][1];
                        num_digits += 2;
                    }
                    else {
                        *last-- = s_twodigit_table[digit_val][0];
                        num_digits++;
                    }
                } while (u32 != 0);
#endif
                last++;
                while (num_digits > 0) {
                    *buf++ = *last++;
                    num_digits--;
                }
                cur++;
                break;
            }
        }
    }
    *buf = '\0';
    return buf;
}

#if defined(_MSC_VER) && (_MSC_VER != 0)
#pragma warning(pop)    // »Ö¸´warning×´Ì¬
#endif
