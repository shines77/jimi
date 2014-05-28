
#ifndef _JIMI_LOG_H_
#define _JIMI_LOG_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdarg.h>
#include <vector>
#include <list>

#include <jimi/core/jimi_def.h>

using namespace std;

////////////////////////////////////////////////////////////////

// LOG config
#define DO_JIMI_LOG                 1
#define JIMI_LOG_TO_FILE            1
#define JIMI_LOG_TO_SCREEN          1

// about JIMI_TO_SCREEN
#if defined(JIMI_LOG_TO_SCREEN) && (JIMI_LOG_TO_SCREEN != 0)
    #define JIMI_LOG_TO_CONSOLE     1
    #define JIMI_LOG_TO_WINDOW      1
    // JIMI_TO_CONSOLE and JIMI_TO_WINDOW can switch only one,
    // JIMI_TO_CONSOLE is first select
    #if defined(JIMI_LOG_TO_CONSOLE) && (JIMI_LOG_TO_CONSOLE != 0)
        #undef  JIMI_LOG_TO_WINDOW
        #define JIMI_LOG_TO_WINDOW  0
    #endif
#else
    #define JIMI_LOG_TO_CONSOLE     0
    #define JIMI_LOG_TO_WINDOW      0
#endif

#if (!defined(JIMI_LOG_TO_FILE) || (JIMI_LOG_TO_FILE == 0)) && (!defined(JIMI_LOG_TO_SCREEN) || (JIMI_LOG_TO_SCREEN == 0))
    #undef  DO_JIMI_LOG
    #define DO_JIMI_LOG             0
#endif

// TRACE config
#define DO_JIMI_TRACE               1
#define JIMI_TRACE_ON_SCREEN        1
#ifdef _WIN32
#define JIMI_TRACE_IN_DEVENV        1
#else
#define JIMI_TRACE_IN_DEVENV        0
#endif

#if (!defined(JIMI_TRACE_ON_SCREEN) || (JIMI_TRACE_ON_SCREEN == 0)) && (!defined(JIMI_TRACE_IN_DEVENV) || (JIMI_TRACE_IN_DEVENV == 0))
    #undef  DO_JIMI_TRACE
    #define DO_JIMI_TRACE           0
#endif

////////////////////////////////////////////////////////////////

#if defined(DO_JIMI_LOG) && (DO_JIMI_LOG != 0)
#define JIMI_LOG(x)                 ((void)utils.log.print(x))
#define JIMI_LOG0(x)                ((void)utils.log.print(x))
#define JIMI_LOG1(x, a)             ((void)utils.log.print(x, (a)))
#define JIMI_LOG2(x, a, b)          ((void)utils.log.print(x, (a), (b)))
#define JIMI_LOG3(x, a, b, c)       ((void)utils.log.print(x, (a), (b), (c)))
#define JIMI_LOG4(x, a, b, c, d)    ((void)utils.log.print(x, (a), (b), (c), (d)))
#else
#define JIMI_LOG(x)                 ((void)(0))
#define JIMI_LOG0(x)                ((void)(0))
#define JIMI_LOG1(x, a)             ((void)(0))
#define JIMI_LOG2(x, a, b)          ((void)(0))
#define JIMI_LOG3(x, a, b, c)       ((void)(0))
#define JIMI_LOG4(x, a, b, c, d)    ((void)(0))
#endif /* DO_JIMI_LOG */

#define _JIMI_LOG                   utils.log.print

////////////////////////////////////////////////////////////////

#if defined(DO_JIMI_TRACE) && (DO_JIMI_TRACE != 0)
/*
#include <cstdio>
#define JIMI_TRACE(x)               ((void)std::printf(x))
#define JIMI_TRACE0(x)              ((void)std::printf(x))
#define JIMI_TRACE1(x, a)           ((void)std::printf(x, a))
#define JIMI_TRACE2(x, a, b)        ((void)std::printf(x, a, b))
#define JIMI_TRACE3(x, a, b, c)     ((void)std::printf(x, a, b, c))
//*/
#define JIMI_TRACE(x)               ((void)utils.log.trace(x))
#define JIMI_TRACE0(x)              ((void)utils.log.trace(x))
#define JIMI_TRACE1(x, a)           ((void)utils.log.trace(x, (a)))
#define JIMI_TRACE2(x, a, b)        ((void)utils.log.trace(x, (a), (b)))
#define JIMI_TRACE3(x, a, b, c)     ((void)utils.log.trace(x, (a), (b), (c)))
#define JIMI_TRACE4(x, a, b, c, d)  ((void)utils.log.trace(x, (a), (b), (c), (d)))
#else
#define JIMI_TRACE(x)               ((void)(0))
#define JIMI_TRACE0(x)              ((void)(0))
#define JIMI_TRACE1(x, a)           ((void)(0))
#define JIMI_TRACE2(x, a, b)        ((void)(0))
#define JIMI_TRACE3(x, a, b, c)     ((void)(0))
#define JIMI_TRACE4(x, a, b, c, d)  ((void)(0))
#endif /* DO_JIMI_TRACE */

#define _JIMI_TRACE                 utils.log.trace

////////////////////////////////////////////////////////////////

#if _MSC_VER
    #define snprintf    _snprintf
    #define snprintf_s  _snprintf_s
#if _MSC_VER <= 1400
    #define vsnprintf   _vsnprintf
    #define vsnprintf_s _vsnprintf_s
#endif
#endif

////////////////////////////////////////////////////////////////

#define MAX_LOG_TEXT_LEN            1024

#ifdef _WIN32
#define DEFAULT_LOG_FILENAME        "jimi_server.log"
#define DEFAULT_LOG_FILENAME_GLOBAL "jimi_global.log"
#else
#define DEFAULT_LOG_FILENAME        "jimi_server.log"
#define DEFAULT_LOG_FILENAME_GLOBAL "jimi_global.log"
#endif

#define DEFAULT_LOG_FORMAT          "[$datetime] [$type] $msg"

#define MAX_LOG_FILENAME_LEN        260
#define STRING_CRLF                 "\r\n"

#define DEFAULT_LOG_TITLE           "Noname Log"
#define DEFAULT_LOG_TITLE_GLOBAL    "Global Log"

#define LOG_TITLE_LINE_LENGTH       60
#define LOG_TITLE_EDGE_LENGTH       1
#define LOG_TITLE_MIN_LENGTH        (LOG_TITLE_EDGE_LENGTH * 2 + 2)
#define LOG_TITLE_STR_BEGIN         "Begin"
#define LOG_TITLE_STR_END           "End"
#define LOG_TITLE_STR_REPEAT_SEC    "========="
#define LOG_TITLE_STR_REPEAT        "="

////////////////////////////////////////////////////////////////

NS_JIMI_BEGIN

NS_JIMI_UTIL_BEGIN

/**
    Global  	Generic level that represents all levels. Useful when setting global configuration for all levels.
    Debug 	    Informational events most useful for developers to debug application.
                Only applicable if NDEBUG is not defined (for non-VC++) or _DEBUG is defined (for VC++).
    Info 	    Mainly useful to represent current progress of application.
    Verbose 	Information that can be highly useful and vary with verbose logging level.
    Trace 	    Information that can be useful to back-trace certain events - mostly useful than debug logs.
    Warning 	Information representing errors in application but application will keep running.
    Error 	    Error information but will continue application to keep running.
    Fatal 	    Very severe error event that will presumably lead the application to abort.
 */
typedef enum Log_Type
{
    LOG_FIRST = -1,
    LOG_GLOBAL = 0, // 全局设置
    LOG_PRINT,      // 直接输出
    LOG_OUT,        // 日志(Log)
    LOG_CONFIG,     // 配置
    LOG_INFO,       // 信息
    LOG_VERBOSE,    // 详细
    LOG_TRACE,      // 跟踪
    LOG_DEBUG,      // 调试
    LOG_WARN,       // 警告
    LOG_ERROR,      // 错误
    LOG_FATAL,      // 致命的
    LOG_UNKNOWN,    // 致命的
    LOG_ALL,        // 全打开
    LOG_OFF,        // 全关闭
    LOG_LAST
} Log_Type_e;

typedef struct LogType_NodeInfo
{
    int     type_id;
    int     type_index;
    char    type_name[32];
} LogType_NodeInfo;

typedef struct LogType_Node
{
    int     type_id;
    char    type_name[32];
    bool    enabled;
    bool    to_file;            // Whether or not to write corresponding log to log file
    bool    to_screen;          // Whether or not to write logs to standard output e.g, terminal or command prompt
    bool    to_trace;           // Whether or not to write logs to the MSVC trace window
    bool    to_remote;          // Whether or not to write logs to remote server or gdb
} LogType_Node;

typedef struct LogConf_Node
{
    int             type_id;
    char            type_name[32];
    char            filename[MAX_LOG_FILENAME_LEN];
    char            title[64];
    char            format[128];
    bool            enabled;
    bool            to_file;            // Whether or not to write corresponding log to log file
    bool            to_screen;          // Whether or not to write logs to standard output e.g, terminal or command prompt
    bool            to_trace;           // Whether or not to write logs to the MSVC trace window
    bool            to_remote;          // Whether or not to write logs to remote server or gdb
    bool            create_new;
    unsigned int    attrib;
    unsigned int    max_log_file_size;
    int             log_flush_threshold;
} LogConf_Node;

typedef struct LogConf_Global
{
    LogConf_Node    node;
    int             type_mask;
    int             min_type_id;
    int             max_type_id;
    int             max_type_cnt;
    int             max_type_len;
    char            remote_ip[48];
    unsigned short  remote_port;
} LogConf_Global;

class LogConfig
{
public:
    typedef vector<LogConf_Node *>          LogConf_NodeVector;
    typedef LogConf_NodeVector::iterator    LogConf_NodeIter;

public:
    LogConfig(const char *filename = NULL);
    virtual ~LogConfig();

    void  destroy();
    bool  init(const char *filename);
    bool  open(const char *filename);
    void  close();

protected:
    bool  copy_from_global(LogConf_Global &_global, LogConf_Node *_node, int _index);

private:
    int   get_max_type_len();
    int   get_type_info();

public:
    LogConf_Global      global;
    LogConf_NodeVector  typeList;
    LogType_NodeInfo   *typeInfoList;
};

class Logger
{
public:
    Logger(const char *filename = NULL, const char *title = NULL, unsigned int attrib = 0);
    virtual ~Logger();

private:
    explicit Logger(Logger &src);
    explicit Logger(int x);
    explicit Logger(unsigned x);

public:
    static Logger *getInstance(const char *filename = NULL,
        const char *title = NULL, unsigned int attrib = 0);

    bool  open(const char *filename, const char *title = NULL);
    int   close();
    int   flush();
    void  destroy();

    void  log_begin(const char *title = NULL);
    void  log_end(const char *title = NULL);

    void  out(unsigned int type, const char *fmt, ...);

    void  write(const char *fmt, ...);
    void  writeln(const char *fmt, ...);

    void  print(const char *fmt, ...);
    void  println(const char *fmt, ...);

    void  log(const char *fmt, ...);
    void  config(const char *fmt, ...);
    void  info(const char *fmt, ...);
    void  verbose(const char *fmt, ...);
    void  trace(const char *fmt, ...);
    void  debug(const char *fmt, ...);
    void  warn(const char *fmt, ...);
    void  error(const char *fmt, ...);
    void  fatal(const char *fmt, ...);

    void  printf(const char *fmt, ...);
    void  _printf(bool newline, const char *fmt, ...);

    static Logger *s_Log;

protected:
    FILE *open_file(const char *filename);

    void  vprintf(const char *fmt, va_list arglist);
    void  vprintf(bool newline, const char *fmt, va_list arglist);
    void  vprintf(bool newline, const char *type_format, const char *type_name,
        const char *fmt, va_list arglist);

    void  outv(unsigned int type, bool newline,
        const char *fmt, va_list arglist);

    void  logTitleSection(const char *title, bool isBegin);
    void  logTitleLine(const char *title, int line_length);

private:
    FILE *m_log_file;
    char *m_filename;
    char *m_title;
    char *m_format;

    LogConfig m_config;
};

NS_JIMI_UTIL_END

class _Utils
{
public:
    _Utils() : log() {};
    ~_Utils() {};

public:
    util::Logger log;
};

extern _Utils   utils;

class _System
{
public:
    _System() : out(utils.log) {};
    ~_System() {};

    util::Logger &out;
};

extern _System  System;

NS_JIMI_END

#endif  /* _JIMI_LOG_H_ */
