
#ifndef _JIMI_LANG_THROWABLE_H_
#define _JIMI_LANG_THROWABLE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>
#include <jimi/io/Serializable.h>
#include <jimi/lang/ObjectBase.h>
#include <jimi/lang/String.h>

NS_JIMI_BEGIN

class Object;

class Throwable : public ObjectBase, public Serializable
{
public:
    Throwable();
    Throwable(const Throwable &cause);
    Throwable(const String &message);
    Throwable(const String &message, const Throwable &cause);
    ~Throwable();

private:
    String detailMessage;
    Throwable *cause;
};

Throwable::Throwable()
: detailMessage()
, cause(NULL)
{

}

Throwable::Throwable(const Throwable &cause)
{
    this->cause = (Throwable *)&cause;
}

Throwable::Throwable(const String &message)
{
    detailMessage = message;
}

Throwable::Throwable(const String &message, const Throwable &cause)
{
    detailMessage = message;
    this->cause = (Throwable *)&cause;
}

Throwable::~Throwable()
{
    this->cause = NULL;
}

NS_JIMI_END

#endif  /* _JIMI_LANG_THROWABLE_H_ */
