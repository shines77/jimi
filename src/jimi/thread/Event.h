
#ifndef _JIMI_THREAD_EVENT_H_
#define _JIMI_THREAD_EVENT_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/core/jimi_def.h"
#include "jimi/thread/ThreadDef.h"
#include "jimi/thread/WaitHandle.h"
#include "jimi/log/log.h"
#include "jimic/string/jm_strings.h"

#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#include <windows.h>

NS_JIMI_BEGIN

/**************************************************************************
 ** EventWaitHandle
 **************************************************************************/

class EventWaitHandle : public WaitHandle<EventWaitHandle>
{
public:
    EventWaitHandle();
    EventWaitHandle(bool bInitialState, bool bManualReset, const char *lpEventName = NULL,
        bool *bCreatedNew = NULL, LPSECURITY_ATTRIBUTES lpEventAttributes = NULL);
    ~EventWaitHandle();

    void Close();

    handle_t Create(bool bInitialState, bool bManualReset, const char *lpEventName = NULL,
        bool *bCreatedNew = NULL, LPSECURITY_ATTRIBUTES lpEventAttributes = NULL);

    bool Reset();
    bool Set();

    bool Pulse();

    uint32_t GetAccessControl() { return m_dwDesiredAccess; }
    void SetAccessControl(uint32_t dwDesiredAccess);

    static handle_t OpenExisting(const char *lpEventName = NULL, bool bInheritHandle = false,
        uint32_t dwDesiredAccess = EVENT_ALL_ACCESS);

private:
    char    *m_pEventName;
    int      m_nManualReset;
    bool     m_bInitialState;
    bool     m_bCreatedNew;
    uint32_t m_dwDesiredAccess;

    LPSECURITY_ATTRIBUTES m_pEventAttributes;
};

EventWaitHandle::EventWaitHandle()
: WaitHandle<EventWaitHandle>()
, m_pEventName(NULL)
, m_nManualReset(EventResetMode::kUndefined)
, m_bInitialState(SignalState::kNoSignalled)
, m_dwDesiredAccess(EVENT_ALL_ACCESS)
, m_bCreatedNew(false)
, m_pEventAttributes(NULL)
{
}

/**
 * The name can have a "Global\" or "Local\" prefix to explicitly create the object
 *   in the global or session namespace. The remainder of the name can contain any
 *   character except the backslash character (\).
 **/
EventWaitHandle::EventWaitHandle(bool bInitialState, bool bManualReset,
                                 const char *lpEventName /* = NULL */,
                                 bool *bCreatedNew /* = NULL */,
                                 LPSECURITY_ATTRIBUTES lpEventAttributes /* = NULL */)
{
    this->Create(bInitialState, bManualReset, lpEventName, bCreatedNew, lpEventAttributes);
}

EventWaitHandle::~EventWaitHandle()
{
    jmLog.info("-----------------------------------------------");
    jmLog.info("EventWaitHandle::~EventWaitHandle() Enter.");
    this->Close();
    jmLog.info("EventWaitHandle::~EventWaitHandle() Over.");
    jmLog.info("-----------------------------------------------");
}

inline
void EventWaitHandle::Close()
{
    jmLog.info("EventWaitHandle::Close() Enter.");
    if (m_pEventName) {
        delete m_pEventName;
        m_pEventName = NULL;
    }
    if (IsValid()) {
        ::CloseHandle(m_hHandle);
        m_hHandle = NULL;
    }
    jmLog.info("EventWaitHandle::Close() Over.");
}

inline
EventWaitHandle::handle_t
EventWaitHandle::Create(bool bInitialState, bool bManualReset,
                        const char *lpEventName /* = NULL */,
                        bool *bCreatedNew /* = NULL */,
                        LPSECURITY_ATTRIBUTES lpEventAttributes /* = NULL */)
{
    if (IsValid())
        return NULL;

    if (lpEventName != NULL) {
        if (m_pEventName)
            delete m_pEventName;

        size_t lenName = ::jm_strlen(lpEventName);
        lenName = JIMI_MIN(lenName + 1, MAX_PATH);
        m_pEventName = new char[lenName];
        if (m_pEventName) {
            ::jm_strncpy(m_pEventName, lenName, lpEventName, lenName - 1);
            m_pEventName[lenName] = '\0';
        }
        m_bCreatedNew = false;
    }
    else {
        m_pEventName = NULL;
        m_bCreatedNew = true;
    }

    m_nManualReset  = bManualReset;
    m_bInitialState = bInitialState;
    m_pEventAttributes = lpEventAttributes;

    m_hHandle = ::CreateEvent(lpEventAttributes, bManualReset, bInitialState, lpEventName);
    if ((!HANDLE_IS_VALID(m_hHandle)) || (::GetLastError() == ERROR_ALREADY_EXISTS)) {
        m_bCreatedNew = false;
    }

    if (bCreatedNew != NULL)
        *bCreatedNew = m_bCreatedNew;

    return m_hHandle;
}

inline
EventWaitHandle::handle_t
EventWaitHandle::OpenExisting(const char *lpEventName /* = NULL */,
                              bool bInheritHandle/* = false */,
                              uint32_t dwDesiredAccess /* = EVENT_ALL_ACCESS */)
{
    return (handle_t)::OpenEvent(dwDesiredAccess, bInheritHandle, lpEventName);
}

inline
bool EventWaitHandle::Reset()
{
    BOOL bSuccess = FALSE;
    if (IsValid())
        bSuccess = ::ResetEvent(m_hHandle);
    return (bSuccess != FALSE);
}

inline
bool EventWaitHandle::Set()
{
    BOOL bSuccess = FALSE;
    if (IsValid())
        bSuccess = ::SetEvent(m_hHandle);
    return (bSuccess != FALSE);
}

/**
 * SetEvent与PulseEvent的区别
 *
 * Reference:
 *   http://blog.sina.com.cn/s/blog_6163bdeb0100qlw1.html
 *
 * 在自动重置模式下PulseEvent和SetEvent的作用没有什么区别, 但在手动模式下
 *   PulseEvent就有明显的不同, 可以比较容易的控制程序是单步走, 还是连续走.
 *
 **/
inline
bool EventWaitHandle::Pulse()
{
    BOOL bSuccess = FALSE;
    if (IsValid())
        bSuccess = ::PulseEvent(m_hHandle);
    return (bSuccess != FALSE);
}

/**************************************************************************
 ** ManualResetEvent
 **************************************************************************/

class ManualResetEvent : public EventWaitHandle
{
public:
    ManualResetEvent(bool bInitialState, const char *lpEventName = NULL)
        : EventWaitHandle(bInitialState, EventResetMode::kManualReset, lpEventName) { };
    ~ManualResetEvent() { jmLog.info("ManualResetEvent::~ManualResetEvent()."); };
};

/**************************************************************************
 ** AutoResetEvent
 **************************************************************************/

class AutoResetEvent : public EventWaitHandle
{
public:
    AutoResetEvent(bool bInitialState, const char *lpEventName = NULL)
        : EventWaitHandle(bInitialState, EventResetMode::kAutoReset, lpEventName) { };
    ~AutoResetEvent() { jmLog.info("AutoResetEvent::~AutoResetEvent()."); };
};

NS_JIMI_END

#endif  /* _JIMI_THREAD_EVENT_H_ */
