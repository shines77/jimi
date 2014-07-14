
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
    /* 是否填0或填'空格', 相当于printf()里 "+%05d" 中的 "0" */
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

    void setDetail(unsigned int _align, unsigned int _fill,
                   unsigned int _width, unsigned int _precision) {
        align       = (unsigned short)_align;
        fill        = (unsigned short)_fill;
        width       = (unsigned short)_width;
        precision   = (unsigned short)_precision;
    }
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

    void setDetail(unsigned int _align, unsigned int _fill, unsigned int _width) {
        align       = (unsigned short)_align;
        fill        = (unsigned short)_fill;
        width       = _width;
    }

public:
    unsigned int width;
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

    void setDetail(unsigned int _align, unsigned int _fill, unsigned int _width) {
        align       = (unsigned short)_align;
        fill        = (unsigned short)_fill;
        width       = _width;
    }

public:
    unsigned int width;
};

class formatter_detail : public jimi::NonCopyable
{
public:
    formatter_detail() {}
    ~formatter_detail() {}

public:
    float_setting   doubles;
    integer_setting int32s;
    string_setting  strings;
    integer_setting hex32s;
    float_setting   floats;
    integer_setting int64s;
    integer_setting hex64s;
    integer_setting reserve;
};

#define DEFAULT_FLOAT_PRECISION     15

template <int Precision = DEFAULT_FLOAT_PRECISION, typename StringType = jimi::string>
class formatter : public jimi::NonCopyable
{
public:
    typedef typename StringType::char_type          char_type;
    typedef typename StringType::value_type         value_type;
    typedef typename StringType::size_type          size_type;
    typedef typename std::basic_string<char_type>   StdStringType;

public:
    formatter() { setFloatPrecision(Precision); setDoublePrecision(Precision); }
    ~formatter() {}

public:
    // format() ...
    template <typename ... Args>
    size_t format_to(StringType & result, const value_type * fmt, Args const & ... args);

    template <typename ... Args>
    size_t format_to(StringType & result, const StringType & fmt, Args const & ... args);

    template <typename ... Args>
    StringType format(const value_type * fmt, Args const & ... args);

    template <typename ... Args>
    StringType format(const StringType & fmt, Args const & ... args);

    // append() ...
    template <typename ... Args>
    size_t append_to(StdStringType & result, Args const & ... args);

    template <typename ... Args>
    size_t append_to(StringType & result, Args const & ... args);

    template <typename ... Args>
    size_t append_to(value_type *buffer, size_t countOfElements, Args const & ... args);

    template <typename ... Args>
    StringType append(Args const & ... args);

    template <typename ... Args>
    StringType append_l(std::locale loc, Args const & ... args);

public:
    // setting detail info
    void setFloatPrecision(unsigned int _precision) {
        detail.floats.precision = (unsigned short)_precision;
    }

    void setDoublePrecision(unsigned int _precision) {
        detail.doubles.precision = (unsigned short)_precision;
    }

    void setFloatDetail(unsigned int _align, unsigned int _fill,
                        unsigned int _width, unsigned int _precision) {
        detail.floats.setDetail(_align, _fill, _width, _precision);
    }

    void setDoubleDetail(unsigned int _align, unsigned int _fill,
                         unsigned int _width, unsigned int _precision) {
        detail.doubles.setDetail(_align, _fill, _width, _precision);
    }

private:
    template<typename T, typename ... Args>
    static void format_to_next_args(StringType * arg_list, const T & value);

    template<typename T, typename ... Args>
    static void format_to_next_args(StringType * arg_list, const T & value, Args const & ... args);

    template<typename T, typename ... Args>
    static void append_to_next(StringType & result, const T & value);

    template<typename T, typename ... Args>
    static void append_to_next(StringType & result, const T & value, Args const & ... args);

private:
    formatter_detail    detail;
};

template <int Precision, typename StringType>
template <typename T, typename ... Args>
JIMI_INLINE
void formatter<Precision, StringType>::format_to_next_args(StringType * arg_list, const T & value)
{
    jimi_assert(arg_list != NULL);
    if (arg_list) {
        new (arg_list)StringType();
        arg_list->append(value);
    }
}

template <int Precision, typename StringType>
template <typename T, typename ... Args>
JIMI_INLINE
void formatter<Precision, StringType>::format_to_next_args(StringType * arg_list, const T & value, Args const & ... args)
{
    jimi_assert(arg_list != NULL);
    if (arg_list) {
        new (arg_list)StringType();
        arg_list->append(value);
        arg_list++;
        format_to_next_args(arg_list, args...);
    }
}

/* format_to() 是否使用栈上分配 arg_list 内存? */
#undef  FORMAT_TO_USE_ALLOCA_ONSTACK
#define FORMAT_TO_USE_ALLOCA_ONSTACK       0

template <int Precision, typename StringType>
template <typename ... Args>
JIMI_INLINE
size_t formatter<Precision, StringType>::format_to(StringType & result, const value_type * fmt, Args const & ... args)
{
    size_type index;
    value_type c;
    value_type *cur;
    StringType *arg_list;
    size_type oldSize = result.size();

    jimi_assert(fmt != NULL);
#if 1
    if (fmt == NULL)
        return 0;
#endif

    const size_t max_args = sizeof...(args);
#if defined(FORMAT_TO_USE_ALLOCA_ONSTACK) && (FORMAT_TO_USE_ALLOCA_ONSTACK != 0)
    arg_list = (StringType *)alloca(max_args * sizeof(StringType));
#else
    arg_list = (StringType *)malloc(max_args * sizeof(StringType));
#endif

    format_to_next_args(arg_list, args...);

    index = 0;
    cur = const_cast<value_type *>(fmt);
    while ((c = *cur++) != '\0') {
        if (c == static_cast<value_type>('{')) {
            c = *cur++;
            if (c == static_cast<value_type>('{')) {
                result.append(c);
            }
            else if (c >= static_cast<value_type>('0') && c <= static_cast<value_type>('9')) {
                index = c - static_cast<value_type>('0');
                while ((c = *cur++) != '\0') {
                    if (c == static_cast<value_type>('}')) {
                        // the end of number
                        if (index < max_args)
                            result.append(*(arg_list + index));
                        break;
                    }
                    else if (c >= static_cast<value_type>('0') && c <= static_cast<value_type>('9')) {
                        index = index * 10 + (c - static_cast<value_type>('0'));
                    }
                    else {
                        // get a error number
                        break;
                    }
                }
            }
        }
        else {
            result.append(c);
        }
    }

#if defined(FORMAT_TO_USE_ALLOCA_ONSTACK) && (FORMAT_TO_USE_ALLOCA_ONSTACK != 0)
    StringType *cur_arg = arg_list;
    for (size_t i = 0; i < max_args; ++i) {
        cur_arg->~basic_string();
        cur_arg++;
    }
#else
    if (arg_list) {
        free(arg_list);
    }
#endif
    return (result.size() - oldSize);
}

template <int Precision, typename StringType>
template <typename ... Args>
JIMI_INLINE
size_t formatter<Precision, StringType>::format_to(StringType & result, const StringType & fmt, Args const & ... args)
{
    return format_to(result, fmt.c_str(), args...);
}

template <int Precision, typename StringType>
template <typename ... Args>
JIMI_INLINE
StringType formatter<Precision, StringType>::format(const value_type * fmt, Args const & ... args)
{
    jimi::string result;
    format_to(result, fmt, args...);
    return result;
}

template <int Precision, typename StringType>
template <typename ... Args>
JIMI_INLINE
StringType formatter<Precision, StringType>::format(const StringType & fmt, Args const & ... args)
{
    return format(fmt.c_str(), args...);
}

/************************************************************************/
/*                                                                      */
/*  Compiler Error: n个重载没有“this”指针的合法转换                   */
/*                                                                      */
/*    http://hi.baidu.com/xiaoza1990/item/35e4bf9bc6728c0b924f4195      */
/*                                                                      */
/************************************************************************/

template <int Precision, typename StringType>
template <typename T, typename ... Args>
JIMI_INLINE
void formatter<Precision, StringType>::append_to_next(StringType & result, const T & value)
{
    result.append(value);
}

template <int Precision, typename StringType>
template <typename T, typename ... Args>
JIMI_INLINE
void formatter<Precision, StringType>::append_to_next(StringType & result, const T & value, Args const & ... args)
{
    result.append(value);
    append_to_next(result, args...);
}

template <int Precision, typename StringType>
template <typename ... Args>
JIMI_FORCEINLINE
size_t formatter<Precision, StringType>::append_to(StdStringType & result, Args const & ... args)
{
    StdStringType::size_type oldSize = result.size();
    StringType result_tmp;
    append_to_next(result_tmp, args...);
    result.assign(result_tmp.c_str(), result_tmp.size());
    return result.size() - oldSize;
}

template <int Precision, typename StringType>
template <typename ... Args>
JIMI_FORCEINLINE
size_t formatter<Precision, StringType>::append_to(StringType & result, Args const & ... args)
{
    size_type oldSize = result.size();
    append_to_next(result, args...);
    return result.size() - oldSize;
}

template <int Precision, typename StringType>
template <typename ... Args>
JIMI_FORCEINLINE
size_t formatter<Precision, StringType>::append_to(value_type *buffer, size_t countOfElements, Args const & ... args)
{
    // TODO: 未实现
    return 0;
}

template <int Precision, typename StringType>
template <typename ... Args>
JIMI_FORCEINLINE
StringType formatter<Precision, StringType>::append(Args const & ... args)
{
    StringType result;
    append_to_next(result, args...);
    return result;
}

template <int Precision, typename StringType>
template <typename ... Args>
JIMI_FORCEINLINE
StringType formatter<Precision, StringType>::append_l(std::locale loc, Args const & ... args)
{
    StringType result;
    return result;
}

NS_JIMI_END

#endif  /* !_JIMI_LANG_FORMATTER_H_ */
