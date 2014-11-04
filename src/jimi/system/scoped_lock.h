
#ifndef _JIMI_MTL_SCOPED_LOCK_H_
#define _JIMI_MTL_SCOPED_LOCK_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>
#include <jimi/internal/NonCopyable.h>
#include <jimi/log/log.h>

NS_JIMI_BEGIN

NS_JIMI_MT_BEGIN

template<class T>
class JIMI_API scoped_lock : public jimi::internal::NonCopyable
{
public:
    //! Construct lock that has not acquired a mutex. 
    scoped_lock() : _pmutex(NULL) {};

    //! Acquire lock on given mutex.
    scoped_lock(T & mutex) /* : _pmutex(NULL) */ {
        acquire_init(mutex);
    }

    //! Release lock (if lock is held).
    ~scoped_lock() {
        release();
    }

    T * get_mutex() { return _pmutex; }
    void set_mutex(T * pmutex) { _pmutex = pmutex; }

    // ISO C++0x compatibility methods

    //! Acquire lock on given mutex(use in constructor only).
    void acquire_init(T & mutex) {
#if JIMI_USE_ASSERT
        internal_acquire(mutex);
#else
        mutex.lock();
        _pmutex = &mutex;
#endif /* JIMI_USE_ASSERT */
    }

    //! Acquire lock on given mutex.
    void acquire(T & mutex) {
#if JIMI_USE_ASSERT
        internal_acquire(mutex);
#else
        T* pmutex_new = &mutex;
        if (_pmutex && (_pmutex != pmutex_new))
            _pmutex->unlock();
        mutex.lock();
        _pmutex = pmutex_new;
#endif /* JIMI_USE_ASSERT */
    }

    //! Try acquire lock on given mutex.
    bool try_acquire(T & mutex, unsigned int spin_count = 0) {
#if JIMI_USE_ASSERT
        return internal_try_acquire(mutex);
#else
        bool result = mutex.try_lock(spin_count);
        if (result)
            _pmutex = &mutex;
        return result;
#endif /* JIMI_USE_ASSERT */
    }

    //! Acquire lock on given mutex(use in constructor only).
    bool has_acquired() {
#if JIMI_USE_ASSERT
        return internal_has_acquired();
#else
        if (_pmutex)
            return _pmutex->locked();
        else
            return false;
#endif /* JIMI_USE_ASSERT */
    }

    //! Release lock
    void release() {
#if JIMI_USE_ASSERT
        if (_pmutex)
            internal_release(*_pmutex);
#else
        if (_pmutex) {
            _pmutex->unlock();
            _pmutex = NULL;
        }
#endif /* JIMI_USE_ASSERT */
    }

public:

#if JIMI_USE_ASSERT
    //! All checks from acquire using mutex.state were moved here
    void JIMI_EXPORTED_METHOD internal_acquire(T & mutex)
    {
        //
    }

    //! All checks from try_acquire using mutex.state were moved here
    bool JIMI_EXPORTED_METHOD internal_try_acquire(T & mutex, unsigned int spin_count = 0)
    {
        return true;
    }

    //! The metux lock has acquired?
    bool JIMI_EXPORTED_METHOD internal_has_acquired()
    {
        if (_pmutex)
            return _pmutex->locked();
        else
            return false;
    }

    //! All checks from release using mutex.state were moved here
    void JIMI_EXPORTED_METHOD internal_release(T & mutex)
    {
        //
    }

#endif  /* JIMI_USE_ASSERT */

    //! The pointer to the current mutex to work
    T * _pmutex;
};

NS_JIMI_MT_END

NS_JIMI_END

#endif  /* _JIMI_MTL_SCOPED_LOCK_H_ */
