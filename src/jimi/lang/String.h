
#ifndef _JIMI_LANG_STRING_H_
#define _JIMI_LANG_STRING_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>
//#include <jimi/lang/Throwable.h>
//#include <jimi/lang/Exception.h>
//#include <jimi/lang/Cloneable.h>
#include <jimi/log/log.h>

NS_JIMI_BEGIN

class Exception;

template <class T, typename char_type = char>
class basic_string
{
public:
    basic_string();
    basic_string(const basic_string &src);
    ~basic_string();

protected:
    char   *data;
    size_t  size;
};

template <class T, typename char_type>
basic_string<T, char_type>::basic_string()
: data(NULL)
, size(0)
{

}

template <class T, typename char_type>
basic_string<T, char_type>::basic_string(const basic_string &src)
{
    this->data = src.data;
    this->size = src.size;
}

template <class T, typename char_type>
basic_string<T, char_type>::~basic_string()
{
}

class Throwable;
class CloneNotSupportedException;

class String : public basic_string<String>
{
public:
    String();
    String(const String &src);
    ~String();

protected:
    void Finalize() throw (Throwable);
    String &Clone() throw (CloneNotSupportedException);
};

String::String() : basic_string<String>()
{
}

String::String(const String &src)
{
    this->data = src.data;
    this->size = src.size;
}

String::~String()
{
    Finalize();
}

void String::Finalize() throw (Throwable)
{
    sLog.info("String::Finalize(), error = %d.", ::GetLastError());
    if (data != NULL)
        delete data;
}

String &String::Clone() throw (CloneNotSupportedException)
{
    return *this;
}

NS_JIMI_END

#endif  /* _JIMI_LANG_STRING_H_ */
