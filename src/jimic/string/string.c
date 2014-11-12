
#include <jimic/string/string.h>

#include <stdarg.h>
#include <math.h>       // for isnan(), isinf()
#include <float.h>
#include <limits.h>     // for define UINT_MAX

#include <math.h>

#if !defined(JMC_STRNCPY_EX_INLINE_DECLARE) || (JMC_STRNCPY_EX_INLINE_DECLARE == 0)

/* implement source code for .cpp or .inl.h */
#include <jimic/string/string.impl.inl>

#endif  /* JMC_STRNCPY_EX_INLINE_DECLARE */
