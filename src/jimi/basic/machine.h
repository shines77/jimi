
#ifndef _JIMI_BASIC_MACHINE_H_
#define _JIMI_BASIC_MACHINE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/basic/stddef.h"

namespace jimi {

namespace internal {

// Macro JIMI_TypeWithAlignmentAtLeastAsStrict(T) should be a type with alignment at least as
// strict as type T.  The type should have a trivial default constructor and destructor, so that
// arrays of that type can be declared without initializers.
// It is correct (but perhaps a waste of space) if JIMI_TypeWithAlignmentAtLeastAsStrict(T) expands
// to a type bigger than T.
// The default definition here works on machines where integers are naturally aligned and the
// strictest alignment is 64.
#ifndef JIMI_TypeWithAlignmentAtLeastAsStrict

#if JIMI_ATTRIBUTE_ALIGNED_PRESENT

#define JIMI_DefineTypeWithAlignment(PowerOf2)        \
struct JIMI_machine_type_with_alignment_##PowerOf2 {  \
    uint32_t member[PowerOf2 / sizeof(uint32_t)];     \
} __attribute__((aligned(PowerOf2)));

#define jimi_alignof(T)     __alignof__(T)

#elif JIMI_DECLSPEC_ALIGN_PRESENT

#define JIMI_DefineTypeWithAlignment(PowerOf2)        \
__declspec(align(PowerOf2))                           \
struct JIMI_machine_type_with_alignment_##PowerOf2 {  \
    uint32_t member[PowerOf2 / sizeof(uint32_t)];     \
};

#define jimi_alignof(T)     __alignof(T)

#else /* A compiler with unknown syntax for data alignment */
  #ifndef _MSC_VER
    #error Must define JIMI_TypeWithAlignmentAtLeastAsStrict(T)
  #endif
#endif

/* Now declare types aligned to useful powers of two */
// TODO: Is JIMI_DefineTypeWithAlignment(8) needed on 32 bit platforms?
JIMI_DefineTypeWithAlignment(16)
JIMI_DefineTypeWithAlignment(32)
JIMI_DefineTypeWithAlignment(64)

typedef JIMI_machine_type_with_alignment_64 JIMI_machine_type_with_strictest_alignment;

// Primary template is a declaration of incomplete type so that it fails with unknown alignments
template<size_t N> struct type_with_alignment;

// Specializations for allowed alignments
template<> struct type_with_alignment<1>  { char member;     };
template<> struct type_with_alignment<2>  { uint16_t member; };
template<> struct type_with_alignment<4>  { uint32_t member; };
template<> struct type_with_alignment<8>  { uint64_t member; };
template<> struct type_with_alignment<16> { JIMI_machine_type_with_alignment_16 member; };
template<> struct type_with_alignment<32> { JIMI_machine_type_with_alignment_32 member; };
template<> struct type_with_alignment<64> { JIMI_machine_type_with_alignment_64 member; };

#if JIMI_ALIGNOF_NOT_INSTANTIATED_TYPES_BROKEN

//! Work around for bug in GNU 3.2 and MSVC compilers.
/** Bug is that compiler sometimes returns 0 for __alignof(T) when T has not yet been instantiated.
    The work-around forces instantiation by forcing computation of sizeof(T) before __alignof(T). */
template<size_t Size, typename T>
struct work_around_alignment_bug {
    static const size_t alignment = jimi_alignof(T);
};
#define JIMI_TypeWithAlignmentAtLeastAsStrict(T)    internal::type_with_alignment<internal::work_around_alignment_bug<sizeof(T), T>::alignment>

#else

#define JIMI_TypeWithAlignmentAtLeastAsStrict(T)    internal::type_with_alignment<jimi_alignof(T)>

#endif  /* JIMI_ALIGNOF_NOT_INSTANTIATED_TYPES_BROKEN */

#endif  /* JIMI_TypeWithAlignmentAtLeastAsStrict */

}  /* namespace internal */

}  /* namespace jimi */

#endif  /* !_JIMI_BASIC_MACHINE_H_ */
