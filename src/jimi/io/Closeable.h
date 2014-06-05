
#ifndef _JIMI_IO_CLOSEABLE_H_
#define _JIMI_IO_CLOSEABLE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>
#include <jimi/lang/AutoCloseable.h>
#include <jimi/io/IOException.h>

NS_JIMI_BEGIN

class Closeable : public AutoCloseable
{
public:
    void Close() throw (IOException) {
        //throw IOException();
    }
};

NS_JIMI_END

#endif  /* _JIMI_IO_CLOSEABLE_H_ */
