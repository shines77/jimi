
#ifndef _JIMI_SYSTEM_PROGRAM_H_
#define _JIMI_SYSTEM_PROGRAM_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/core/jimi_def.h"
#include "jimi/internal/NonCopyable.h"
#include "jimi/lang/String.h"

#include <string>
#include <vector>

NS_JIMI_BEGIN

class Program : public internal::NonCopyable
{
public:
    typedef std::vector<std::string>  StringArray;
    typedef std::vector<std::wstring> WStringArray;

    typedef void (*func_main_void) (int, char **);
    typedef void (*func_wmain_void)(int, wchar_t **);
    typedef int  (*func_main_int)  (int, char **);
    typedef int  (*func_wmain_int) (int, wchar_t **);
    typedef void (*func_modern_main) (StringArray args);
    typedef void (*func_modern_wmain)(WStringArray args);

    enum Main_Mode {
        PlainMode = 0,
        ModernMode
    };

    Program() : m_nMode(PlainMode), m_nReturnValue(0), m_pfnMainVoid(NULL), m_pfnWMainVoid(NULL),
        m_pfnMainInt(NULL), m_pfnWMainInt(NULL), m_pfnModernMain(NULL), m_pfnModernWMain(NULL) {}

    Program(func_main_void pfnMain, int nReturnValue = 0)
        : m_nMode(PlainMode), m_nReturnValue(nReturnValue), m_pfnMainVoid(pfnMain), m_pfnWMainVoid(NULL),
          m_pfnMainInt(NULL), m_pfnWMainInt(NULL), m_pfnModernMain(NULL), m_pfnModernWMain(NULL) {}

    Program(func_wmain_void pfnMain, int nReturnValue = 0)
        : m_nMode(PlainMode), m_nReturnValue(nReturnValue), m_pfnMainVoid(NULL), m_pfnWMainVoid(pfnMain),
          m_pfnMainInt(NULL), m_pfnWMainInt(NULL), m_pfnModernMain(NULL), m_pfnModernWMain(NULL) {}

    Program(func_main_int pfnMain, int nReturnValue = 0)
        : m_nMode(PlainMode), m_nReturnValue(nReturnValue), m_pfnMainVoid(NULL), m_pfnWMainVoid(NULL),
          m_pfnMainInt(pfnMain), m_pfnWMainInt(NULL), m_pfnModernMain(NULL), m_pfnModernWMain(NULL) {}

    Program(func_wmain_int pfnMain, int nReturnValue = 0)
        : m_nMode(PlainMode), m_nReturnValue(nReturnValue), m_pfnMainVoid(NULL), m_pfnWMainVoid(NULL),
          m_pfnMainInt(NULL), m_pfnWMainInt(pfnMain), m_pfnModernMain(NULL), m_pfnModernWMain(NULL) {}

    Program(func_modern_main pfnMain, int nReturnValue = 0)
        : m_nMode(ModernMode), m_nReturnValue(nReturnValue), m_pfnMainVoid(NULL), m_pfnWMainVoid(NULL),
          m_pfnMainInt(NULL), m_pfnWMainInt(NULL), m_pfnModernMain(pfnMain), m_pfnModernWMain(NULL) {}

    Program(func_modern_wmain pfnMain, int nReturnValue = 0)
        : m_nMode(ModernMode), m_nReturnValue(nReturnValue), m_pfnMainVoid(NULL), m_pfnWMainVoid(NULL),
          m_pfnMainInt(NULL), m_pfnWMainInt(NULL), m_pfnModernMain(NULL), m_pfnModernWMain(pfnMain) {}

    ~Program() {};

    void SetMain(func_main_void pfnMain)    { ResetSettings(); m_pfnMainVoid    = pfnMain; m_nMode = PlainMode;  }
    void SetMain(func_wmain_void pfnMain)   { ResetSettings(); m_pfnWMainVoid   = pfnMain; m_nMode = PlainMode;  }
    void SetMain(func_main_int pfnMain)     { ResetSettings(); m_pfnMainInt     = pfnMain; m_nMode = PlainMode;  }
    void SetMain(func_wmain_int pfnMain)    { ResetSettings(); m_pfnWMainInt    = pfnMain; m_nMode = PlainMode;  }
    void SetMain(func_modern_main pfnMain)  { ResetSettings(); m_pfnModernMain  = pfnMain; m_nMode = ModernMode; }
    void SetMain(func_modern_wmain pfnMain) { ResetSettings(); m_pfnModernWMain = pfnMain; m_nMode = ModernMode; }

    int GetMode() { return m_nMode; }

    int GetReturnValue() { return m_nReturnValue; }
    void SetReturnValue(int value) {
        m_nReturnValue = value;
    }

    int Run(int argc, char *argv[]) {
        if (m_pfnMainVoid) {
            m_pfnMainVoid(argc, argv);
        }
        else if (m_pfnMainInt) {
            m_nReturnValue = m_pfnMainInt(argc, argv);
        }
        else if (m_pfnModernMain) {
            StringArray args;
            args.reserve(argc);
            for (int i = 0; i < argc; ++i)
                args.push_back(argv[i]);
            m_pfnModernMain(args);
        }
        else if (m_nMode == PlainMode) {
            Main(argc, argv);
        }
        else if (m_nMode == ModernMode) {
            StringArray args;
            args.reserve(argc);
            for (int i = 0; i < argc; ++i)
                args.push_back(argv[i]);
            Main(args);
        }
        else {
            // Get some error
        }
        return m_nReturnValue;
    }

    int Run(int argc, wchar_t *argv[]) {
        if (m_pfnWMainVoid) {
            m_pfnWMainVoid(argc, argv);
        }
        else if (m_pfnWMainInt) {
            m_nReturnValue = m_pfnWMainInt(argc, argv);
        }
        else if (m_pfnModernMain) {
            WStringArray args;
            args.reserve(argc);
            for (int i = 0; i < argc; ++i)
                args.push_back(argv[i]);
            m_pfnModernWMain(args);
        }
        else if (m_nMode == PlainMode) {
            Main(argc, argv);
        }
        else if (m_nMode == ModernMode) {
            WStringArray args;
            args.reserve(argc);
            for (int i = 0; i < argc; ++i)
                args.push_back(argv[i]);
            Main(args);
        }
        else {
            // Get some error
        }
        return m_nReturnValue;
    }

    virtual void Main(int argc, char *argv[]) {
        // TODO:
    }

    virtual void Main(int argc, wchar_t *argv[]) {
        // TODO:
    }

    virtual void Main(StringArray args) {
        // TODO:
    }

    virtual void Main(WStringArray args) {
        // TODO:
    }

protected:
    void ResetSettings() {
        m_pfnMainVoid = NULL; m_pfnWMainVoid = NULL;  m_pfnMainInt = NULL;
        m_pfnWMainInt = NULL; m_pfnModernMain = NULL; m_pfnModernWMain = NULL;
    }

    void SetMode(int mode) {
        m_nMode = mode;
    }

private:
    int                 m_nMode;
    int                 m_nReturnValue;
    func_main_void      m_pfnMainVoid;
    func_wmain_void     m_pfnWMainVoid;
    func_main_int       m_pfnMainInt;
    func_wmain_int      m_pfnWMainInt;
    func_modern_main    m_pfnModernMain;
    func_modern_wmain   m_pfnModernWMain;
};

NS_JIMI_END

#endif  /* !_JIMI_SYSTEM_PROGRAM_H_ */
