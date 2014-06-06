
#ifndef _JIMI_THREAD_EVENT_H_
#define _JIMI_THREAD_EVENT_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>
#include <jimi/thread/thread_def.h>
#include <jimi/thread/WaitHandle.h>
#include <jimi/log/log.h>
#include <jimic/string/jm_strings.h>

#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#include <windows.h>

NS_JIMI_BEGIN

NS_JIMI_SYSTEM_BEGIN

class EventWaitHandle : public WaitHandle<EventWaitHandle>
{
public:
    EventWaitHandle();
    EventWaitHandle(bool bInitialState, bool bManualReset, const char *lpEventName = NULL,
        bool *bCreatedNew = NULL, LPSECURITY_ATTRIBUTES lpEventAttributes = NULL);
    ~EventWaitHandle();

    void Close();

private:
    char   *m_pEventName;
    int     m_nManualReset;
    bool    m_bInitialState;
    bool    m_bCreatedNew;

    LPSECURITY_ATTRIBUTES m_pEventAttributes;
};

EventWaitHandle::EventWaitHandle()
: WaitHandle<EventWaitHandle>()
, m_pEventName(NULL)
, m_nManualReset(EventResetMode::kUndefined)
, m_bInitialState(SignalState::kNoSignalled)
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
    if (lpEventName != NULL) {
        size_t lenName = jm_strlen(lpEventName);
        lenName = JIMI_MIN(lenName + 1, MAX_PATH);
        m_pEventName = new char[lenName];
        if (m_pEventName) {
            jm_strncpy(m_pEventName, lenName, lpEventName, lenName - 1);
            m_pEventName[lenName] = '\0';
        }
        m_bCreatedNew = false;
    }
    else
        m_bCreatedNew = true;

    m_nManualReset  = bManualReset;
    m_bInitialState = bInitialState;
    m_pEventAttributes = lpEventAttributes;

    m_hHandle = ::CreateEvent(lpEventAttributes, bManualReset, bInitialState, lpEventName);
    if ((!HANDLE_IS_VALID(m_hHandle)) || (::GetLastError() == ERROR_ALREADY_EXISTS)) {
        m_bCreatedNew = false;
    }

    if (bCreatedNew != NULL)
        *bCreatedNew = m_bCreatedNew;
}

EventWaitHandle::~EventWaitHandle()
{
    Close();
}

void EventWaitHandle::Close()
{
    if (m_pEventName) {
        delete m_pEventName;
        m_pEventName = NULL;
    }
    if (IsValid()) {
        ::CloseHandle(m_hHandle);
        m_hHandle = NULL;
    }
}

class ManualResetEvent : EventWaitHandle
{
public:
    ManualResetEvent();
    ~ManualResetEvent();
};

class AutoResetEvent : EventWaitHandle
{
public:
    AutoResetEvent();
    ~AutoResetEvent();
};

NS_JIMI_SYSTEM_END

NS_JIMI_END

#endif  /* _JIMI_THREAD_EVENT_H_ */
