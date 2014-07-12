
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
        width(kDefaultIntegerWidth) {}
    ~integer_setting() {}
public:
    unsigned short width;
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
        width(kDefaultStringWidth) {}
    ~string_setting() {}
public:
    unsigned short width;
};

template <typename StringType>
class formatter : public jimi::NonCopyable
{
public:
    /* 对齐方式, 相当于printf()里 "%+05d" 中的 "+", "-" 前缀 */
    static const int kDefaultAlign = format_setting::ALIGN_NONE;
    /* 是否填0或填'空格', 相当于printf()里 "%03d" 中的 "0" */
    static const int kDefaultFill = format_setting::FILL_NONE;
    /* 显示数据的宽度, 相当于printf()里 "%03d" 中的 "3" */
    static const int kDefaultWidth = 10;
    /* 显示数据的精度, 相当于printf()里 "%0.6f" 中的 "6" */
    static const int kDefaultPrecision = 15;

public:
    formatter() {}
    ~formatter() {}

public:
    template <typename ... Args>
    StringType format(const StringType & fmt, Args const & ... args);

    template <typename ... Args>
    StringType format(const char * fmt, Args const & ... args);

    template <typename ... Args>
    StringType format_fast(Args const & ... args);

    template <typename ... Args>
    StringType format_l(std::locale loc, Args const & ... args);

    template <typename ... Args>
    int format_s(const std::string & result, Args const & ... args);

    template <typename ... Args>
    int format_s(const jimi::string & result, Args const & ... args);

    template <typename ... Args>
    int format_s(const char *buffer, size_t countOfElements, Args const & ... args);

private:
    float_setting   float_set;
    integer_setting integer_set;
    string_setting  string_set;
};

template <typename StringType>
template <typename ... Args>
StringType formatter<StringType>::format(const StringType & fmt, Args const & ... args)
{
    StringType strResult;
    return strResult;
}

template <typename StringType>
template <typename ... Args>
StringType formatter<StringType>::format(const char * fmt, Args const & ... args)
{
    StringType strResult;
    return strResult;
}

template <typename StringType>
template <typename ... Args>
StringType formatter<StringType>::format_fast(Args const & ... args)
{
    StringType strResult;
    return strResult;
}

template <typename StringType>
template <typename ... Args>
StringType formatter<StringType>::format_l(std::locale loc, Args const & ... args)
{
    StringType strResult;
    return strResult;
}

NS_JIMI_END

#endif  /* !_JIMI_LANG_FORMATTER_H_ */
