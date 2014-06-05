
#ifndef _JIMI_LANG_OBJECT_H_
#define _JIMI_LANG_OBJECT_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>
//#include <jimi/lang/Throwable.h>
//#include <jimi/lang/Exception.h>
#include <jimi/lang/Cloneable.h>
#include <jimi/io/Closeable.h>
#include <jimi/lang/CloneNotSupportedException.h>
#include <jimi/io/IOException.h>
#include <jimi/lang/String.h>
#include <jimi/log/log.h>

NS_JIMI_BEGIN

class InterruptedException : public Exception
{
    //
};

class IllegalMonitorStateException : public Exception
{
    //
};

class Object;
typedef Cloneable<Object> ObjectCloneable;

class Object : public Cloneable<Object>, public Closeable
{
public:
    Object();
    Object(const Object &src);
    ~Object();

    bool Equals(Object *object);
    bool EqualsRef(const Object &object);

    void Notify() throw (IllegalMonitorStateException) { };
    void NotifyAll() throw (IllegalMonitorStateException) { };

    void Wait() throw (InterruptedException) { };
    void Wait(uint32_t timeout) throw (InterruptedException) { };
    void Wait(uint32_t timeout, int32_t nanos) throw (InterruptedException) { };

    void Close() throw (IOException);
    Object Clone() throw (CloneNotSupportedException);
    String ToString();

protected:
    void Finalize() throw (Exception);    

private:
    String Name;
};

Object::Object()
: Name()
{
}

Object::Object(const Object &src)
{
    Name = src.Name;
}

Object::~Object()
{
    Finalize();
}

void Object::Finalize() throw (Exception)
{
    sLog.info("Object::Finalize(), error = %d.", ::GetLastError());
}

inline bool Object::Equals(Object *object)
{
    return (this == object);
}

inline bool Object::EqualsRef(const Object &object)
{
    return (this == &object);
}


void Object::Close() throw (IOException)
{
    sLog.info("Object::Close(), this = 0x%08X, error = %d.", (void *)this, ::GetLastError());
}

inline Object Object::Clone() throw (CloneNotSupportedException)
{
    return *this;
}

inline String Object::ToString()
{
    return String(Name);
}

NS_JIMI_END

#endif  /* _JIMI_LANG_OBJECT_H_ */
