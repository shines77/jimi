
#ifndef _JIMI_LANG_OBJECTBASE_H_
#define _JIMI_LANG_OBJECTBASE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/basic/jimi_def.h"
#include "jimi/lang/String.h"
#include "jimi/log/log.h"

namespace jimi {

class ObjectBase
{
public:
    ObjectBase();
    ObjectBase(const ObjectBase &src);
    ~ObjectBase();

    bool Equals(ObjectBase *object);
    bool EqualsRef(const ObjectBase &object);

    void Notify()       { /* TODO: */ };
    void NotifyAll()    { /* TODO: */ };

    void Wait() { };
    void Wait(uint32_t timeout) { /* TODO: */ };
    void Wait(uint32_t timeout, int32_t nanos) { /* TODO: */ };

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

inline void ObjectBase::Finalize()
{
    jmLog.info("ObjectBase::Finalize(), error = %d.", ::GetLastError());
}

inline bool ObjectBase::Equals(ObjectBase *object)
{
    return (this == object);
}

inline bool ObjectBase::EqualsRef(const ObjectBase &object)
{
    return (this == &object);
}

inline void ObjectBase::Close()
{
    jmLog.info("ObjectBase::Close(), this = 0x%08X, error = %d.", (void *)this, ::GetLastError());
}

inline ObjectBase ObjectBase::Clone()
{
    return *this;
}

inline String ObjectBase::ToString()
{
    return Name;
}

}  /* namespace jimi */

#endif  /* _JIMI_LANG_OBJECTBASE_H_ */
