
#ifndef _JIMI_BASIC_ARCHDEF_H_
#define _JIMI_BASIC_ARCHDEF_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/*******************************************************************************
 *
 * Pre-defined Compiler Macros
 *
 * Reference:
 *    http://sourceforge.net/p/predef/wiki/Architectures/
 *    http://sourceforge.net/p/predef/wiki/DataModels/
 *
 *******************************************************************************/


/** Arch: alpha **/

/**
 * ------------------------------------------------------------------------------
 * Identification 	__alpha__ 	            Defined by GNU C
 * Version 	        __alpha_ev'V'__ 	    V = Version
 *
 * example:
 *   Alpha EV4      __alpha_ev4__
 *   Alpha EV5      __alpha_ev5__
 *   Alpha EV6      __alpha_ev6__
 *
 * ------------------------------------------------------------------------------
 * Identification 	__alpha 	            Defined by DEC C
 *
 * ------------------------------------------------------------------------------
 * Identification 	_M_ALPHA 	            Defined by Visual Studio
 *
 */

#if defined(__alpha__) || defined(__alpha) || defined(_M_ALPHA)

  #define JIMI_ARCH_ALPHA

  #if defined(__GNUC__) && defined(__alpha__)
    #if defined(__alpha_ev4__)
      #define JIMI_ARCH_ALPHA_EV4     __alpha_ev4__
      #define JIMI_ARCH_ALPHA_VER     __alpha_ev4__
    #elif defined(__alpha_ev5__)
      #define JIMI_ARCH_ALPHA_EV5     __alpha_ev5__
      #define JIMI_ARCH_ALPHA_VER     __alpha_ev5__
    #elif defined(__alpha_ev6__)
      #define JIMI_ARCH_ALPHA_EV6     __alpha_ev5__
      #define JIMI_ARCH_ALPHA_VER     __alpha_ev6__
    #endif
  #endif  /* __GNUC__ && __alpha__ */

#endif  /* Arch: alpha */


/** Arch: Intel x86 **/

/**
 * ------------------------------------------------------------------------------
 * Identification 	i386 	                Defined by GNU C
 *                  __i386
 *                  __i386__
 *
 * Version 	        __i386__ 	            CPU: 80386
 * Version 	        __i486__ 	            CPU: 80486
 * Version 	        __i586__ 	            CPU: Pentium
 * Version 	        __i686__ 	            CPU: Pentium Pro/II
 *
 * ------------------------------------------------------------------------------
 * Identification 	__i386 	                Defined by Sun Studio
 *
 * ------------------------------------------------------------------------------
 * Identification 	__i386 	                Defined by Stratus VOS C
 *                  __IA32__
 *
 * ------------------------------------------------------------------------------
 * Identification 	_M_I86 	                Only defined for 16-bits architectures
 *
 *    Defined by Visual Studio, Digital Mars, and Watcom C/C++ (see note below)
 *
 * ------------------------------------------------------------------------------
 * Identification 	_M_IX86 	            Only defined for 32-bits architectures
 *
 *    Defined by Visual Studio, Intel C/C++, Digital Mars, and Watcom C/C++
 *
 * Version 	        _M_IX86 	            format = V00, V = Version
 *
 * example:
 *
 *   _M_IX86 	    300 	                CPU: 80386
 *                  400 	                CPU: 80486
 *                  500 	                CPU: Pentium
 *                  600                     CPU: Pentium Pro/II
 *
 * ------------------------------------------------------------------------------
 * Identification 	__X86__ 	            Defined by Watcom C/C++
 *
 * ------------------------------------------------------------------------------
 * Identification 	_X86_ 	                Defined by MinGW32
 *
 * ------------------------------------------------------------------------------
 * Identification 	__THW_INTEL__ 	        Defined by XL C/C++
 *
 * ------------------------------------------------------------------------------
 * Identification 	__I86__ 	            Defined by Digital Mars
 *
 * Version 	        __I86__ 	            format = V, V = Version
 *
 * example:
 *
 *   __I86__ 	    3 	                    CPU: 80386
 *                  4 	                    CPU: 80486
 *                  5 	                    CPU: Pentium
 *                  6                       CPU: Pentium Pro/II
 *
 * ------------------------------------------------------------------------------
 * Identification 	__INTEL__ 	            Defined by CodeWarrior
 *
 * ------------------------------------------------------------------------------
 * Identification 	__386 	                Defined by Diab
 *
 * Notice: That Watcom C/C++ defines _M_IX86 for both 16-bits and 32-bits architectures.
 *         Use __386__ or _M_I386 to detect 32-bits architectures in this case.
 *
 * Notice: That the Stratus VOS is big-endian on IA32, so these macros cannot be used to
 *         detect endianness if __VOS__ is set.
 *
 */

#if defined(i386) || defined(__i386) || defined(__i386__) || defined(__IA32__) \
    || defined(_M_I86) || defined(_M_IX86) || defined(__X86__) || defined(_X86_) \
    || defined(__THW_INTEL__) || defined(__I86__) || defined(__INTEL__) || defined(__386) \
    || (defined(_WIN32) && !defined(_WIN64))

  #define JIMI_ARCH_I386
  #define JIMI_ARCH_IX86
  #define JIMI_ARCH_X86

  #if defined(_MSC_VER) || defined(__ICL) || defined(__INTEL_COMPILER)
    #if defined(_M_IX86)
      #undef JIMI_ARCH_I386
      #undef JIMI_ARCH_IX86
      #undef JIMI_ARCH_X86

      #define JIMI_ARCH_I386        _M_IX86
      #define JIMI_ARCH_IX86        _M_IX86
      #define JIMI_ARCH_X86         _M_IX86

      #define JIMI_ARCH_I386_VER    _M_IX86
      #define JIMI_ARCH_IX86_VER    _M_IX86
      #define JIMI_ARCH_X86_VER     _M_IX86
    #endif  /* _M_IX86 */
  #endif  /* _MSC_VER || __ICL || __INTEL_COMPILER */

#endif  /* Arch: Intel x86 */


/** Arch: AMD64 or X64 **/

/**
 * ------------------------------------------------------------------------------
 * Identification 	__amd64__ 	            Defined by GNU C and Sun Studio
 *                  __amd64
 *                  __x86_64__
 *                  __x86_64
 *
 * ------------------------------------------------------------------------------
 * Identification 	_M_X64 	                Defined by Visual Studio
 *                  _M_AMD64
 *
 */

#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) \
    || defined(_M_X64) || defined(_M_AMD64) || defined(_WIN64)

  #define JIMI_ARCH_AMD64
  #define JIMI_ARCH_X64
  #define JIMI_ARCH_X86_64

#endif  /* Arch: AMD64 */


/** Arch: Intel Itanium (IA-64) **/

/**
 * ------------------------------------------------------------------------------
 * Identification 	__ia64__ 	            Defined by GNU C
 *                  _IA64
 *                  __IA64__
 *
 * ------------------------------------------------------------------------------
 * Identification 	__ia64 	                Defined by HP aCC
 *
 * ------------------------------------------------------------------------------
 * Identification 	_M_IA64 	            Defined by Visual Studio
 *
 * ------------------------------------------------------------------------------
 * Identification 	_M_IA64 	            Defined by Intel C/C++
 *
 * Version 	        _M_IA64 	            format = ?
 *
 * example:
 *   _M_IA64        64100
 *
 * ------------------------------------------------------------------------------
 * Identification 	__itanium_ 	            Defined by Intel C/C++
 *
 */

#if defined(__ia64__) || defined(_IA64) || defined(__IA64__) || defined(__ia64) \
    || defined(_M_IA64) || defined(__itanium_)

  #define JIMI_ARCH_IA64

  #if defined(__ICL) || defined(__INTEL_COMPILER)
    #define JIMI_ARCH_IA64_VER  _M_IA64
  #endif  /* Intel C/C++ compiler */

#endif  /* Arch: Intel Itanium (IA-64) */


#endif  /* _JIMI_BASIC_ARCHDEF_H_ */
