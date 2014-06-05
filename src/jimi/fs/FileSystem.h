
#ifndef _JIMI_FS_FILESYSTEM_H_
#define _JIMI_FS_FILESYSTEM_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>
#include <jimi/log/log.h>

#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#include <windows.h>

NS_JIMI_BEGIN

NS_JIMI_FS_BEGIN

class FileSystem
{
public:
    FileSystem();
    ~FileSystem();

    //
};

NS_JIMI_FS_END

NS_JIMI_END

#endif  /* _JIMI_FS_FILESYSTEM_H_ */
