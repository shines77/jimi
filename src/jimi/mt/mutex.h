
#ifndef _JIMI_MTL_MUTEX_H_
#define _JIMI_MTL_MUTEX_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/core/jimi_def.h"
#include "jimi/log/log.h"

#if JIMI_IS_WINDOWS
/* InitializeCriticalSectionAndSpinCount() 必须定义 #define _WIN32_WINNT 0x0403 */
/* 因为只有 winnt 4.03 以后才支持这个函数, 可定义为 0x80000400 (1024) 或 0x80000FA0 (4000) */
#define DEFAULT_CS_SPINCOUNT        4000
#include "jimi/machine/windows_api.h"
#else
#include <pthread.h>
#endif

NS_JIMI_BEGIN

NS_JIMI_MT_BEGIN

class JIMI_DLL mutex
{
public:
    mutex();
    ~mutex();

    // ISO C++0x compatibility methods

    //! Acquire lock
    void lock();

    //! Try acquiring lock (non-blocking)
    /** Return true if lock acquired; false otherwise. */
    bool try_lock(unsigned int spin_count = 0);

    //! Whether have locked?
    bool locked() { return m_bLocked; };

    //! Release lock
    void unlock();

    //! Return native_handle
#if JIMI_IS_WINDOWS
    typedef LPCRITICAL_SECTION  native_handle_type;
#else
    typedef pthread_mutex_t*    native_handle_type;
#endif /* JIMI_IS_WINDOWS */
    native_handle_type native_handle() { return (native_handle_type) &impl; }

    enum state_t {
        INITIALIZED = 0,
        HELD        = 1,
        DESTROYED   = 2
    };

    // Mutex traits
    static const bool is_rw_mutex = false;
    static const bool is_recursive_mutex = false;
    static const bool is_fair_mutex = false;

private:

    //! All checks from mutex constructor using mutex.state were moved here
    void JIMI_EXPORTED_METHOD internal_construct();

    //! All checks from mutex destructor using mutex.state were moved here
    void JIMI_EXPORTED_METHOD internal_destroy();

#if JIMI_IS_WINDOWS
    CRITICAL_SECTION impl;
#else
    pthread_mutex_t impl;
#endif /* JIMI_IS_WINDOWS */

    bool m_bLocked;

#if JIMI_IS_WINDOWS
  #if JIMI_USE_THREADING_TOOLS
private:
    enum state_t state;

public:
    //!  Set the internal state
    void set_state(state_t s) { state = s; }
  #endif /* JIMI_USE_THREADING_TOOLS */
#endif /* JIMI_IS_WINDOWS */
};

NS_JIMI_MT_END

NS_JIMI_END

#endif  /* _JIMI_MTL_MUTEX_H_ */
