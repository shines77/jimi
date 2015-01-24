
#ifndef _JIMI_LANG_STRINGDETAIL_H_
#define _JIMI_LANG_STRINGDETAIL_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/core/jimi_def.h"

namespace jimi {

namespace string_detail {

/*
 * Lightly structured memcpy, simplifies copying PODs and introduces
 * some asserts. Unfortunately using this function may cause
 * measurable overhead (presumably because it adjusts from a begin/end
 * convention to a pointer/size convention, so it does some extra
 * arithmetic even though the caller might have done the inverse
 * adaptation outside).
 */
template <class Pod>
inline
void pod_copy(Pod *dest, const Pod *src, size_t size) {
    jimi_assert(size >= 0);
    jimi_assert(dest >= (src + size) || (dest + size) <= src);
    ::memcpy(dest, src, size * sizeof(Pod));
}

template <class Pod>
inline
void pod_copy2(Pod *dest, const Pod *src, const Pod *end) {
    jimi_assert(end >= src);
    jimi_assert(dest >= end || (dest + (end - src)) <= src);
    ::memcpy(dest, src, (end - src) * sizeof(Pod));
}

/*
 * Lightly structured memmove, simplifies copying PODs and introduces
 * some asserts
 */
template <class Pod>
inline
void pod_move(Pod *dest, const Pod *src, size_t size) {
    jimi_assert(size >= 0);
    jimi_assert(dest >= (src + size) || (dest + size) <= src);
    ::memmove(dest, src, size * sizeof(Pod));
}

template <class Pod, class T>
inline
void pod_fill(Pod *dest, T c, size_t size) {
    jimi_assert(dest && size > 0);
    if (sizeof(T) == 1) {   /* static for char */
        ::memset(dest, c, size);
    }
    else {
        const Pod *align_end = dest + (size & ~(size_t)7u);
        for (; dest != align_end; dest += 8) {
            dest[0] = c;
            dest[1] = c;
            dest[2] = c;
            dest[3] = c;
            dest[4] = c;
            dest[5] = c;
            dest[6] = c;
            dest[7] = c;
        }
        // Leftovers
        const Pod *end = dest + size;
        for (; dest != end; ++dest) {
            *dest = c;
        }
    }
}

}  /* end of the namespace string_detail */

}  /* namespace jimi */

#endif  /* _JIMI_LANG_STRINGDETAIL_H_ */
