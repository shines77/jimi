
#ifndef _JIMI_THREAD_ATOMIC_H_
#define _JIMI_THREAD_ATOMIC_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>

NS_JIMI_BEGIN

template <class _ValueT = size_t>
class atomic
{
public:
    typedef _ValueT value_type;

public:
    atomic() : _value(0) {}
    atomic(value_type value) : _value(value) {}
    ~atomic() {}

private:
    value_type _value;
};

template <class _ValueT = size_t>
class non_atomic
{
public:
    typedef _ValueT value_type;

public:
    non_atomic() : _value(0) {}
    non_atomic(value_type value) : _value(value) {}
    ~non_atomic() {}

    value_type load(int action) { return _value; }
    value_type store(value_type value, int action) {
        value_type old_value = _value;
        _value = value;
        return old_value;
    }

    value_type fetch_inc(int action) {
        value_type old_value = _value;
        ++_value;;
        return old_value;
    }

    value_type fetch_dec(int action) {
        value_type old_value = _value;
        --_value;
        return old_value;
    }

    value_type fetch_add(value_type value, int action) {
        value_type old_value = _value;
        _value += value;
        return old_value;
    }

    value_type fetch_sub(value_type value, int action) {
        value_type old_value = _value;
        _value -= value;
        return old_value;
    }

private:
    value_type _value;
};

NS_JIMI_END

#endif  /* _JIMI_THREAD_ATOMIC_H_ */
