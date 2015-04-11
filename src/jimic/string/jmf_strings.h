
#ifndef _JIMIC_STRING_JMF_STRINGS_H_
#define _JIMIC_STRING_JMF_STRINGS_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimic/basic/platform.h"
#include "jimic/config/config.h"

#if JIMI_IS_WINDOWS

#if defined(JIMI_IS_WIN32) && !defined(JIMI_IS_WIN64)
#include "jimic/string/win32/intel/jmf_strlen32.h"
#include "jimic/string/win32/intel/jmf_strlwr32.h"
#else
#include "jimic/string/win32/intel/jmf_strlen64.h"
#include "jimic/string/win32/intel/jmf_strlwr64.h"
#endif

#endif  /* JIMI_IS_WINDOWS */

///////////////////////////////////////////////////////////////////////////

#endif  /* !_JIMIC_STRING_JMF_STRINGS_H_ */
