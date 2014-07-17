
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
        align       = (unsigned short)_align;
        fill        = (unsigned short)_fill;
        width       = (unsigned short)_width;
        precision   = (unsigned short)_precision;
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
        align       = (unsigned short)_align;
        fill        = (unsigned short)_fill;
        width       = _width;
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
        align       = (unsigned short)_align;
        fill        = (unsigned short)_fill;
        width       = _width;
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
class formatter_detail : public jimi::NonAssignable
{
public:
    formatter_detail() {}
    ~formatter_detail() {}

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

#define DEFAULT_FLOAT_PRECISION     0

template <int Precision = DEFAULT_FLOAT_PRECISION, typename StringType = jimi::string>
class formatter : public jimi::NonAssignable
{
public:
//  typedef typename StringType::char_type          char_type;
    typedef typename StringType::value_type         value_type;
    typedef typename StringType::size_type          size_type;
    typedef typename std::basic_string<value_type>  StdStringType;

public:
    formatter() { setFloatPrecision(Precision); setDoublePrecision(Precision); }
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

public:
    // setting detail info

    // float, double and decimal's align
    formatter & setFloatAlign(unsigned int _align) {
        detail.floats.align = _align;
        return *this;
    }
    formatter & setDoubleAlign(unsigned int _align) {
        detail.doubles.align = _align;
        return *this;
    }
    formatter & setDecimalAlign(unsigned int _align) {
        detail.floats.align  = _align;
        detail.doubles.align = _align;
        return *this;
    }

    // float, double and decimal's fill
    formatter & setFloatFill(unsigned int _fill) {
        detail.floats.fill  = _fill;
        return *this;
    }
    formatter & setDoubleFill(unsigned int _fill) {
        detail.doubles.fill  = _fill;
        return *this;
    }
    formatter & setDecimalFill(unsigned int _fill) {
        detail.floats.fill   = _fill;
        detail.doubles.fill  = _fill;
        return *this;
    }

    // float, double and decimal's width
    formatter & setFloatWidth(unsigned int _width) {
        detail.floats.width = (unsigned short)_width;
        return *this;
    }
    formatter & setDoubleWidth(unsigned int _width) {
        detail.doubles.width = (unsigned short)_width;
        return *this;
    }
    formatter & setDecimalWidth(unsigned int _width) {
        detail.floats.width  = (unsigned short)_width;
        detail.doubles.width = (unsigned short)_width;
        return *this;
    }

    // float, double and decimal's precision
    formatter & setFloatPrecision(unsigned int _precision) {
        detail.floats.precision = (unsigned short)_precision;
        return *this;
    }
    formatter & setDoublePrecision(unsigned int _precision) {
        detail.doubles.precision = (unsigned short)_precision;
        return *this;
    }
    formatter & setDecimalPrecision(unsigned int _precision) {
        detail.floats.precision  = (unsigned short)_precision;
        detail.doubles.precision = (unsigned short)_precision;
        return *this;
    }

    // float, double and decimal's detail
    formatter & setFloat(unsigned int _align, unsigned int _fill,
                         unsigned int _width, unsigned int _precision) {
        detail.floats.setDetail(_align, _fill, _width, _precision);
        return *this;
    }
    formatter & setDouble(unsigned int _align, unsigned int _fill,
                          unsigned int _width, unsigned int _precision) {
        detail.doubles.setDetail(_align, _fill, _width, _precision);
        return *this;
    }
    formatter & setDecimal(unsigned int _align, unsigned int _fill,
                           unsigned int _width, unsigned int _precision) {
        detail.floats.setDetail(_align, _fill, _width, _precision);
        detail.doubles.setDetail(_align, _fill, _width, _precision);
        return *this;
    }

    // float, double and decimal's fromat string detail
    formatter & setFloat(const value_type *fmt) {
        detail.floats.setDetail(fmt);
        return *this;
    }
    formatter & setDouble(const value_type *fmt) {
        detail.doubles.setDetail(fmt);
        return *this;
    }
    formatter & setDecimal(const value_type *fmt) {
        detail.floats.setDetail(fmt);
        detail.doubles.setDetail(fmt);
        return *this;
    }

    // int32, int64 and integer's align
    formatter & setInt32Align(unsigned int _align) {
        detail.int32s.align = _align;
        return *this;
    }
    formatter & setInt64Align(unsigned int _align) {
        detail.int64s.align = _align;
        return *this;
    }
    formatter & setIntegerAlign(unsigned int _align) {
        detail.int32s.align = _align;
        detail.int64s.align = _align;
        return *this;
    }

    // int32, int64 and integer's fill
    formatter & setInt32Fill(unsigned int _fill) {
        detail.int32s.fill  = _fill;
        return *this;
    }
    formatter & setInt64Fill(unsigned int _fill) {
        detail.int64s.fill  = _fill;
        return *this;
    }
    formatter & setIntegerFill(unsigned int _fill) {
        detail.int32s.fill  = _fill;
        detail.int64s.fill  = _fill;
        return *this;
    }

    // int32, int64 and integer's width
    formatter & setInt32Width(unsigned int _width) {
        detail.int32s.width = _width;
        return *this;
    }
    formatter & setInt64Width(unsigned int _width) {
        detail.int64s.width = _width;
        return *this;
    }
    formatter & setIntegerWidth(unsigned int _width) {
        detail.int32s.width = _width;
        detail.int64s.width = _width;
        return *this;
    }

    // int32, int64 and integer's detail
    formatter & setInt32(unsigned int _align, unsigned int _fill,
                         unsigned int _width) {
        detail.int32s.setDetail(_align, _fill, _width);
        return *this;
    }
    formatter & setInt64(unsigned int _align, unsigned int _fill,
                         unsigned int _width) {
        detail.int64s.setDetail(_align, _fill, _width);
        return *this;
    }
    formatter & setInteger(unsigned int _align, unsigned int _fill,
                           unsigned int _width) {
        detail.int32s.setDetail(_align, _fill, _width);
        detail.int64s.setDetail(_align, _fill, _width);
        return *this;
    }

    // int32, int64 and integer's fromat string detail
    formatter & setInt32(const value_type *fmt) {
        detail.int32s.setDetail(fmt);
        return *this;
    }
    formatter & setInt64(const value_type *fmt) {
        detail.int64s.setDetail(fmt);
        return *this;
    }
    formatter & setInteger(const value_type *fmt) {
        detail.int32s.setDetail(fmt);
        detail.int64s.setDetail(fmt);
        return *this;
    }

    // string's detail info
    formatter & setStringAlign(unsigned int _align) {
        detail.strings.align = _align;
        return *this;
    }
    formatter & setStringFill(unsigned int _fill) {
        detail.strings.fill  = _fill;
        return *this;
    }
    formatter & setStringWidth(unsigned int _width) {
        detail.strings.width = _width;
        return *this;
    }
    formatter & setString(unsigned int _align, unsigned int _fill,
                          unsigned int _width) {
        detail.strings.setDetail(_align, _fill, _width);
        return *this;
    }
    formatter & setString(const value_type *fmt) {
        detail.strings.setDetail(fmt);
        return *this;
    }

    // hex32 and hex64's align
    formatter & setHex32Align(unsigned int _align) {
        detail.int32s.align = _align;
        return *this;
    }
    formatter & setHex64Align(unsigned int _align) {
        detail.int64s.align = _align;
        return *this;
    }

    // hex32 and hex64's fill
    formatter & setHex32Fill(unsigned int _fill) {
        detail.int32s.fill  = _fill;
        return *this;
    }
    formatter & setHex64Fill(unsigned int _fill) {
        detail.int64s.fill  = _fill;
        return *this;
    }

    // hex32 and hex64's width
    formatter & setHex32Width(unsigned int _width) {
        detail.int32s.width = _width;
        return *this;
    }
    formatter & setHex64Width(unsigned int _width) {
        detail.int64s.width = _width;
        return *this;
    }

    // hex32 and hex64's detail
    formatter & setHex32(unsigned int _align, unsigned int _fill,
                         unsigned int _width) {
        detail.int32s.setDetail(_align, _fill, _width);
        return *this;
    }
    formatter & setHex64(unsigned int _align, unsigned int _fill,
                         unsigned int _width) {
        detail.int64s.setDetail(_align, _fill, _width);
        return *this;
    }

    // hex32 and hex64's fromat string detail
    formatter & setHex32(const value_type *fmt) {
        detail.int32s.setDetail(fmt);
        return *this;
    }
    formatter & setHex64(const value_type *fmt) {
        detail.int64s.setDetail(fmt);
        return *this;
    }

protected:
    // for format_to() ...
    template<typename T, typename ... Args>
    static void format_to_next_args(StringType * arg_list, const T & value);

    template<typename T, typename ... Args>
    static void format_to_next_args(StringType * arg_list, const T & value,
                                    Args const ... args);

    // for csharp_format_old_to() ...
    template<typename T, typename ... Args>
    static void csharp_format_old_to_next_args(StringType * arg_list, const T & value);

    template<typename T, typename ... Args>
    static void csharp_format_old_to_next_args(StringType * arg_list, const T & value,
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

protected:
    formatter_detail<value_type> detail;
};

////////////////////////////////////////////////////////////////////////////////
// csharp_format_old_to()
////////////////////////////////////////////////////////////////////////////////

template <int Precision, typename StringType>
template <typename T, typename ... Args>
JIMI_INLINE
void formatter<Precision, StringType>::csharp_format_old_to_next_args(
                                        StringType * arg_list, const T & value)
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
void formatter<Precision, StringType>::csharp_format_old_to_next_args(
                                        StringType * arg_list, const T & value,
                                        Args const ... args)
{
    jimi_assert(arg_list != NULL);
    if (arg_list) {
        new (arg_list)StringType();
        arg_list->append(value);
        arg_list++;
        csharp_format_old_to_next_args(arg_list, args...);
    }
}

/* format_to() 是否使用栈上分配 arg_list 内存? */
#undef  FORMAT_TO_USE_ALLOCA_ONSTACK
#define FORMAT_TO_USE_ALLOCA_ONSTACK       0

template <int Precision, typename StringType>
template <typename ... Args>
JIMI_INLINE
typename StringType::size_type
formatter<Precision, StringType>::csharp_format_old_to(
                                    StringType & result, const value_type * fmt,
                                    Args const ... args)
{
    size_t      index;
    value_type  c;
    value_type *cur;
    StringType *arg_list;
    size_type   oldSize = result.size();

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

    csharp_format_old_to_next_args(arg_list, args...);

    index = 0;
    cur = const_cast<value_type *>(fmt);
    while ((c = *cur++) != '\0') {
        if (c == static_cast<value_type>('{')) {
            c = *cur++;
            if (c == static_cast<value_type>('{')) {
                result.append(c);
            }
            else if (c >= static_cast<value_type>('0')
                     && c <= static_cast<value_type>('9')) {
                index = c - static_cast<value_type>('0');
                while ((c = *cur++) != '\0') {
                    if (c == static_cast<value_type>('}')) {
                        // the end of number
                        if (index < max_args)
                            result.append(*(arg_list + index));
                        break;
                    }
                    else if (c >= static_cast<value_type>('0')
                             && c <= static_cast<value_type>('9')) {
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
typename StringType::size_type
formatter<Precision, StringType>::csharp_format_old_to(StringType & result,
                                                       const StringType & fmt,
                                                       Args const ... args)
{
    return format_to(result, fmt.c_str(), args...);
}

template <int Precision, typename StringType>
template <typename ... Args>
JIMI_INLINE
StringType
formatter<Precision, StringType>::csharp_format_old(const value_type * fmt,
                                                    Args const ... args)
{
    jimi::string result;
    csharp_format_old_to(result, fmt, args...);
    return result;
}

template <int Precision, typename StringType>
template <typename ... Args>
JIMI_INLINE
StringType
formatter<Precision, StringType>::csharp_format_old(const StringType & fmt,
                                                    Args const ... args)
{
    return csharp_format_old(fmt.c_str(), args...);
}

////////////////////////////////////////////////////////////////////////////////
// format_fast_to()
////////////////////////////////////////////////////////////////////////////////

namespace detail {

    template<size_t i, typename Tuple, typename StringType>
    typename std::enable_if< (i == std::tuple_size<Tuple>::value) >::type
    JIMI_INLINE AppendArgByIndex(size_t, Tuple &, StringType &)
    {
        throw std::invalid_argument("arg index out of range");
    }

    template<size_t i = 0, typename Tuple, typename StringType>
    typename std::enable_if< (i < std::tuple_size<Tuple>::value) >::type
    JIMI_INLINE AppendArgByIndex(size_t index, Tuple & tp, StringType & result)
    {
        if (i == index)
            result.append(std::get<i>(tp));
        else
            AppendArgByIndex<i + 1>(index, tp, result);
    }

}  /* namespace of detail */

template <int Precision, typename StringType>
template <typename ... Args>
JIMI_INLINE
typename StringType::size_type
formatter<Precision, StringType>::format_fast_to(StringType & result,
                                                 const value_type * fmt,
                                                 Args const ... args)
{
    value_type *cur;
    size_t      index;
    value_type  c;
    size_type   oldSize = result.size();

    jimi_assert(fmt != NULL);
#if 1
    if (fmt == NULL)
        return 0;
#endif

    auto tp = std::tuple<Args...>(args...);
    //const size_t max_args = sizeof...(args);
    const size_t max_args = std::tuple_size<decltype(tp)>::value;

    index = 0;
    cur = const_cast<value_type *>(fmt);
    
    while ((c = *cur++) != '\0') {
        if (c == static_cast<value_type>('?')) {
            c = *cur++;
            if (c != static_cast<value_type>('?')) {
                if (index < max_args) {
                    detail::AppendArgByIndex<0>(index, tp, result);
                    if (c != static_cast<value_type>('\0'))
                        result.append(c);
                    else
                        break;
                    index++;
                }
                else {
                    if (c != static_cast<value_type>('\0'))
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

    return (result.size() - oldSize);
}

template <int Precision, typename StringType>
template <typename ... Args>
JIMI_INLINE
typename StringType::size_type
formatter<Precision, StringType>::format_fast_to(StringType & result,
                                                 const StringType & fmt,
                                                 Args const ... args)
{
    return format_fast_to2(result, fmt.c_str(), args...);
}

template <int Precision, typename StringType>
template <typename ... Args>
JIMI_INLINE
StringType
formatter<Precision, StringType>::format_fast(const value_type * fmt, Args const ... args)
{
    StringType result;
    format_fast_to(result, fmt, args...);
    return result;
}

template <int Precision, typename StringType>
template <typename ... Args>
JIMI_INLINE
StringType
formatter<Precision, StringType>::format_fast(const StringType & fmt, Args const ... args)
{
    return format_fast(fmt.c_str(), args...);

    #include <chrono>
    using std::chrono::high_resolution_clock;
    high_resolution_clock::now();
}

////////////////////////////////////////////////////////////////////////////////
// format_fast_to2()
////////////////////////////////////////////////////////////////////////////////

template <int Precision, typename StringType>
template <typename T, typename ... Args>
JIMI_INLINE
void formatter<Precision, StringType>::format_fast_to2_next(StringType & result,
                                                            value_type* & fmt,
                                                            size_t index, size_t max_args,
                                                            const T & value)
{
    value_type c;
    while ((c = *fmt++) != '\0') {
        if (c == static_cast<value_type>('?')) {
            c = *fmt++;
            if (c != static_cast<value_type>('?')) {
                if (index < max_args) {
                    result.append(value);
                    if (c != static_cast<value_type>('\0'))
                        result.append(c);
                    else
                        break;
                    while ((c = *fmt++) != '\0') {
                        result.append(c);
                    }
                    break;
                }
                else {
                    if (c != static_cast<value_type>('\0'))
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

template <int Precision, typename StringType>
template <typename T, typename ... Args>
JIMI_INLINE
void formatter<Precision, StringType>::format_fast_to2_next(StringType & result,
                                                            value_type* & fmt,
                                                            size_t index, size_t max_args,
                                                            const T & value, Args const ... args)
{
    value_type c;
    while ((c = *fmt++) != '\0') {
        if (c == static_cast<value_type>('?')) {
            c = *fmt++;
            if (c != static_cast<value_type>('?')) {
                if (index < max_args) {
                    result.append(value);
                    if (c != static_cast<value_type>('\0'))
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
                    if (c != static_cast<value_type>('\0'))
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

template <int Precision, typename StringType>
template <typename ... Args>
JIMI_INLINE
typename StringType::size_type
formatter<Precision, StringType>::format_fast_to2(StringType & result,
                                                  const value_type * fmt,
                                                  Args const ... args)
{
    value_type *cur;
    size_type   oldSize = result.size();

    jimi_assert(fmt != NULL);
#if 1
    if (fmt == NULL)
        return 0;
#endif

    const size_t max_args = sizeof...(args);
    cur = const_cast<value_type *>(fmt);
    format_fast_to2_next(result, cur, 0, max_args, args...);

    return (result.size() - oldSize);
}

template <int Precision, typename StringType>
template <typename ... Args>
JIMI_INLINE
typename StringType::size_type
formatter<Precision, StringType>::format_fast_to2(StringType & result,
                                                  const StringType & fmt,
                                                  Args const ... args)
{
    return format_fast_to2(result, fmt.c_str(), args...);
}

template <int Precision, typename StringType>
template <typename ... Args>
JIMI_INLINE
StringType
formatter<Precision, StringType>::format_fast2(const value_type * fmt,
                                               Args const ... args)
{
    StringType result;
    format_fast_to2(result, fmt, args...);
    return result;
}

template <int Precision, typename StringType>
template <typename ... Args>
JIMI_INLINE
StringType
formatter<Precision, StringType>::format_fast2(const StringType & fmt,
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
void formatter<Precision, StringType>::append_to_next(StringType & result, const T & value,
                                                      Args const ... args)
{
    result.append(value);
    append_to_next(result, args...);
}

template <int Precision, typename StringType>
template <typename ... Args>
JIMI_FORCEINLINE
typename formatter<Precision, StringType>::StdStringType::size_type
formatter<Precision, StringType>::append_to(StdStringType & result, Args const ... args)
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
typename StringType::size_type
formatter<Precision, StringType>::append_to(StringType & result, Args const ... args)
{
    size_type oldSize = result.size();
    append_to_next(result, args...);
    return result.size() - oldSize;
}

template <int Precision, typename StringType>
template <typename ... Args>
JIMI_FORCEINLINE
typename StringType::size_type
formatter<Precision, StringType>::append_to(value_type *buffer, size_t countOfElements,
                                            Args const ... args)
{
    // TODO: 未实现
    return 0;
}

template <int Precision, typename StringType>
template <typename ... Args>
JIMI_FORCEINLINE
StringType
formatter<Precision, StringType>::append(Args const & ... args)
{
    StringType result;
    append_to_next(result, args...);
    return result;
}

template <int Precision, typename StringType>
template <typename ... Args>
JIMI_FORCEINLINE
StringType
formatter<Precision, StringType>::append_l(std::locale loc, Args const ... args)
{
    StringType result;
    return result;
}

NS_JIMI_END

#endif  /* !_JIMI_LANG_FORMATTER_H_ */
