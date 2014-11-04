
#ifndef _JIMI_THREAD_WAITHANDLE_H_
#define _JIMI_THREAD_WAITHANDLE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>
#include <jimi/thread/ThreadDef.h>
#include <jimi/log/log.h>

#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#include <windows.h>

NS_JIMI_BEGIN

//
// Reference: http://blog.csdn.net/amdk_7/article/details/1334044
//
// 线程同步之Win32API的Wait函数
//
// RegisterWaitForSingleObject(), UnregisterWait(), UnregisterWaitEx()
//

template <class T>
class WaitHandle
{
public:
    typedef jm_handle_t handle_t;

public:
    WaitHandle();
    ~WaitHandle();

    void Close();

    handle_t GetHandle() { return m_hHandle; }
    void SetHandle(handle_t handle) { m_hHandle = handle; }

    bool IsValid() { return HANDLE_IS_VALID(m_hHandle); }

    bool WaitOne(uint32_t uMillisecTimeout = Timeout::kInfinite, bool exitContext = false);
    uint32_t WaitOneEx(uint32_t uMillisecTimeout = Timeout::kInfinite, bool exitContext = false);

    static bool WaitAny(const handle_t *pHandles, int count,
        uint32_t uMillisecTimeout = Timeout::kInfinite, bool exitContext = false);
    static uint32_t WaitAnyEx(const handle_t *pHandles, int count,
        uint32_t uMillisecTimeout = Timeout::kInfinite, bool exitContext = false);

    static bool WaitAll(const handle_t *pHandles, int count,
        uint32_t uMillisecTimeout = Timeout::kInfinite, bool exitContext = false);
    static uint32_t WaitAllEx(const handle_t *pHandles, int count,
        uint32_t uMillisecTimeout = Timeout::kInfinite, bool exitContext = false);

    static bool SignalAndWait(const handle_t toSignal, const handle_t toWaitOn,
        uint32_t uMillisecTimeout = Timeout::kInfinite, bool exitContext = false);
    static uint32_t SignalAndWaitEx(const handle_t toSignal, const handle_t toWaitOn,
        uint32_t uMillisecTimeout = Timeout::kInfinite, bool exitContext = false);

    static bool IsSignalled(uint32_t uWaitState) { return (uWaitState == WaitFor::kSignalled);  }
    static bool IsTimeout(uint32_t uWaitState)   { return (uWaitState == WaitFor::kTimeout);    }
    static bool IsAbandoned(uint32_t uWaitState) { return (uWaitState == WaitFor::kAbandoned);  }
    static bool IsFailed(uint32_t uWaitState)    { return (uWaitState == WaitFor::kFailed);     }

    static bool IsAnySignalled(uint32_t uWaitState, int count);
    static bool IsAnyAbandoned(uint32_t uWaitState, int count);

protected:
    handle_t m_hHandle;
};

template <class T>
WaitHandle<T>::WaitHandle()
: m_hHandle(NULL)
{
}

template <class T>
WaitHandle<T>::~WaitHandle()
{
    jmLog.info("-------------------------------------");
    jmLog.info("WaitHandle<T>::~WaitHandle() Enter.");
    //T *pThis = static_cast<T *>(this);
    //pThis->Close();
    this->Close();
    jmLog.info("WaitHandle<T>::~WaitHandle() Over.");
    jmLog.info("-------------------------------------");
}

template <class T>
inline
void WaitHandle<T>::Close()
{
    jmLog.info("WaitHandle<T>::Close() Enter.");
    if (IsValid()) {
        ::CloseHandle(m_hHandle);
        m_hHandle = NULL;
    }
    jmLog.info("WaitHandle<T>::Close() Over.");
}

template <class T>
/* static */
inline
bool WaitHandle<T>::WaitOne(uint32_t uMillisecTimeout /* = Timeout::kInfinite */,
                            bool exitContext /* = false */)
{
    return (::WaitForSingleObjectEx(m_hHandle, uMillisecTimeout, exitContext)
                                        == WaitFor::kSignalled);
}

template <class T>
/* static */
inline
uint32_t WaitHandle<T>::WaitOneEx(uint32_t uMillisecTimeout /* = Timeout::kInfinite */,
                                  bool exitContext /* = false */)
{
    return ::WaitForSingleObjectEx(m_hHandle, uMillisecTimeout, exitContext);
}

/* static */
template <class T>
inline
bool WaitHandle<T>::IsAnySignalled(uint32_t uWaitState, int count)
{
    int32_t nWaitObjectIndex = uWaitState - WaitFor::kWaitObject_0;
    return (nWaitObjectIndex >= 0 && nWaitObjectIndex < count);
}

template <class T>
/* static */
inline
bool WaitHandle<T>::IsAnyAbandoned(uint32_t uWaitState, int count)
{
    int32_t nAbandonIndex = uWaitState - WaitFor::kAbandoned_0;
    return (nAbandonIndex >= 0 && nAbandonIndex < count);
}

template <class T>
/* static */
inline
bool WaitHandle<T>::WaitAny(const handle_t *pHandles, int count,
                            uint32_t uMillisecTimeout /*= Timeout::kInfinite*/,
                            bool exitContext /*= false*/)
{
    int32_t nWaitObjectIndex = ::WaitForMultipleObjectsEx(count, pHandles, FALSE,
                                    uMillisecTimeout, exitContext) - WaitFor::kSignalled;
    return (nWaitObjectIndex >= 0 && nWaitObjectIndex < count);
}

template <class T>
/* static */
inline
uint32_t WaitHandle<T>::WaitAnyEx(const handle_t *pHandles, int count,
                                  uint32_t uMillisecTimeout /*= Timeout::kInfinite*/,
                                  bool exitContext /*= false*/)
{
    return ::WaitForMultipleObjectsEx(count, pHandles, FALSE, uMillisecTimeout, exitContext);
}

template <class T>
/* static */
inline
bool WaitHandle<T>::WaitAll(const handle_t *pHandles, int count,
                            uint32_t uMillisecTimeout /*= Timeout::kInfinite*/,
                            bool exitContext /*= false*/)
{
    return (::WaitForMultipleObjectsEx(count, pHandles, TRUE, uMillisecTimeout, exitContext)
                                        == WaitFor::kSignalled);
}

template <class T>
/* static */
inline
uint32_t WaitHandle<T>::WaitAllEx(const handle_t *pHandles, int count,
                                  uint32_t uMillisecTimeout /*= Timeout::kInfinite*/,
                                  bool exitContext /*= false*/)
{
    return ::WaitForMultipleObjectsEx(count, pHandles, TRUE, uMillisecTimeout, exitContext);
}

template <class T>
/* static */
inline
bool WaitHandle<T>::SignalAndWait(const handle_t toSignal, const handle_t toWaitOn,
                                  uint32_t uMillisecTimeout /* = Timeout::kInfinite */,
                                  bool exitContext /* = false */)
{
    return (::SignalObjectAndWait(toSignal, toWaitOn, uMillisecTimeout, exitContext)
                                    == WaitFor::kSignalled);
}

template <class T>
/* static */
inline
uint32_t WaitHandle<T>::SignalAndWaitEx(const handle_t toSignal, const handle_t toWaitOn,
                                        uint32_t uMillisecTimeout /* = Timeout::kInfinite */,
                                        bool exitContext /* = false */)
{
    return ::SignalObjectAndWait(toSignal, toWaitOn, uMillisecTimeout, exitContext);
}

NS_JIMI_END

#endif  /* _JIMI_THREAD_WAITHANDLE_H_ */
