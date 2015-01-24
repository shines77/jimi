
#ifndef _JIMI_DECLARE_COMM_H_
#define _JIMI_DECLARE_COMM_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef jm_const
#define jm_const        const
#endif  /* JM_CONST */

#ifndef JM_CHAR_DEFINED
#define JM_CHAR_DEFINED
typedef char            jm_char;
#endif  /* JM_CHAR_DEFINED */

#ifndef JM_UCHAR_DEFINED
#define JM_UCHAR_DEFINED
typedef unsigned char   jm_uchar;
#endif  /* JM_UCHAR_DEFINED */

/**
 * for jimi_winapi
 */
#if defined(JIMI_IS_MSVC)
#ifndef __CLR_OR_STD_CALL
#if defined (_M_CEE) || defined (MRTDLL)
#define __CLR_OR_STD_CALL   __clrcall
#else  /* defined (_M_CEE) || defined (MRTDLL) */
#define __CLR_OR_STD_CALL   __stdcall
#endif  /* defined (_M_CEE) || defined (MRTDLL) */
#endif  /* __CLR_OR_STD_CALL */

#define JIMI_WINAPI         __CLR_OR_STD_CALL
#define JMC_WINAPI          JIMI_WINAPI
#else
#define JIMI_WINAPI
#define JMC_WINAPI
#endif  /* JIMI_IS_MSVC */

/**
 * define about __cdecl, __stdcall and __fastcall
 */
#undef __CDECL
#undef __STDCALL
#undef __FASTCALL

#undef _CDECL_SUFFIX
#undef _STDCALL_SUFFIX
#undef _FASTCALL_SUFFIX

/**
 * define __cdecl, __stdcall and __fastcall for windows
 */
#if defined(_MSC_VER)

#ifndef __CDECL
#define __CDECL             __cdecl
#endif

#ifndef __STDCALL
#define __STDCALL           __stdcall
#endif

#ifndef __FASTCALL
#define __FASTCALL          __fastcall
#endif

#define _CDECL_SUFFIX
#define _STDCALL_SUFFIX
#define _FASTCALL_SUFFIX

/**
 * define __cdecl, __stdcall and __fastcall for linux
 */
#elif defined(__GNUC__)

#define __CDECL
#define __STDCALL
#define __FASTCALL

#ifndef _CDECL_SUFFIX
#define _CDECL_SUFFIX       __attribute__ ((__cdecl__))
#endif

#ifndef _STDCALL_SUFFIX
#define _STDCALL_SUFFIX
#endif

#ifndef _FASTCALL_SUFFIX
#define _FASTCALL_SUFFIX    __attribute__ ((fastcall))
#endif

/**
 * define __cdecl, __stdcall and __fastcall for another os
 */
#else  /* !_MSC_VER && !__GNUC__ */

#define __CDECL
#define __STDCALL
#define __FASTCALL

#define _CDECL_SUFFIX
#define _STDCALL_SUFFIX
#define _FASTCALL_SUFFIX

#endif  /* _MSC_VER */

#endif  /* _JIMI_DECLARE_COMM_H_ */
