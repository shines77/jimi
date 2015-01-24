
#ifndef _JIMI_FS_FILESYSTEM_H_
#define _JIMI_FS_FILESYSTEM_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/basic/jimi_def.h"
#include "jimi/log/log.h"

#if JIMI_IS_WINDOWS

#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#endif

namespace jimi {

class FileSystem
{
public:
    FileSystem();
    ~FileSystem();

    //
};

}  /* namespace jimi */

#endif  /* _JIMI_FS_FILESYSTEM_H_ */
