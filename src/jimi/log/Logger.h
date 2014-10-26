
#ifndef _JIMI_LOG_LOGGER_H_
#define _JIMI_LOG_LOGGER_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>
#include <jimi/log/log_def.h>

#include <stdarg.h>
#include <vector>
#include <list>

#if JIMI_IS_WINDOWS

#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#else

#ifndef MAX_PATH
#define MAX_PATH    260
#endif

#endif

using namespace std;

#if defined(_MSC_VER)
    #pragma warning (push)
    #pragma warning (disable : 4251)
    #pragma warning (disable : 4275)
#endif

////////////////////////////////////////////////////////////////

#include <jimi/log/log_config.h>

////////////////////////////////////////////////////////////////

#if defined(JIMI_LOG_ENABLE) && (JIMI_LOG_ENABLE != 0)
#define JIMI_LOG(x)                     ((void)NS_JIMI_LOG(utils.log.print(x)))
#define JIMI_LOG0(x)                    ((void)NS_JIMI_LOG(utils.log.print(x)))
#define JIMI_LOG1(x,a)                  ((void)NS_JIMI_LOG(utils.log.print(x,(a))))
#define JIMI_LOG2(x,a,b)                ((void)NS_JIMI_LOG(utils.log.print(x,(a),(b))))
#define JIMI_LOG3(x,a,b,c)              ((void)NS_JIMI_LOG(utils.log.print(x,(a),(b),(c))))
#define JIMI_LOG4(x,a,b,c,d)            ((void)NS_JIMI_LOG(utils.log.print(x,(a),(b),(c),(d))))
#else
#define JIMI_LOG(x)                     ((void)(0))
#define JIMI_LOG0(x)                    ((void)(0))
#define JIMI_LOG1(x,a)                  ((void)(0))
#define JIMI_LOG2(x,a,b)                ((void)(0))
#define JIMI_LOG3(x,a,b,c)              ((void)(0))
#define JIMI_LOG4(x,a,b,c,d)            ((void)(0))
#endif /* JIMI_LOG_ENABLE */

#ifndef _JIMI_LOG
#define _JIMI_LOG                       NS_JIMI_LOG(utils.log.print)
#endif

#ifndef jmLog
#define jmLog                           NS_JIMI_LOG(utils.log)
#endif

#ifndef DebugLog
#define DebugLog                        NS_JIMI_LOG(utils.log)
#endif

////////////////////////////////////////////////////////////////

#if defined(JIMI_LOG_TRACE_ENABLE) && (JIMI_LOG_TRACE_ENABLE != 0)
/*
#include <cstdio>
#define JIMI_LOG_TRACE(x)               ((void)std::printf(x))
#define JIMI_LOG_TRACE0(x)              ((void)std::printf(x))
#define JIMI_LOG_TRACE1(x,a)            ((void)std::printf(x,a))
#define JIMI_LOG_TRACE2(x,a,b)          ((void)std::printf(x,a,b))
#define JIMI_LOG_TRACE3(x,a,b,c)        ((void)std::printf(x,a,b,c))
//*/
#define JIMI_LOG_TRACE(x)               ((void)NS_JIMI_LOG(utils.log.trace(x)))
#define JIMI_LOG_TRACE0(x)              ((void)NS_JIMI_LOG(utils.log.trace(x)))
#define JIMI_LOG_TRACE1(x,a)            ((void)NS_JIMI_LOG(utils.log.trace(x,(a))))
#define JIMI_LOG_TRACE2(x,a,b)          ((void)NS_JIMI_LOG(utils.log.trace(x,(a),(b)))
#define JIMI_LOG_TRACE3(x,a,b,c)        ((void)NS_JIMI_LOG(utils.log.trace(x,(a),(b),(c))))
#define JIMI_LOG_TRACE4(x,a,b,c,d)      ((void)NS_JIMI_LOG(utils.log.trace(x,(a),(b),(c),(d))))
#else
#define JIMI_LOG_TRACE(x)               ((void)(0))
#define JIMI_LOG_TRACE0(x)              ((void)(0))
#define JIMI_LOG_TRACE1(x,a)            ((void)(0))
#define JIMI_LOG_TRACE2(x,a,b)          ((void)(0))
#define JIMI_LOG_TRACE3(x,a,b,c)        ((void)(0))
#define JIMI_LOG_TRACE4(x,a,b,c,d)      ((void)(0))
#endif /* JIMI_LOG_TRACE_ENABLE */

#ifndef _JIMI_LOG
#define _JIMI_LOG_TRACE                 NS_JIMI_LOG(utils.log.trace)
#endif

#ifndef TraceLog
#define TraceLog                        NS_JIMI_LOG(utils.log)
#endif

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
#endif  /* _WIN32 */

#define DEFAULT_LOG_FORMAT          "[$datetime] [$type] $msg"

#define MAX_LOG_PATH                MAX_PATH        // 260
#define MAX_LOG_TITLE               128
#define STRING_CRLF                 "\r\n"

#define DEFAULT_LOG_TITLE           "Noname Log"
#define DEFAULT_LOG_TITLE_GLOBAL    "Global Log"

#define LOG_TITLE_LINE_LENGTH       60
#define LOG_TITLE_EDGE_LENGTH       1
#define LOG_TITLE_MIN_LENGTH        (LOG_TITLE_EDGE_LENGTH * 2 + 2)
#define LOG_TITLE_STR_BEGIN         "Begin"
#define LOG_TITLE_STR_END           "End"
#define LOG_TITLE_STR_REPEAT_SEC    "========"
#define LOG_TITLE_STR_REPEAT        "="

////////////////////////////////////////////////////////////////

NS_JIMI_BEGIN

NS_JIMI_LOG_BEGIN

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
    LOG_UNKNOWN,    // 未知
    LOG_ALL,        // 全打开
    LOG_OFF,        // 全关闭
    LOG_LAST
} Log_Type_t;

typedef struct JIMI_API LogType_NodeInfo
{
    int     tag_id;
    int     tag_index;
    char    tag_name[32];
} LogType_NodeInfo;

typedef struct JIMI_API LogType_Node
{
    int     tag_id;
    char    tag_name[32];
    bool    enabled;
    bool    to_file;            // Whether or not to write corresponding log to log file
    bool    to_screen;          // Whether or not to write logs to standard output e.g, terminal or command prompt
    bool    to_trace;           // Whether or not to write logs to the MSVC trace window
    bool    to_remote;          // Whether or not to write logs to remote server or gdb
} LogType_Node;

typedef struct JIMI_API LogConf_Node
{
    int             tag_id;
    char            tag_name[32];
    char            filename[MAX_LOG_PATH];
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

typedef struct JIMI_API LogConf_Global
{
    LogConf_Node    node;
    int             tag_mask;
    int             min_tag_id;
    int             max_tag_id;
    int             max_tag_cnt;
    int             max_tag_len;
    char            remote_ip[48];
    unsigned short  remote_port;
} LogConf_Global;

class JIMI_API LogConfig
{
public:
    typedef std::vector<LogConf_Node *>     LogConf_NodeVec;
    typedef LogConf_NodeVec::iterator       LogConf_NodeVecIter;

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
    int   get_max_tag_len();
    int   get_tags_info();

public:
    LogConf_Global          global;
    LogConf_NodeVec         typeList;
    LogType_NodeInfo       *infoList;
};

class JIMI_API Logger
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

    void  out(int tag, const char *fmt, ...);

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
    size_t get_log_filename(const char *filename, char **new_filename);

    static size_t get_app_path(char *app_path, size_t buf_len);
    static size_t get_app_filename(const char *filename, char *new_filename, size_t buf_len);
    static size_t make_app_filename(const char *filename, char **new_filename);

    void  vprintf(const char *fmt, va_list arglist);
    void  vprintf(bool newline, const char *fmt, va_list arglist);
    void  vprintf(bool newline, const char *tag_format, const char *tag_name,
        size_t tag_len, const char *fmt, va_list arglist);

    void  outv(int tag, bool newline, const char *fmt, va_list arglist);

    void  log_title_section(const char *title, bool isBegin);
    void  log_title_line(const char *title, int line_length);

private:
    FILE *m_log_file;
    char m_filename[MAX_LOG_PATH + 64];
    char m_title[MAX_LOG_TITLE];
    char m_format[128];

    LogConfig m_config;
};

class JIMI_API _Utils
{
public:
    _Utils() : log(DEFAULT_LOG_FILENAME_GLOBAL, DEFAULT_LOG_TITLE_GLOBAL) {};
    ~_Utils() {};

public:
    Logger log;
};

extern _Utils   utils;

class JIMI_API _System
{
public:
    _System() : out(utils.log) {};
    ~_System() {};

    Logger &out;
};

extern _System  System;

NS_JIMI_LOG_END

NS_JIMI_END

JIMI_EXPIMP_TEMPLATE template class JIMI_API_TPL std::allocator<jimi::log::LogConf_Node *>;
JIMI_EXPIMP_TEMPLATE template class JIMI_API_TPL std::vector<jimi::log::LogConf_Node *, std::allocator<jimi::log::LogConf_Node *> >;

#if defined(_MSC_VER)
    #pragma warning (pop)
#endif

#endif  /* _JIMI_LOG_LOGGER_H_ */
