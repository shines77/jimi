
#ifndef _JIMI_LANG_FORMATTER_H_
#define _JIMI_LANG_FORMATTER_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>
#include <jimi/log/log.h>
#include <jimi/internal/NonCopyable.h>
#include <jimi/lang/String.h>

#include <string>
#include <locale>

using namespace std;

NS_JIMI_BEGIN

class format_setting : public jimi::NonCopyable
{
public:
    enum fmt_align_e {
        ALIGN_NONE = 0,
        ALIGN_LEFT,
        ALIGN_RIGHT
    };
    enum fmt_fill_e {
        FILL_NONE = 0,
        FILL_SPACE,
        FILL_ZERO
    };

    /* 对齐方式, 相当于printf()里 "%+05d" 中的 "+", "-" 前缀 */
    static const int kDefaultAlign = ALIGN_NONE;
    /* 是否填0或填'空格', 相当于printf()里 "%05d" 中的 "0" */
    static const int kDefaultFill = FILL_NONE;

public:
    format_setting() : align(kDefaultAlign), fill(kDefaultFill) {}
    format_setting(const unsigned short _align, const unsigned short _fill)
        : align(_align), fill(_fill) {}
    ~format_setting() {}

public:
    unsigned short align;
    unsigned short fill;
};

class float_setting : public format_setting
{
public:
    /* 对齐方式, 相当于printf()里 "%+03.6f" 中的 "+", "-" 前缀 */
    static const int kDefaultFloatAlign = format_setting::ALIGN_NONE;
    /* 是否填0或填'空格', 相当于printf()里 "%03.6f" 中的 "0" */
    static const int kDefaultFloatFill = format_setting::FILL_NONE;
    /* 显示数据的宽度, 相当于printf()里 "%0.6f" 中的 "0" */
    static const int kDefaultFloatWidth = 0;
    /* 显示数据的精度, 相当于printf()里 "%0.6f" 中的 "6" */
    static const int kDefaultFloatPrecision = 15;

public:
    float_setting() : format_setting(kDefaultFloatAlign, kDefaultFloatFill),
        width(kDefaultFloatWidth), precision(kDefaultFloatPrecision) {}
    ~float_setting() {}
public:
    unsigned short width;
    unsigned short precision;
};

class integer_setting : public format_setting
{
public:
    /* 对齐方式, 相当于printf()里 "%+05d" 中的 "+", "-" 前缀 */
    static const int kDefaultIntegerAlign = format_setting::ALIGN_NONE;
    /* 是否填0或填'空格', 相当于printf()里 "%05d" 中的 "0" */
    static const int kDefaultIntegerFill = format_setting::FILL_NONE;
    /* 显示数据的宽度, 相当于printf()里 "%05d" 中的 "5" */
    static const int kDefaultIntegerWidth = 0;

public:
    integer_setting() : format_setting(kDefaultIntegerAlign, kDefaultIntegerFill),
        width(kDefaultIntegerWidth), resever1(0) {}
    ~integer_setting() {}
public:
    unsigned short width;
    unsigned short resever1;
};

class string_setting : public format_setting
{
public:
    /* 对齐方式, 相当于printf()里 "%+030s" 中的 "+", "-" 前缀 */
    static const int kDefaultStringAlign = format_setting::ALIGN_NONE;
    /* 是否填0或填'空格', 相当于printf()里 "%+030s" 中的 "0" */
    static const int kDefaultStringFill = format_setting::FILL_NONE;
    /* 显示数据的宽度, 相当于printf()里 "%+030s" 中的 "30" */
    static const int kDefaultStringWidth = 0;

public:
    string_setting() : format_setting(kDefaultStringAlign, kDefaultStringFill),
        width(kDefaultStringWidth), resever1(0) {}
    ~string_setting() {}
public:
    unsigned short width;
    unsigned short resever1;
};

class formatter : public jimi::NonCopyable
{
public:
    formatter() {}
    ~formatter() {}

public:
    template <typename ... Args>
    jimi::string format(const jimi::string & fmt, Args const & ... args);

    template <typename ... Args>
    jimi::string format(const char * fmt, Args const & ... args);

    template <typename ... Args>
    jimi::string format_fast(Args const & ... args);

    template <typename ... Args>
    jimi::string format_l(std::locale loc, Args const & ... args);

    template <typename ... Args>
    int format_s(std::string & result, Args const & ... args) { return 0; };

    template <typename ... Args>
    int format_s(jimi::string & result, Args const & ... args) { return 0; };

    template <typename ... Args>
    int format_s(char *buffer, size_t countOfElements, Args const & ... args) { return 0; };

private:
    template<typename T, typename ... Args>
    void format_fast_next(jimi::string & result, const T & value);

    template<typename T, typename ... Args>
    void format_fast_next(jimi::string & result, const T & value, Args const & ... args);

private:
    float_setting   float_set;
    integer_setting integer_set;
    string_setting  string_set;
};

template <typename ... Args>
JIMI_INLINE
jimi::string formatter::format(const jimi::string & fmt, Args const & ... args)
{
    jimi::string strResult;
    return strResult;
}

template <typename ... Args>
JIMI_INLINE
jimi::string formatter::format(const char * fmt, Args const & ... args)
{
    jimi::string strResult;
    return strResult;
}

/************************************************************************/
/*                                                                      */
/*  Compiler Error: n个重载没有“this”指针的合法转换                   */
/*                                                                      */
/*    http://hi.baidu.com/xiaoza1990/item/35e4bf9bc6728c0b924f4195      */
/*                                                                      */
/************************************************************************/

template <typename T, typename ... Args>
JIMI_INLINE
void formatter::format_fast_next(jimi::string & result, const T & value)
{
    result.append(value);
}

template <typename T, typename ... Args>
JIMI_INLINE
void formatter::format_fast_next(jimi::string & result, const T & value, Args const & ... args)
{
    result.append(value);
    format_fast_next(result, args...);
}

template <typename ... Args>
JIMI_INLINE
jimi::string formatter::format_fast(Args const & ... args)
{
    jimi::string result;
    format_fast_next(result, args...);
    return result;
}

template <typename ... Args>
JIMI_FORCE_INLINE
jimi::string formatter::format_l(std::locale loc, Args const & ... args)
{
    jimi::string result;
    return result;
}

/*
template <typename ... Args>
JIMI_INLINE
int formatter::format_s(jimi::string & result, Args const & ... args)
{
    format_fast_next(result, args...);
    return result.size();
}
//*/

NS_JIMI_END

#endif  /* !_JIMI_LANG_FORMATTER_H_ */
