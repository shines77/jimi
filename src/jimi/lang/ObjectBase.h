
#ifndef _JIMI_LANG_OBJECTBASE_H_
#define _JIMI_LANG_OBJECTBASE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>
#include <jimi/lang/String.h>
#include <jimi/log/log.h>

NS_JIMI_BEGIN

class ObjectBase
{
public:
    ObjectBase();
    ObjectBase(const ObjectBase &src);
    ~ObjectBase();

    bool Equals(ObjectBase *object);
    bool EqualsRef(const ObjectBase &object);

    void Notify() { };
    void NotifyAll() { };

    void Wait() { };
    void Wait(uint32_t timeout) { };
    void Wait(uint32_t timeout, int32_t nanos) { };

    void Close();
    ObjectBase Clone();
    String ToString();

protected:
    void Finalize();    

private:
    String Name;
};

ObjectBase::ObjectBase()
: Name()
{
}

ObjectBase::ObjectBase(const ObjectBase &src)
{
    Name = src.Name;
}

ObjectBase::~ObjectBase()
{
    Finalize();
}

void ObjectBase::Finalize()
{
    jmLog.info("ObjectBase::Finalize(), error = %d.", ::GetLastError());
}

bool ObjectBase::Equals(ObjectBase *object)
{
    return (this == object);
}

bool ObjectBase::EqualsRef(const ObjectBase &object)
{
    return (this == &object);
}

void ObjectBase::Close()
{
    jmLog.info("ObjectBase::Close(), this = 0x%08X, error = %d.", (void *)this, ::GetLastError());
}

ObjectBase ObjectBase::Clone()
{
    return *this;
}

String ObjectBase::ToString()
{
    return String(Name);
}

NS_JIMI_END

#endif  /* _JIMI_LANG_OBJECTBASE_H_ */
