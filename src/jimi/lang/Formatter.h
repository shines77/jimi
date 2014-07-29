
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

#include <tuple>
#include <type_traits>

NS_JIMI_BEGIN

namespace detail {

    enum fmt_align_e {
        AlignNone   = 0,
        AlignSpace  = ' ',
        AlignLeft   = '-',
        AlignRight  = '+'
    };
    enum fmt_fill_e {
        FillNone    = 0,
        FillSpace   = ' ',
        FillZero    = '0'
    };

}  /* namespace of detail */

template <typename char_type = char>
class format_setting : public jimi::NonAssignable
{
public:
    /* 对齐方式, 相当于printf()里 "%+05d" 中的 "+", "-" 前缀 */
    static const int kDefaultAlign = detail::AlignNone;
    /* 是否填0或填'空格', 相当于printf()里 "+%05d" 中的 "0" */
    static const int kDefaultFill = detail::FillNone;

public:
    format_setting() : align(kDefaultAlign), fill(kDefaultFill) {}
    format_setting(const unsigned short _align, const unsigned short _fill)
        : align(_align), fill(_fill) {}
    ~format_setting() {}

public:
    unsigned short align;
    unsigned short fill;
};

template <typename char_type = char>
class float_setting : public format_setting<char_type>
{
public:
    /* 对齐方式, 相当于printf()里 "%+03.6f" 中的 "+", "-" 前缀 */
    static const int kDefaultFloatAlign = detail::AlignNone;
    /* 是否填0或填'空格', 相当于printf()里 "%03.6f" 中的 "0" */
    static const int kDefaultFloatFill = detail::FillNone;
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
        if (_align == detail::AlignNone || _align == detail::AlignSpace
            || _align == detail::AlignLeft || _align == detail::AlignRight)
            align = (unsigned short)_align;
        else
            align = detail::AlignNone;
        if (_fill == detail::FillNone || _fill == detail::FillSpace
            || _fill == detail::FillZero)
            fill = (unsigned short)_fill;
        else
            fill = detail::FillNone;
        width     = (unsigned short)_width;
        precision = (unsigned short)_precision;
    }
    void setDetail(const char_type *fmt) {
        align       = (unsigned short)0;
        fill        = (unsigned short)0;
        width       = 0;
        precision   = 0;
    }

public:
    unsigned short width;
    unsigned short precision;
};

template <typename char_type = char>
class integer_setting : public format_setting<char_type>
{
public:
    /* 对齐方式, 相当于printf()里 "%+05d" 中的 "+", "-" 前缀 */
    static const int kDefaultIntegerAlign = detail::AlignNone;
    /* 是否填0或填'空格', 相当于printf()里 "%05d" 中的 "0" */
    static const int kDefaultIntegerFill = detail::FillNone;
    /* 显示数据的宽度, 相当于printf()里 "%05d" 中的 "5" */
    static const int kDefaultIntegerWidth = 0;

public:
    integer_setting() : format_setting(kDefaultIntegerAlign, kDefaultIntegerFill),
        width(kDefaultIntegerWidth) {}
    ~integer_setting() {}

    void setDetail(unsigned int _align, unsigned int _fill, unsigned int _width) {
        if (_align == detail::AlignNone || _align == detail::AlignSpace
            || _align == detail::AlignLeft || _align == detail::AlignRight)
            align = (unsigned short)_align;
        else
            align = detail::AlignNone;
        if (_fill == detail::FillNone || _fill == detail::FillSpace
            || _fill == detail::FillZero)
            fill = (unsigned short)_fill;
        else
            fill = detail::FillNone;
        width = _width;
    }
    void setDetail(const char_type *fmt) {
        align       = (unsigned short)0;
        fill        = (unsigned short)0;
        width       = 0;
    }

public:
    unsigned int width;
};

template <typename char_type = char>
class string_setting : public format_setting<char_type>
{
public:
    /* 对齐方式, 相当于printf()里 "%+030s" 中的 "+", "-" 前缀 */
    static const int kDefaultStringAlign = detail::AlignNone;
    /* 是否填0或填'空格', 相当于printf()里 "%+030s" 中的 "0" */
    static const int kDefaultStringFill = detail::FillNone;
    /* 显示数据的宽度, 相当于printf()里 "%+030s" 中的 "30" */
    static const int kDefaultStringWidth = 0;

public:
    string_setting() : format_setting(kDefaultStringAlign, kDefaultStringFill),
        width(kDefaultStringWidth) {}
    ~string_setting() {}

    void setDetail(unsigned int _align, unsigned int _fill, unsigned int _width) {
        if (_align == detail::AlignNone || _align == detail::AlignSpace
            || _align == detail::AlignLeft || _align == detail::AlignRight)
            align = (unsigned short)_align;
        else
            align = detail::AlignNone;
        if (_fill == detail::FillNone || _fill == detail::FillSpace
            || _fill == detail::FillZero)
            fill = (unsigned short)_fill;
        else
            fill = detail::FillNone;
        width = _width;
    }
    void setDetail(const char_type *fmt) {
        align       = (unsigned short)0;
        fill        = (unsigned short)0;
        width       = 0;
    }

public:
    unsigned int width;
};

template <typename char_type = char>
class format_detail : public jimi::NonAssignable
{
public:
    format_detail() {}
    ~format_detail() {}

public:
    // setting detail info

    // float, double and decimal's align
    format_detail & setFloatAlign(unsigned int _align) {
        floats.align = _align;
        return *this;
    }
    format_detail & setDoubleAlign(unsigned int _align) {
        doubles.align = _align;
        return *this;
    }
    format_detail & setDecimalAlign(unsigned int _align) {
        floats.align  = _align;
        doubles.align = _align;
        return *this;
    }

    // float, double and decimal's fill
    format_detail & setFloatFill(unsigned int _fill) {
        floats.fill  = _fill;
        return *this;
    }
    format_detail & setDoubleFill(unsigned int _fill) {
        doubles.fill  = _fill;
        return *this;
    }
    format_detail & setDecimalFill(unsigned int _fill) {
        floats.fill   = _fill;
        doubles.fill  = _fill;
        return *this;
    }

    // float, double and decimal's width
    format_detail & setFloatWidth(unsigned int _width) {
        floats.width = (unsigned short)_width;
        return *this;
    }
    format_detail & setDoubleWidth(unsigned int _width) {
        doubles.width = (unsigned short)_width;
        return *this;
    }
    format_detail & setDecimalWidth(unsigned int _width) {
        floats.width  = (unsigned short)_width;
        doubles.width = (unsigned short)_width;
        return *this;
    }

    // float, double and decimal's precision
    format_detail & setFloatPrecision(unsigned int _precision) {
        floats.precision = (unsigned short)_precision;
        return *this;
    }
    format_detail & setDoublePrecision(unsigned int _precision) {
        doubles.precision = (unsigned short)_precision;
        return *this;
    }
    format_detail & setDecimalPrecision(unsigned int _precision) {
        floats.precision  = (unsigned short)_precision;
        doubles.precision = (unsigned short)_precision;
        return *this;
    }

    // float, double and decimal's detail
    format_detail & setFloat(unsigned int _align, unsigned int _fill,
                         unsigned int _width, unsigned int _precision) {
        floats.setDetail(_align, _fill, _width, _precision);
        return *this;
    }
    format_detail & setDouble(unsigned int _align, unsigned int _fill,
                          unsigned int _width, unsigned int _precision) {
        doubles.setDetail(_align, _fill, _width, _precision);
        return *this;
    }
    format_detail & setDecimal(unsigned int _align, unsigned int _fill,
                           unsigned int _width, unsigned int _precision) {
        floats.setDetail(_align, _fill, _width, _precision);
        doubles.setDetail(_align, _fill, _width, _precision);
        return *this;
    }

    // float, double and decimal's fromat string detail
    format_detail & setFloat(const char_type *fmt) {
        floats.setDetail(fmt);
        return *this;
    }
    format_detail & setDouble(const char_type *fmt) {
        doubles.setDetail(fmt);
        return *this;
    }
    format_detail & setDecimal(const char_type *fmt) {
        floats.setDetail(fmt);
        doubles.setDetail(fmt);
        return *this;
    }

    // int32, int64 and integer's align
    format_detail & setInt32Align(unsigned int _align) {
        int32s.align = _align;
        return *this;
    }
    format_detail & setInt64Align(unsigned int _align) {
        int64s.align = _align;
        return *this;
    }
    format_detail & setIntegerAlign(unsigned int _align) {
        int32s.align = _align;
        int64s.align = _align;
        return *this;
    }

    // int32, int64 and integer's fill
    format_detail & setInt32Fill(unsigned int _fill) {
        int32s.fill  = _fill;
        return *this;
    }
    format_detail & setInt64Fill(unsigned int _fill) {
        int64s.fill  = _fill;
        return *this;
    }
    format_detail & setIntegerFill(unsigned int _fill) {
        int32s.fill  = _fill;
        int64s.fill  = _fill;
        return *this;
    }

    // int32, int64 and integer's width
    format_detail & setInt32Width(unsigned int _width) {
        int32s.width = _width;
        return *this;
    }
    format_detail & setInt64Width(unsigned int _width) {
        int64s.width = _width;
        return *this;
    }
    format_detail & setIntegerWidth(unsigned int _width) {
        int32s.width = _width;
        int64s.width = _width;
        return *this;
    }

    // int32, int64 and integer's detail
    format_detail & setInt32(unsigned int _align, unsigned int _fill,
                         unsigned int _width) {
        int32s.setDetail(_align, _fill, _width);
        return *this;
    }
    format_detail & setInt64(unsigned int _align, unsigned int _fill,
                         unsigned int _width) {
        int64s.setDetail(_align, _fill, _width);
        return *this;
    }
    format_detail & setInteger(unsigned int _align, unsigned int _fill,
                           unsigned int _width) {
        int32s.setDetail(_align, _fill, _width);
        int64s.setDetail(_align, _fill, _width);
        return *this;
    }

    // int32, int64 and integer's fromat string detail
    format_detail & setInt32(const char_type *fmt) {
        int32s.setDetail(fmt);
        return *this;
    }
    format_detail & setInt64(const char_type *fmt) {
        int64s.setDetail(fmt);
        return *this;
    }
    format_detail & setInteger(const char_type *fmt) {
        int32s.setDetail(fmt);
        int64s.setDetail(fmt);
        return *this;
    }

    // string's detail info
    format_detail & setStringAlign(unsigned int _align) {
        strings.align = _align;
        return *this;
    }
    format_detail & setStringFill(unsigned int _fill) {
        strings.fill  = _fill;
        return *this;
    }
    format_detail & setStringWidth(unsigned int _width) {
        strings.width = _width;
        return *this;
    }
    format_detail & setString(unsigned int _align, unsigned int _fill,
                          unsigned int _width) {
        strings.setDetail(_align, _fill, _width);
        return *this;
    }
    format_detail & setString(const char_type *fmt) {
        strings.setDetail(fmt);
        return *this;
    }

    // hex32 and hex64's align
    format_detail & setHex32Align(unsigned int _align) {
        int32s.align = _align;
        return *this;
    }
    format_detail & setHex64Align(unsigned int _align) {
        int64s.align = _align;
        return *this;
    }

    // hex32 and hex64's fill
    format_detail & setHex32Fill(unsigned int _fill) {
        int32s.fill  = _fill;
        return *this;
    }
    format_detail & setHex64Fill(unsigned int _fill) {
        int64s.fill  = _fill;
        return *this;
    }

    // hex32 and hex64's width
    format_detail & setHex32Width(unsigned int _width) {
        int32s.width = _width;
        return *this;
    }
    format_detail & setHex64Width(unsigned int _width) {
        int64s.width = _width;
        return *this;
    }

    // hex32 and hex64's detail
    format_detail & setHex32(unsigned int _align, unsigned int _fill,
                         unsigned int _width) {
        int32s.setDetail(_align, _fill, _width);
        return *this;
    }
    format_detail & setHex64(unsigned int _align, unsigned int _fill,
                         unsigned int _width) {
        int64s.setDetail(_align, _fill, _width);
        return *this;
    }

    // hex32 and hex64's fromat string detail
    format_detail & setHex32(const char_type *fmt) {
        int32s.setDetail(fmt);
        return *this;
    }
    format_detail & setHex64(const char_type *fmt) {
        int64s.setDetail(fmt);
        return *this;
    }

public:
    float_setting<char_type>   doubles;
    integer_setting<char_type> int32s;
    string_setting<char_type>  strings;
    integer_setting<char_type> hex32s;
    float_setting<char_type>   floats;
    integer_setting<char_type> int64s;
    integer_setting<char_type> hex64s;
    integer_setting<char_type> reserve;
};

#ifndef DEFAULT_FLOAT_PRECISION
#define DEFAULT_FLOAT_PRECISION     6
#endif

template <typename StringType = jimi::string, int Precision = DEFAULT_FLOAT_PRECISION>
class formatter : public jimi::NonAssignable
{
public:
//  typedef typename StringType::char_type          char_type;
    typedef typename StringType::value_type         value_type;
    typedef typename StringType::size_type          size_type;
    typedef typename std::basic_string<value_type>  StdStringType;

public:
    formatter() {}
    ~formatter() {}

public:
    // format_to() ...
    template <typename ... Args>
    size_type format_to(StringType & result, const value_type * fmt, Args const ... args);

    template <typename ... Args>
    size_type format_to(StringType & result, const StringType & fmt, Args const ... args);

    template <typename ... Args>
    StringType format(const value_type * fmt, Args const ... args);

    template <typename ... Args>
    StringType format(const StringType & fmt, Args const ... args);

    // format_fast_to() ...
    template <typename ... Args>
    size_type format_fast_to(StringType & result, const value_type * fmt, Args const ... args);

    template <typename ... Args>
    size_type format_fast_to_new(StringType & result, const value_type * fmt, Args const ... args);

    template <typename ... Args>
    size_type format_fast_to(StringType & result, const StringType & fmt, Args const ... args);

    template <typename ... Args>
    StringType format_fast(const value_type * fmt, Args const ... args);

    template <typename ... Args>
    StringType format_fast(const StringType & fmt, Args const ... args);

    // format_fast_to2() ...
    template <typename ... Args>
    size_type format_fast_to2(StringType & result, const value_type * fmt, Args const ... args);

    template <typename ... Args>
    size_type format_fast_to2(StringType & result, const StringType & fmt, Args const ... args);

    template <typename ... Args>
    StringType format_fast2(const value_type * fmt, Args const ... args);

    template <typename ... Args>
    StringType format_fast2(const StringType & fmt, Args const ... args);

    // csharp_format_to() ...
    template <typename ... Args>
    size_type csharp_format_to(StringType & result, const value_type * fmt, Args const ... args);

    template <typename ... Args>
    size_type csharp_format_to_new(StringType & result, const value_type * fmt, Args const ... args);

    template <typename ... Args>
    size_type csharp_format_to(StringType & result, const StringType & fmt, Args const ... args);

    template <typename ... Args>
    StringType csharp_format(const value_type * fmt, Args const ... args);

    template <typename ... Args>
    StringType csharp_format(const StringType & fmt, Args const ... args);

    // csharp_format_old_to() ...
    template <typename ... Args>
    size_type csharp_format_old_to(StringType & result, const value_type * fmt, Args const ... args);

    template <typename ... Args>
    size_type csharp_format_old_to(StringType & result, const StringType & fmt, Args const ... args);

    template <typename ... Args>
    StringType csharp_format_old(const value_type * fmt, Args const ... args);

    template <typename ... Args>
    StringType csharp_format_old(const StringType & fmt, Args const ... args);

    // append() ...
    template <typename ... Args>
    typename StdStringType::size_type append_to(StdStringType & result, Args const ... args);

    template <typename ... Args>
    size_type append_to(StringType & result, Args const ... args);

    template <typename ... Args>
    size_type append_to(value_type *buffer, size_t countOfElements, Args const ... args);

    template <typename ... Args>
    StringType append(Args const & ... args);

    template <typename ... Args>
    StringType append_l(std::locale loc, Args const ... args);

protected:
    // for format_to() ...
    template<typename T, typename ... Args>
    static void format_to_next_arg(StringType * arg_list, const T & value);

    template<typename T, typename ... Args>
    static void format_to_next_arg(StringType * arg_list, const T & value,
                                   Args const ... args);

    // for csharp_format_to() ...
    template<typename T, typename ... Args>
    static void csharp_format_to_next_arg(StringType * arg_list, const T & value);

    template<typename T, typename ... Args>
    static void csharp_format_to_next_arg(StringType * arg_list, const T & value,
                                          Args const ... args);

    // for format_fast_to2() ...
    template<typename T, typename ... Args>
    static void format_fast_to2_next(StringType & result, value_type* & fmt,
        size_t index, size_t max_args, const T & value);

    template<typename T, typename ... Args>
    static void format_fast_to2_next(StringType & result, value_type* & fmt,
        size_t index, size_t max_args, const T & value, Args const ... args);

    // for append_to() ...
    template<typename T, typename ... Args>
    static void append_to_next(StringType & result, const T & value);

    template<typename T, typename ... Args>
    static void append_to_next(StringType & result, const T & value, Args const ... args);
};

////////////////////////////////////////////////////////////////////////////////
// csharp_format_to()
////////////////////////////////////////////////////////////////////////////////

template <typename StringType, int Precision>
template <typename T, typename ... Args>
JIMI_INLINE
void formatter<StringType, Precision>::csharp_format_to_next_arg(
                                        StringType * arg_list, const T & value)
{
    jimi_assert(arg_list != NULL);
    if (arg_list) {
        new (arg_list)StringType();
        arg_list->append(value);
    }
}

template <typename StringType, int Precision>
template <typename T, typename ... Args>
JIMI_INLINE
void formatter<StringType, Precision>::csharp_format_to_next_arg(
                                        StringType * arg_list, const T & value,
                                        Args const ... args)
{
    jimi_assert(arg_list != NULL);
    if (arg_list) {
        new (arg_list)StringType();
        arg_list->append(value);
        arg_list++;
        csharp_format_to_next_arg(arg_list, args...);
    }
}

/* 使用递归获取指定arg */

/* 江南版的递归取模版变参 detail::appendArgByIndex<i>() */
#if 1

template <typename StringType, int Precision>
template <typename ... Args>
JIMI_INLINE
typename StringType::size_type
formatter<StringType, Precision>::csharp_format_to(
                                    StringType & result, const value_type * fmt,
                                    Args const ... args)
{
    value_type *cur;
    value_type  c;
    size_t      index;

    jimi_assert(fmt != NULL);
#if 1
    if (fmt == NULL)
        return 0;
#endif

    result.clear();

    const size_t max_args = sizeof...(args);
    auto tp = std::tuple<Args...>(args...);

    cur = const_cast<value_type *>(fmt);

    while ((c = *cur++) != '\0') {
        if (c == '{') {
            c = *cur++;
            if (c != '{') {
                if (c >= '0' && c <= '9') {
                    index = c - '0';
                    while ((c = *cur++) != '\0') {
                        if (c == '}') {
                            // get the index
                            if (index < max_args)
                                detail::appendArgByIndex<0>(index, tp, result);
                            break;
                        }
                        /*
                        else if (c == ':') {
                            // get the index
                            if (index < max_args) {
                                detail::appendArgByIndex<0>(index, tp, result);
                            }
                            break;
                        }
                        //*/
                        else if (c >= '0'
                                 && c <= '9') {
                            index = index * 10 + (c - '0');
                        }
                        else {
                            // get a error index number
                            break;
                        }
                    }
                }
            }
            else {
                result.append(c);
            }
        }
        else {
            result.append(c);
        }
    }

    return result.size();
}

#else  /* 先递归扫描并保存args, 在从list里append() */

/* csharp_format_to() 是否使用栈上分配 arg_list 内存? */
#undef  FORMAT_TO_USE_ALLOCA_ONSTACK
#define FORMAT_TO_USE_ALLOCA_ONSTACK       0

template <typename StringType, int Precision>
template <typename ... Args>
JIMI_INLINE
typename StringType::size_type
formatter<StringType, Precision>::csharp_format_to(
                                    StringType & result, const value_type * fmt,
                                    Args const ... args)
{
    value_type *cur;
    value_type  c;
    size_t      index;
    StringType *arg_list;

    jimi_assert(fmt != NULL);
#if 1
    if (fmt == NULL)
        return 0;
#endif

    result.clear();

    const size_t max_args = sizeof...(args);
#if defined(FORMAT_TO_USE_ALLOCA_ONSTACK) && (FORMAT_TO_USE_ALLOCA_ONSTACK != 0)
    arg_list = (StringType *)alloca(max_args * sizeof(StringType));
#else
    arg_list = (StringType *)malloc(max_args * sizeof(StringType));
#endif

    csharp_format_to_next_arg(arg_list, args...);

    cur = const_cast<value_type *>(fmt);
    while ((c = *cur++) != '\0') {
        if (c == '{') {
            c = *cur++;
            if (c != '{') {
                if (c >= '0' && c <= '9') {
                    index = c - '0';
                    while ((c = *cur++) != '\0') {
                        if (c == '}') {
                            // get the index
                            if (index < max_args)
                                result.append(*(arg_list + index));
                            break;
                        }
                        else if (c >= '0' && c <= '9') {
                            index = index * 10 + (c - '0');
                        }
                        else {
                            // get a error index number
                            break;
                        }
                    }
                }
            }
            else {
                result.append(c);
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
    return result.size();
}

#endif  /* 使用递归获取指定args */

namespace detail {

    template<typename StringType, typename T>
    void
    JIMI_INLINE appendArgByIndex2(StringType & result, size_t & index, const T & value)
    {
        if (index == 0)
            result.append(value);
    }

    template<typename StringType, typename T, typename ... Args>
    void
    JIMI_INLINE appendArgByIndex2(StringType & result, size_t & index, const T & value,
                                  Args const & ... args)
    {
#if 1
        if (index == 0) {
            result.append(value);
        }
        else {
            index--;
            appendArgByIndex2(result, index, args...);
        }
#else
        if (index != 0) {
            index--;
            appendArgByIndex2(result, index, args...);
        }
        else
            result.append(value);
#endif
    }

    template<size_t i = 0, typename StringType, typename T>
    void
    JIMI_INLINE appendArgByIndex2_tpl(StringType & result, size_t index, const T & value)
    {
        if (i == index)
            result.append(value);
    }

    template<size_t i = 0, typename StringType, typename T, typename ... Args>
    void
    JIMI_INLINE appendArgByIndex2_tpl(StringType & result, size_t index, const T & value,
                                      Args const & ... args)
    {
        if (i == index)
            result.append(value);
        else
            appendArgByIndex2_tpl<i + 1>(result, index, args...);
    }

}  /* namespace of detail */

/* 新版的递归取模版变参 detail::appendArgByIndex2() */

template <typename StringType, int Precision>
template <typename ... Args>
JIMI_INLINE
typename StringType::size_type
formatter<StringType, Precision>::csharp_format_to_new(
                                    StringType & result, const value_type * fmt,
                                    Args const ... args)
{
    value_type *cur;
    value_type  c;
    size_t      index;

    jimi_assert(fmt != NULL);
#if 1
    if (fmt == NULL)
        return 0;
#endif

    result.clear();

    const size_t max_args = sizeof...(args);
    cur = const_cast<value_type *>(fmt);

    while ((c = *cur++) != '\0') {
        if (c == '{') {
            c = *cur++;
            if (c != '{') {
                if (c >= '0' && c <= '9') {
                    index = c - '0';
                    while ((c = *cur++) != '\0') {
                        if (c == '}') {
                            // get the index
                            if (index < max_args) {
                                //detail::appendArgByIndex2(result, index, args...);
                                detail::appendArgByIndex2_tpl<0>(result, index, args...);
                            }
                            break;
                        }
                        /*
                        else if (c == ':') {
                            // get the index
                            if (index < max_args) {
                                detail::appendArgByIndex2_tpl<0>(index, tp, result);
                            }
                            break;
                        }
                        //*/
                        else if (c >= '0' && c <= '9') {
                            index = index * 10 + (c - '0');
                        }
                        else {
                            // get a error index number
                            break;
                        }
                    }
                }
            }
            else {
                result.append(c);
            }
        }
        else {
            result.append(c);
        }
    }

    return result.size();
}

template <typename StringType, int Precision>
template <typename ... Args>
JIMI_INLINE
typename StringType::size_type
formatter<StringType, Precision>::csharp_format_to(StringType & result,
                                                   const StringType & fmt,
                                                   Args const ... args)
{
    return csharp_format_to(result, fmt.c_str(), args...);
}

template <typename StringType, int Precision>
template <typename ... Args>
JIMI_INLINE
StringType
formatter<StringType, Precision>::csharp_format(const value_type * fmt,
                                                Args const ... args)
{
    jimi::string result;
    csharp_format_to(result, fmt, args...);
    return result;
}

template <typename StringType, int Precision>
template <typename ... Args>
JIMI_INLINE
StringType
formatter<StringType, Precision>::csharp_format(const StringType & fmt,
                                                Args const ... args)
{
    return csharp_format(fmt.c_str(), args...);
}

////////////////////////////////////////////////////////////////////////////////
// csharp_format_old_to()
////////////////////////////////////////////////////////////////////////////////

/* csharp_format_old_to() 是否使用栈上分配 arg_list 内存? */
#undef  FORMAT_TO_USE_ALLOCA_ONSTACK
#define FORMAT_TO_USE_ALLOCA_ONSTACK       0

template <typename StringType, int Precision>
template <typename ... Args>
JIMI_INLINE
typename StringType::size_type
formatter<StringType, Precision>::csharp_format_old_to(
                                    StringType & result, const value_type * fmt,
                                    Args const ... args)
{
    value_type *cur;
    value_type  c;
    size_t      index;
    StringType *arg_list;

    jimi_assert(fmt != NULL);
#if 1
    if (fmt == NULL)
        return 0;
#endif

    result.clear();

    const size_t max_args = sizeof...(args);
#if defined(FORMAT_TO_USE_ALLOCA_ONSTACK) && (FORMAT_TO_USE_ALLOCA_ONSTACK != 0)
    arg_list = (StringType *)alloca(max_args * sizeof(StringType));
#else
    arg_list = (StringType *)malloc(max_args * sizeof(StringType));
#endif

    csharp_format_to_next_arg(arg_list, args...);

    index = 0;
    cur = const_cast<value_type *>(fmt);
    while ((c = *cur++) != '\0') {
        if (c == '{') {
            c = *cur++;
            if (c != '{') {
                if (c >= '0' && c <= '9') {
                    index = c - '0';
                    while ((c = *cur++) != '\0') {
                        if (c == '}') {
                            // get the index
                            if (index < max_args)
                                result.append(*(arg_list + index));
                            break;
                        }
                        else if (c >= '0' && c <= '9') {
                            index = index * 10 + (c - '0');
                        }
                        else {
                            // get a error index number
                            break;
                        }
                    }
                }
            }
            else {
                result.append(c);
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
    return result.size();
}

template <typename StringType, int Precision>
template <typename ... Args>
JIMI_INLINE
typename StringType::size_type
formatter<StringType, Precision>::csharp_format_old_to(StringType & result,
                                                       const StringType & fmt,
                                                       Args const ... args)
{
    return format_to(result, fmt.c_str(), args...);
}

template <typename StringType, int Precision>
template <typename ... Args>
JIMI_INLINE
StringType
formatter<StringType, Precision>::csharp_format_old(const value_type * fmt,
                                                    Args const ... args)
{
    jimi::string result;
    csharp_format_old_to(result, fmt, args...);
    return result;
}

template <typename StringType, int Precision>
template <typename ... Args>
JIMI_INLINE
StringType
formatter<StringType, Precision>::csharp_format_old(const StringType & fmt,
                                                    Args const ... args)
{
    return csharp_format_old(fmt.c_str(), args...);
}

////////////////////////////////////////////////////////////////////////////////
// format_fast_to()
////////////////////////////////////////////////////////////////////////////////

namespace detail {

    template<size_t i = 0, typename Tuple, typename StringType>
    typename std::enable_if< (i >= std::tuple_size<Tuple>::value) >::type
    JIMI_INLINE appendArgByIndex(size_t, const Tuple &, StringType &)
    {
        throw std::invalid_argument("arg index out of range");
    }

    template<size_t i = 0, typename Tuple, typename StringType>
    typename std::enable_if< (i < std::tuple_size<Tuple>::value) >::type
    JIMI_INLINE appendArgByIndex(size_t index, const Tuple & tp, StringType & result)
    {
#if 0
        if (i == index)
            result.append(std::get<i>(tp));
        else
            appendArgByIndex<i + 1>(index, tp, result);
#else
        if (i != index)
            appendArgByIndex<i + 1>(index, tp, result);
        else
            result.append(std::get<i>(tp));
#endif
    }

}  /* namespace of detail */

template <typename StringType, int Precision>
template <typename ... Args>
JIMI_INLINE
typename StringType::size_type
formatter<StringType, Precision>::format_fast_to(StringType & result,
                                                 const value_type * fmt,
                                                 Args const ... args)
{
    value_type *cur;
    size_t      index;
    value_type  c;

    jimi_assert(fmt != NULL);
#if 1
    if (fmt == NULL)
        return 0;
#endif

    result.clear();

    const size_t max_args = sizeof...(args);
    auto tp = std::tuple<Args...>(args...);
    //const size_t max_args = std::tuple_size<decltype(tp)>::value;

    index = 0;
    cur = const_cast<value_type *>(fmt);
    
    while ((c = *cur++) != '\0') {
        if (c == '?') {
            c = *cur++;
            if (c != '?') {
                if (index < max_args) {
                    detail::appendArgByIndex<0>(index, tp, result);
                    if (c != '\0')
                        result.append(c);
                    else
                        break;
                    index++;
                }
                else {
                    if (c != '\0')
                        result.append(c);
                    else
                        break;
                    while ((c = *fmt++) != '\0') {
                        result.append(c);
                    }
                    break;
                }
            }
            else {
                result.append(c);
            }
        }
        else {
            result.append(c);
        }
    }

    return result.size();
}

template <typename StringType, int Precision>
template <typename ... Args>
JIMI_INLINE
typename StringType::size_type
formatter<StringType, Precision>::format_fast_to_new(StringType & result,
                                                     const value_type * fmt,
                                                     Args const ... args)
{
    value_type *cur;
    size_t      index;
    value_type  c;

    jimi_assert(fmt != NULL);
#if 1
    if (fmt == NULL)
        return 0;
#endif

    result.clear();

    const size_t max_args = sizeof...(args);

    index = 0;
    cur = const_cast<value_type *>(fmt);
    
    while ((c = *cur++) != '\0') {
        if (c == '?') {
            c = *cur++;
            if (c != '?') {
                if (index < max_args) {
                    //detail::appendArgByIndex2(result, index, args...);
                    detail::appendArgByIndex2_tpl<0>(result, index, args...);
                    if (c != '\0')
                        result.append(c);
                    else
                        break;
                    index++;
                }
                else {
                    if (c != '\0')
                        result.append(c);
                    else
                        break;
                    while ((c = *fmt++) != '\0') {
                        result.append(c);
                    }
                    break;
                }
            }
            else {
                result.append(c);
            }
        }
        else {
            result.append(c);
        }
    }

    return result.size();
}

template <typename StringType, int Precision>
template <typename ... Args>
JIMI_INLINE
typename StringType::size_type
formatter<StringType, Precision>::format_fast_to(StringType & result,
                                                 const StringType & fmt,
                                                 Args const ... args)
{
    return format_fast_to(result, fmt.c_str(), args...);
}

template <typename StringType, int Precision>
template <typename ... Args>
JIMI_INLINE
StringType
formatter<StringType, Precision>::format_fast(const value_type * fmt, Args const ... args)
{
    StringType result;
    format_fast_to(result, fmt, args...);
    return result;
}

template <typename StringType, int Precision>
template <typename ... Args>
JIMI_INLINE
StringType
formatter<StringType, Precision>::format_fast(const StringType & fmt, Args const ... args)
{
    return format_fast(fmt.c_str(), args...);

    #include <chrono>
    using std::chrono::high_resolution_clock;
    high_resolution_clock::now();
}

////////////////////////////////////////////////////////////////////////////////
// format_fast_to2()
////////////////////////////////////////////////////////////////////////////////

template <typename StringType, int Precision>
template <typename T, typename ... Args>
JIMI_INLINE
void formatter<StringType, Precision>::format_fast_to2_next(StringType & result,
                                                            value_type* & fmt,
                                                            size_t index, size_t max_args,
                                                            const T & value)
{
    value_type c;
    while ((c = *fmt++) != '\0') {
        if (c == '?') {
            c = *fmt++;
            if (c != '?') {
                if (index < max_args) {
                    result.append(value);
                    if (c != '\0')
                        result.append(c);
                    else
                        break;
                    while ((c = *fmt++) != '\0') {
                        result.append(c);
                    }
                    break;
                }
                else {
                    if (c != '\0')
                        result.append(c);
                    else
                        break;
                    while ((c = *fmt++) != '\0') {
                        result.append(c);
                    }
                    break;
                }
            }
            else {
                result.append(c);
            }
        }
        else {
            result.append(c);
        }
    }
}

template <typename StringType, int Precision>
template <typename T, typename ... Args>
JIMI_INLINE
void formatter<StringType, Precision>::format_fast_to2_next(StringType & result,
                                                            value_type* & fmt,
                                                            size_t index, size_t max_args,
                                                            const T & value, Args const ... args)
{
    value_type c;
    while ((c = *fmt++) != '\0') {
        if (c == '?') {
            c = *fmt++;
            if (c != '?') {
                if (index < max_args) {
                    result.append(value);
                    if (c != '\0')
                        result.append(c);
                    else
                        break;
                    index++;
                    format_fast_to2_next(result, fmt, index, max_args, args...);
                    while ((c = *fmt++) != '\0') {
                        result.append(c);
                    }
                    break;
                }
                else {
                    if (c != '\0')
                        result.append(c);
                    else
                        break;
                    while ((c = *fmt++) != '\0') {
                        result.append(c);
                    }
                    break;
                }
            }
            else {
                result.append(c);
            }
        }
        else {
            result.append(c);
        }
    }
}

template <typename StringType, int Precision>
template <typename ... Args>
JIMI_INLINE
typename StringType::size_type
formatter<StringType, Precision>::format_fast_to2(StringType & result,
                                                  const value_type * fmt,
                                                  Args const ... args)
{
    value_type *cur;

    jimi_assert(fmt != NULL);
#if 1
    if (fmt == NULL)
        return 0;
#endif

    result.clear();

    const size_t max_args = sizeof...(args);
    cur = const_cast<value_type *>(fmt);
    format_fast_to2_next(result, cur, 0, max_args, args...);

    return result.size();
}

template <typename StringType, int Precision>
template <typename ... Args>
JIMI_INLINE
typename StringType::size_type
formatter<StringType, Precision>::format_fast_to2(StringType & result,
                                                  const StringType & fmt,
                                                  Args const ... args)
{
    return format_fast_to2(result, fmt.c_str(), args...);
}

template <typename StringType, int Precision>
template <typename ... Args>
JIMI_INLINE
StringType
formatter<StringType, Precision>::format_fast2(const value_type * fmt,
                                               Args const ... args)
{
    StringType result;
    format_fast_to2(result, fmt, args...);
    return result;
}

template <typename StringType, int Precision>
template <typename ... Args>
JIMI_INLINE
StringType
formatter<StringType, Precision>::format_fast2(const StringType & fmt,
                                               Args const ... args)
{
    return format_fast2(fmt.c_str(), args...);
}

////////////////////////////////////////////////////////////////////////////////
// append_to()
////////////////////////////////////////////////////////////////////////////////

/************************************************************************/
/*                                                                      */
/*  Compiler Error: n个重载没有“this”指针的合法转换                   */
/*                                                                      */
/*    http://hi.baidu.com/xiaoza1990/item/35e4bf9bc6728c0b924f4195      */
/*                                                                      */
/************************************************************************/

template <typename StringType, int Precision>
template <typename T, typename ... Args>
JIMI_INLINE
void formatter<StringType, Precision>::append_to_next(StringType & result, const T & value)
{
    result.append(value);
}

template <typename StringType, int Precision>
template <typename T, typename ... Args>
JIMI_INLINE
void formatter<StringType, Precision>::append_to_next(StringType & result, const T & value,
                                                      Args const ... args)
{
    result.append(value);
    append_to_next(result, args...);
}

template <typename StringType, int Precision>
template <typename ... Args>
JIMI_FORCEINLINE
typename formatter<StringType, Precision>::StdStringType::size_type
formatter<StringType, Precision>::append_to(StdStringType & result, Args const ... args)
{
    StringType result_tmp;
    append_to_next(result_tmp, args...);
    result.assign(result_tmp.c_str(), result_tmp.size());
    return result.size();
}

template <typename StringType, int Precision>
template <typename ... Args>
JIMI_FORCEINLINE
typename StringType::size_type
formatter<StringType, Precision>::append_to(StringType & result, Args const ... args)
{
    result.clear();
    append_to_next(result, args...);
    return result.size();
}

template <typename StringType, int Precision>
template <typename ... Args>
JIMI_FORCEINLINE
typename StringType::size_type
formatter<StringType, Precision>::append_to(value_type *buffer, size_t countOfElements,
                                            Args const ... args)
{
    // TODO: 未实现
    return 0;
}

template <typename StringType, int Precision>
template <typename ... Args>
JIMI_FORCEINLINE
StringType
formatter<StringType, Precision>::append(Args const & ... args)
{
    StringType result;
    append_to_next(result, args...);
    return result;
}

template <typename StringType, int Precision>
template <typename ... Args>
JIMI_FORCEINLINE
StringType
formatter<StringType, Precision>::append_l(std::locale loc, Args const ... args)
{
    StringType result;
    return result;
}

NS_JIMI_END

#endif  /* !_JIMI_LANG_FORMATTER_H_ */
