
#include <jimic/string/jmc_strings.h>
#include <jimic/string/jm_strings.h>

#include <stdarg.h>
#include <math.h>       // for isnan(), isinf()
#include <float.h>
#include <limits.h>     // for UINT_MAX

#if !defined(JMC_DTOS_INLINE_DECLARE) || (JMC_DTOS_INLINE_DECLARE == 0)

/* implement source code for .cpp or .inl.h */
#include <jimic/string/jmc_strings.impl.inl>

#endif  /* !JMC_DTOS_INLINE_DECLARE */
