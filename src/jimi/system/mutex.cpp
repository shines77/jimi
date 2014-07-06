
#include <jimi/system/mutex.h>
#include <jimi/lang/aligned_space.h>
#include <jimi/system/scoped_lock.h>

NS_JIMI_BEGIN

NS_JIMI_SYSTEM_BEGIN

mutex::mutex() : m_bLocked(false)
{
#if JIMI_USE_THREADING_TOOLS
    internal_construct();
#else
  #if JIMI_IS_WINDOWS
    // The flag(The 3th param) can be 0 or CRITICAL_SECTION_NO_DEBUG_INFO
    ::InitializeCriticalSectionEx(&impl, DEFAULT_CS_SPINCOUNT, CRITICAL_SECTION_NO_DEBUG_INFO);
  #else
    int error_code = pthread_mutex_init(&impl, NULL);
    if (error_code)
        jimi::internal::handle_perror(error_code, "mutex: pthread_mutex_init() failed.");
  #endif /* JIMI_IS_WINDOWS */
#endif /* JIMI_USE_THREADING_TOOLS */
}

mutex::~mutex()
{
#if JIMI_USE_THREADING_TOOLS
    internal_destroy();
#else
  #if JIMI_IS_WINDOWS
    ::DeleteCriticalSection(&impl);
  #else
    pthread_mutex_destroy(&impl);
  #endif /* JIMI_IS_WINDOWS */
#endif /* JIMI_USE_THREADING_TOOLS */
}

void mutex::internal_construct() {
#if JIMI_USE_THREADING_TOOLS
  #if JIMI_IS_WINDOWS
    ::InitializeCriticalSectionEx(&impl, DEFAULT_CS_SPINCOUNT, 0);
    state = INITIALIZED;  
  #else
    int error_code = pthread_mutex_init(&impl, NULL);
    if (error_code)
        jimi::internal::handle_perror(error_code, "mutex: pthread_mutex_init() failed.");
  #endif /* JIMI_IS_WINDOWS */    
    ITT_SYNC_CREATE(&impl, _T("jimi::mutex"), _T(""));
#endif /* JIMI_USE_THREADING_TOOLS */
}

void mutex::internal_destroy() {
#if JIMI_USE_THREADING_TOOLS
  #if JIMI_IS_WINDOWS
    switch (state) {
      case INITIALIZED:
          ::DeleteCriticalSection(&impl);
          break;
      case DESTROYED: 
          JIMI_ASSERT(false, "mutex: already destroyed.");
          break;
      default: 
          JIMI_ASSERT(false, "mutex: illegal state for destruction.");
          break;
    }
    state = DESTROYED;
  #else
    int error_code = pthread_mutex_destroy(&impl); 
    JIMI_ASSERT_EX(!error_code, "mutex: pthread_mutex_destroy() failed.");
  #endif /* JIMI_IS_WINDOWS */
#endif /* JIMI_USE_THREADING_TOOLS */
}

void mutex::lock()
{
#if JIMI_USE_ASSERT
    aligned_space<scoped_lock<mutex>, 1> tmp;
    new(tmp.begin()) scoped_lock<mutex>(*this);
#else
  #if JIMI_IS_WINDOWS
    ::EnterCriticalSection(&impl);
  #else
    pthread_mutex_lock(&impl);
  #endif /* JIMI_IS_WINDOWS */
#endif /* JIMI_USE_ASSERT */

    m_bLocked = true;
}

bool mutex::try_lock(unsigned int spin_count /* =0 */)
{
    bool bLocked;
#if JIMI_USE_ASSERT
    aligned_space<scoped_lock<mutex>, 1> tmp;
    scoped_lock<mutex> & s = *tmp.begin();
    s.set_mutex(NULL);
    bLocked = s.internal_try_acquire(*this);
#else
  #if JIMI_IS_WINDOWS
    bLocked = (::TryEnterCriticalSection(&impl) != 0);
  #else
    bLocked = (pthread_mutex_trylock(&impl) == 0);
  #endif /* JIMI_IS_WINDOWS */
#endif /* JIMI_USE_ASSERT */

    m_bLocked = bLocked;
    return bLocked;
}

void mutex::unlock()
{
#if JIMI_USE_ASSERT
    aligned_space<scoped_lock<mutex>, 1> tmp;
    scoped_lock<mutex> & s = *tmp.begin();
    s.set_mutex(NULL);
    s.internal_release(*this);
#else
  #if JIMI_IS_WINDOWS
    ::LeaveCriticalSection(&impl);
  #else
    pthread_mutex_unlock(&impl);
  #endif /* JIMI_IS_WINDOWS */
#endif /* JIMI_USE_ASSERT */

    m_bLocked = false;
}

NS_JIMI_SYSTEM_END

NS_JIMI_END
