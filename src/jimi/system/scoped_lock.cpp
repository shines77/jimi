
#include <jimi/system/scoped_lock.h>

NS_JIMI_BEGIN

NS_JIMI_SYSTEM_BEGIN

#if JIMI_USE_ASSERT

template<class T>
void scoped_lock::internal_acquire(T& mutex)
{
    //
}

template<class T>
bool scoped_lock::internal_try_acquire(T& mutex, unsigned int spin_count /* =0 */)
{
    return true;
}

template<class T>
void scoped_lock::internal_release(T& mutex)
{
    //
}

#endif

NS_JIMI_SYSTEM_END

NS_JIMI_END
