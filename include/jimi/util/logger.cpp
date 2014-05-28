
#include <jimi/util/logger.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include <share.h>
#include <windows.h>

NS_JIMI_BEGIN

_Utils  utils;
_System System;

NS_JIMI_UTIL_BEGIN

Logger *Logger::s_Log = NULL;

/* Compilers */
const JIMI_MACRO_T s_compilers[] = {
    { "[Compiler]", "" },

#ifdef _MSC_VER         /* Visual C++ */
    JIMI_MAKE_STR(_MSC_VER),
#endif

#ifdef _DLL             /* _DLL */
    JIMI_MAKE_STR(_DLL),
#endif

#ifdef _LIB             /* _LIB */
    JIMI_MAKE_STR(_LIB),
#endif

#ifdef JIMI_USE_STATIC    /* JIMI_USE_STATIC */
    JIMI_MAKE_STR(JIMI_USE_STATIC),
#endif

#ifdef LOG4CC_USE_STATIC    /* LOG4CC_USE_STATIC */
    JIMI_MAKE_STR(LOG4CC_USE_STATIC),
#endif

#ifdef __FILE__    /* __FILE__ */
    JIMI_MAKE_STR(__FILE__),
#endif
};

LogType_Node default_LogTypeList[] = {
    /* type_id      type_name  enabled  file   screen   trace  remote */
    { LOG_GLOBAL,   "global",   true,   true,   true,   true,   true    },
    { LOG_PRINT,    "write",    true,   true,   true,   false,  true    },
    { LOG_OUT,      "log",      true,   true,   true,   false,  true    },
    { LOG_CONFIG,   "config",   true,   true,   false,  false,  true    },
    { LOG_INFO,     "info",     true,   true,   true,   false,  true    },
    { LOG_VERBOSE,  "verbose",  true,   true,   true,   false,  true    },
    { LOG_TRACE,    "trace",    true,   true,   false,  true,   true    },
    { LOG_DEBUG,    "debug",    true,   true,   true,   false,  true    },
    { LOG_WARN,     "warn",     true,   true,   true,   false,  true    },
    { LOG_ERROR,    "error",    true,   true,   true,   false,  true    },
    { LOG_FATAL,    "fatal",    true,   true,   true,   false,  true    },
    { LOG_UNKNOWN,  "unknown",  true,   true,   true,   false,  true    }
};

///////////////////////////////////////////////////////////////////////////
// global function(s)

/************************************************************************
 *
 * C string replace function
 * Descript: return a replacement string,
 *      exhausted after use free () to release.
 *   If the call succeeds returns a pointer that point the string is
 *      replaced with the new, returns NULL if it fails
 *
 * C 字符串替换函数
 * 描述: 返回一个替换以后的字符串, 用完之后要用free()释放
 *       如果调用成功返回指向被替换后新字符串的指针, 如果失败返回NULL
 *
 ************************************************************************/

char *str_replace(const char *str_buf, const char *src_str, const char *dst_str, bool repeat = true)
{
    char *new_str = NULL, *next;
    unsigned int len, str_len, src_len, dst_len;
    register unsigned int i = 0;
    char *dup_buf;

    if ((str_buf == NULL) || (src_str == NULL) || (dst_str == NULL))
        return NULL;

    dup_buf = (char *)str_buf;
    src_len = strlen(src_str);
    while (strstr(dup_buf, src_str) != NULL) {
        dup_buf = strstr(dup_buf, src_str) + src_len;
        i++;
        if (!repeat)
            break;
    }

    if (i != 0) {
        dup_buf = (char *)str_buf;
        str_len = strlen(str_buf);
        dst_len = strlen(dst_str);
        len = (str_len - (src_len - dst_len) * i + 1) * sizeof(char);
        new_str = (char *)malloc(len);
        if (new_str != NULL) {
            new_str[0] = '\0';
            if (len > 1)
                new_str[len - 1] = '\0';
            //memset(new_str, 0, len);
            while ((next = strstr(dup_buf, src_str)) != NULL) {
                new_str = strncat(new_str, dup_buf, (next - dup_buf));
                //new_str = strcat(new_str, dst_str);
                new_str = strncat(new_str, dst_str, dst_len);
                dup_buf = next + src_len;
                if (!repeat)
                    break;
            }
            new_str = strcat(new_str, dup_buf);
            return new_str;
        }
    }
    else {
        str_len = strlen(str_buf);
        len = (str_len + 1) * sizeof(char);
        new_str = (char *)malloc(len);
        if (new_str != NULL) {
            new_str[0] = '\0';
            //memset(new_str, 0, len);
            new_str = strcpy(new_str, str_buf);
            return new_str;
        }
    }
    return new_str;
}

int get_datetime_str(char *datetime, int bufsize, bool use_millisec = true)
{
    int n;
    SYSTEMTIME sysTime;
    GetLocalTime(&sysTime);

    if (use_millisec) {
        n = _snprintf_s_l(datetime, bufsize, bufsize, "%04d-%02d-%02d %02d:%02d:%02d.%03d", 0,
            sysTime.wYear,
            sysTime.wMonth,
            sysTime.wDay,
            sysTime.wHour,
            sysTime.wMinute,
            sysTime.wSecond,
            sysTime.wMilliseconds);
    }
    else {
        n = _snprintf_s_l(datetime, bufsize, bufsize, "%04d-%02d-%02d %02d:%02d:%02d", 0,
            sysTime.wYear,
            sysTime.wMonth,
            sysTime.wDay,
            sysTime.wHour,
            sysTime.wMinute,
            sysTime.wSecond);
    }
    return n;
}

///////////////////////////////////////////////////////////////////////////
// LogConfig

LogConfig::LogConfig(const char *filename /*= NULL*/)
: typeInfoList(NULL)
{
    init(filename);
}

LogConfig::~LogConfig()
{
    destroy();
}

void LogConfig::destroy()
{
    close();

    if (typeInfoList != NULL) {
        delete[] typeInfoList;
        typeInfoList = NULL;
    }

    LogConf_NodeIter iter;
    for (iter = typeList.begin(); iter != typeList.end(); ++iter) {
        delete (*iter);
    }

    typeList.clear();
}

int LogConfig::get_max_type_len()
{
    int max_type_len = 0;
    int max_list_length = _countof(default_LogTypeList);
    for (int i = 1; i < max_list_length; ++i) {
        int len = strlen(default_LogTypeList[i].type_name);
        if (len > max_type_len) {
            max_type_len = len;
        }
    }
    return max_type_len;
}

int LogConfig::get_type_info()
{
    int i;
    int min_type_id = INT_MAX;
    int max_type_id = 0;
    int max_type_len = 0;
    int max_list_length = _countof(default_LogTypeList);
    for (i = 0; i < max_list_length; ++i) {
        int type_id = default_LogTypeList[i].type_id;
        if (type_id < min_type_id)
            min_type_id = type_id;
        if (type_id > max_type_id)
            max_type_id = type_id;
        int len = strlen(default_LogTypeList[i].type_name);
        if (len > max_type_len) {
            max_type_len = len;
        }
    }

    global.min_type_id  = min_type_id;
    global.max_type_id  = max_type_id;
    global.max_type_len = max_type_len;

    int max_type_cnt = 0;
    LogType_NodeInfo *_pTypeInfoList = new LogType_NodeInfo[max_type_id + 1];
    if (_pTypeInfoList != NULL) {
        for (i = 0; i < max_list_length; ++i) {
            int type_id = default_LogTypeList[i].type_id;
            _pTypeInfoList[type_id].type_id = type_id;
            _pTypeInfoList[type_id].type_index = i - 1;
            strcpy_s(_pTypeInfoList[type_id].type_name, _countof(_pTypeInfoList[type_id].type_name),
                default_LogTypeList[i].type_name);
            max_type_cnt++;
        }
        typeInfoList = _pTypeInfoList;
    }
    return max_type_cnt;
}

bool LogConfig::init(const char *filename)
{
    LogType_Node *_default = &default_LogTypeList[0];
    JIMI_ASSERT_TRUE(_default == NULL);

    // Init Globals
    memset((void *)&global, 0, sizeof(global));

    // "jimi_global.log"
    strcpy_s(global.node.filename, _countof(global.node.filename), DEFAULT_LOG_FILENAME_GLOBAL);
    // "Global Log"
    strcpy_s(global.node.title, _countof(global.node.title), DEFAULT_LOG_TITLE_GLOBAL);
    // "[$datetime] [$type] %msg"
    strcpy_s(global.node.format, _countof(global.node.format), DEFAULT_LOG_FORMAT);

    // type_id
    global.node.type_id    = _default->type_id;
    memcpy(global.node.type_name, _default->type_name, sizeof(_default->type_name));

    global.node.enabled    = _default->enabled;
    global.node.to_file    = _default->to_file;
    global.node.to_screen  = _default->to_screen;
    global.node.to_trace   = _default->to_trace;
    global.node.to_remote  = _default->to_remote;
    global.node.create_new = true;

    global.node.max_log_file_size   = 0;
    global.node.log_flush_threshold = 0;

    // Globals remote
    global.type_mask = 0;
    global.max_type_cnt = get_type_info();

    strcpy_s(global.remote_ip, _countof(global.remote_ip), "");
    global.remote_port = 0;

    // Init Nodes
    int max_list_length = _countof(default_LogTypeList);
    typeList.clear();
    for (int i = 1; i < max_list_length; ++i) {
        LogConf_Node *node = new LogConf_Node();
        if (node != NULL) {
            memset((void *)node, 0, sizeof(LogConf_Node));
            if (copy_from_global(global, node, i))
                typeList.push_back(node);
        }
    }
    return open(filename);
}

bool LogConfig::copy_from_global(LogConf_Global &_global, LogConf_Node *_node, int _index)
{
    LogType_Node *_default = &default_LogTypeList[_index];
    JIMI_ASSERT_TRUE(_default == NULL);
    if (_node != NULL) {
        _node->filename[0] = '\0';
        _node->title[0]    = '\0';
        memcpy(_node->format, _global.node.format, sizeof(_global.node.format));
        _node->enabled    = _global.node.enabled   & _default->enabled;
        _node->to_file    = _global.node.to_file   & _default->to_file;
        _node->to_screen  = _global.node.to_screen & _default->to_screen;
        _node->to_trace   = _global.node.to_trace  & _default->to_trace;
        _node->to_remote  = _global.node.to_remote & _default->to_remote;
        _node->create_new = _global.node.create_new;
        _node->attrib     = _global.node.attrib;
        _node->max_log_file_size   = _global.node.max_log_file_size;
        _node->log_flush_threshold = _global.node.log_flush_threshold;
        _node->type_id    = _default->type_id;
        memcpy(_node->type_name, _default->type_name, sizeof(_default->type_name));
        return true;
    }
    return false;
}

bool LogConfig::open(const char *filename)
{
    bool ret_val = false;
    if (filename != NULL) {
        //
        ret_val = true;
    }
    return ret_val;
}

void LogConfig::close()
{
    //
}

///////////////////////////////////////////////////////////////////////////
// Log

Logger::Logger(const char *filename /*= NULL*/, const char *title /*= NULL*/, unsigned int attrib /*= 0*/)
: m_log_file(NULL)
, m_filename(NULL)
, m_title(NULL)
, m_format(NULL)
{
    this->open(filename, title);
}

Logger::Logger(Logger &src)
{
    strcpy(this->m_filename, src.m_filename);
}

Logger::~Logger()
{
    this->close();
}

Logger *Logger::getInstance(const char *filename /*= NULL*/, const char *title /*= NULL*/, unsigned int attrib /*= 0*/)
{
    if (s_Log == NULL) {
        char *s_filename;
        if (filename == NULL)
            s_filename = DEFAULT_LOG_FILENAME_GLOBAL;   // "jimi_global.log"
        else
            s_filename = (char *)filename;

        char *s_title;
        if (title == NULL)
            s_title = DEFAULT_LOG_TITLE_GLOBAL;         // "Global Log"
        else
            s_title = (char *)title;

        s_Log = new Logger(s_filename, s_title, attrib);
    }
    return s_Log;
}

void Logger::destroy()
{
    if (s_Log != NULL) {
        delete s_Log;
        s_Log = NULL;
    }
}

bool Logger::open(const char *filename, const char *title /*= NULL*/)
{
    FILE *log_file = this->open_file(filename);
    if (log_file != NULL) {
        m_log_file = log_file;
        m_filename = (char *)filename;
        if (title == NULL)
            m_title = DEFAULT_LOG_TITLE;        // "Noname Log"
        else
            m_title = (char *)title;
    }
    return (log_file != NULL && log_file != INVALID_HANDLE_VALUE);
}

FILE *Logger::open_file(const char *filename)
{
    FILE *log_file = NULL;

    // if log is open, close it first.
    this->close();

    if (filename != NULL) {
        int fd = 0;
        errno_t err;
        err = ::_sopen_s((int *)&fd, filename, _O_CREAT|_O_TRUNC|_O_BINARY|_O_RDWR, _SH_DENYNO, _S_IREAD|_S_IWRITE);
        if (err == 0)
            log_file = (FILE *)fd;
    }
    return log_file;
}

int Logger::close()
{
    int ret_val = -1;
    if (m_log_file != NULL && m_log_file != INVALID_HANDLE_VALUE) {
        ret_val = ::_close((int)m_log_file);
        if (ret_val == 0) {
            m_log_file = NULL;
            m_filename = NULL;
        }
    }
    return ret_val;
}

int Logger::flush()
{
    int ret_val = -1;
    if (m_log_file != NULL && m_log_file != INVALID_HANDLE_VALUE) {
        ret_val = ::fflush(m_log_file);
    }
    return ret_val;
}

void Logger::printf(const char *fmt, ...)
{
    va_list arglist;
    va_start(arglist, fmt);
    this->vprintf(false, fmt, arglist);
    va_end(arglist);
}

void Logger::_printf(bool newline, const char *fmt, ...)
{
    va_list arglist;
    va_start(arglist, fmt);
    this->vprintf(newline, fmt, arglist);
    va_end(arglist);
}

void Logger::vprintf(const char *fmt, va_list arglist)
{
    return this->vprintf(false, fmt, arglist);
}

void  Logger::vprintf(bool newline, const char *fmt, va_list arglist)
{
    vprintf(newline, NULL, NULL, fmt, arglist);
}

void Logger::vprintf(bool newline, const char *type_format, const char *type_name,
                  const char *fmt, va_list arglist)
{
#if defined(DO_JIMI_LOG) && (DO_JIMI_LOG != 0)
    int n;
    char msg_buf[MAX_LOG_TEXT_LEN];
    n = _vsnprintf_s_l(msg_buf, _countof(msg_buf), MAX_LOG_TEXT_LEN - 1, fmt, 0, arglist);
#if defined(JIMI_LOG_TO_SCREEN) && (JIMI_LOG_TO_SCREEN != 0)
    ::printf(msg_buf);
#endif
#if defined(JIMI_LOG_TO_FILE) && (JIMI_LOG_TO_FILE != 0)
    {
        if (type_format == NULL) {
            n = _vsnprintf_s_l(msg_buf, _countof(msg_buf), MAX_LOG_TEXT_LEN - 1, fmt, 0, arglist);
        }
        else {
            char fmt_buf[512];
            char *type_format_new = NULL;
            char *type_format_ = (char *)type_format;
            bool has_type = false;
            if (strstr(type_format_, "$datetime") != NULL) {
                char datetime[256];
                n = get_datetime_str(datetime, _countof(datetime));
                type_format_new = str_replace(type_format_, "$datetime", datetime);
                JIMI_ASSERT_TRUE(type_format_new == NULL);
                if (type_format_ != type_format)
                    free(type_format_);
                type_format_ = type_format_new;
            }
            if (strstr(type_format_, "$type") != NULL) {
                if (type_name != NULL) {
                    char type_fmt[64];
                    // "[% 7s]"
                    sprintf_s(type_fmt, _countof(type_fmt), "%% %ds", m_config.global.max_type_len);
                    type_format_new = str_replace(type_format_, "$type", type_fmt);
                    has_type = true;
                }
                else
                    type_format_new = str_replace(type_format_, "$type", "");
                JIMI_ASSERT_TRUE(type_format_new == NULL);
                if (type_format_ != type_format)
                    free(type_format_);
                type_format_ = type_format_new;

            }
            if (strstr(type_format_, "$msg") != NULL) {
                type_format_new = str_replace(type_format_, "$msg", "%s");
                JIMI_ASSERT_TRUE(type_format_new == NULL);
                if (type_format_ != type_format)
                    free(type_format_);
                type_format_ = type_format_new;
            }
            if (has_type)
                n = _snprintf_s_l(fmt_buf, _countof(fmt_buf), _countof(fmt_buf) - 1, type_format_, 0, type_name, fmt);
            else
                n = _snprintf_s_l(fmt_buf, _countof(fmt_buf), _countof(fmt_buf) - 1, type_format_, 0, fmt);
            n = _vsnprintf_s_l(msg_buf, _countof(msg_buf), MAX_LOG_TEXT_LEN - 1, fmt_buf, 0, arglist);
            if (type_format_ != type_format) {
                free(type_format_);
                type_format_ = NULL;
            }
            type_format_new = NULL;
        }
        if (newline)
            strcat_s(msg_buf, _countof(msg_buf), STRING_CRLF);
        if (m_log_file == NULL || m_log_file == INVALID_HANDLE_VALUE)
            this->open(DEFAULT_LOG_FILENAME);
        if (m_log_file)
            ::_write((int)m_log_file, msg_buf, strlen(msg_buf));
    }
#endif
#endif
}

void Logger::write(const char *fmt, ...)
{
    va_list arglist;
    va_start(arglist, fmt);
    this->outv(LOG_PRINT, false, fmt, arglist);
    va_end(arglist);
}

void Logger::writeln(const char *fmt, ...)
{
    va_list arglist;
    va_start(arglist, fmt);
    this->outv(LOG_PRINT, true, fmt, arglist);
    va_end(arglist);
}

void Logger::print(const char *fmt, ...)
{
    va_list arglist;
    va_start(arglist, fmt);
    this->outv(LOG_PRINT, false, fmt, arglist);
    va_end(arglist);
}

void Logger::println(const char *fmt, ...)
{
    va_list arglist;
    va_start(arglist, fmt);
    this->outv(LOG_PRINT, true, fmt, arglist);
    va_end(arglist);
}

void Logger::log(const char *fmt, ...)
{
    va_list arglist;
    va_start(arglist, fmt);
    this->outv(LOG_OUT, true, fmt, arglist);
    va_end(arglist);
}

void Logger::config(const char *fmt, ...)
{
    va_list arglist;
    va_start(arglist, fmt);
    this->outv(LOG_CONFIG, true, fmt, arglist);
    va_end(arglist);
}

void Logger::info(const char *fmt, ...)
{
    va_list arglist;
    va_start(arglist, fmt);
    this->outv(LOG_INFO, true, fmt, arglist);
    va_end(arglist);
}

void Logger::verbose(const char *fmt, ...)
{
    va_list arglist;
    va_start(arglist, fmt);
    this->outv(LOG_VERBOSE, true, fmt, arglist);
    va_end(arglist);
}

void Logger::trace(const char *fmt, ...)
{
    va_list arglist;
    va_start(arglist, fmt);
    this->outv(LOG_TRACE, true, fmt, arglist);
    va_end(arglist);
}

void Logger::debug(const char *fmt, ...)
{
    va_list arglist;
    va_start(arglist, fmt);
    this->outv(LOG_DEBUG, true, fmt, arglist);
    va_end(arglist);
}

void Logger::warn(const char *fmt, ...)
{
    va_list arglist;
    va_start(arglist, fmt);
    this->outv(LOG_WARN, true, fmt, arglist);
    va_end(arglist);
}

void Logger::error(const char *fmt, ...)
{
    va_list arglist;
    va_start(arglist, fmt);
    this->outv(LOG_ERROR, true, fmt, arglist);
    va_end(arglist);
}

void Logger::fatal(const char *fmt, ...)
{
    va_list arglist;
    va_start(arglist, fmt);
    this->outv(LOG_FATAL, true, fmt, arglist);
    va_end(arglist);
}

void Logger::out(unsigned int type, const char *fmt, ...)
{
    va_list arglist;
    va_start(arglist, fmt);
    this->outv(type, true, fmt, arglist);
    va_end(arglist);
}

/********************************************
    enum Log_Type
    {
        LOG_FIRST = 0,
        LOG_ALL,        // 全打开
        LOG_OFF,        // 全关闭
        LOG_WRITE,      // 直接输出
        LOG_OUT,        // 日志(Log)
        LOG_CONFIG,     // 配置
        LOG_INFO,       // 信息
        LOG_VERBOSE,    // 详细
        LOG_TRACE,      // 跟踪
        LOG_DEBUG,      // 调试
        LOG_WARN,       // 警告
        LOG_ERROR,      // 错误
        LOL_FATAL,      // 致命的
        LOG_LAST
    };
 ********************************************/

//#define LOG_TYPE_FORMAT           "(Type = %s) : "
#define LOG_TYPE_FORMAT             "[% 7s] "
//#define UNKNOWN_LOG_TYPE_FORMAT     "(Type = unknown flag (%d) :  "
#define UNKNOWN_LOG_TYPE_FORMAT     "[unknown flag=(%d)] "

void Logger::outv(unsigned int type, bool newline, const char *fmt, va_list arglist)
{
#if 1
    int index;
    JIMI_ASSERT_TRUE(m_config.typeInfoList == NULL);
    JIMI_ASSERT_FALSE(type >= m_config.global.min_type_id && type <= m_config.global.max_type_id);

    if (type == LOG_PRINT) {
        // 直接输出
        this->vprintf(newline, fmt, arglist);
    }
    else {
        if (m_config.typeInfoList != NULL
            && type >= m_config.global.min_type_id && type <= m_config.global.max_type_id) {
            index = m_config.typeInfoList[type].type_index;
            this->vprintf(true, (const char *)m_config.typeList[index]->format,
                (const char *)m_config.typeInfoList[type].type_name, fmt, arglist);
            return;
        }
        else {
            index = -1;
            return;
        }
    }
#else
    switch (type) {
        case LOG_OFF:
            // do nothing
            return;
        case LOG_PRINT:
            // 直接输出
            this->vprintf(newline, fmt, arglist);
            return;
        case LOG_OUT:
            // 日志(Log)
            this->_printf(false, LOG_TYPE_FORMAT, "log");
            break;
        case LOG_CONFIG:
            // 配置
            this->_printf(false, LOG_TYPE_FORMAT, "config");
            break;
        case LOG_INFO:
            // 信息
            this->_printf(false, LOG_TYPE_FORMAT, "info");
            break;
        case LOG_VERBOSE:
            // 详细
            this->_printf(false, LOG_TYPE_FORMAT, "verbose");
            break;
        case LOG_TRACE:
            // 跟踪
            this->_printf(false, LOG_TYPE_FORMAT, "trace");
            break;
        case LOG_DEBUG:
            // 调试
            this->_printf(false, LOG_TYPE_FORMAT, "debug");
            break;
        case LOG_WARN:
            // 警告
            this->_printf(false, LOG_TYPE_FORMAT, "warning");
            break;
        case LOG_ERROR:
            // 错误
            this->_printf(false, LOG_TYPE_FORMAT, "error");
            break;
        case LOL_FATAL:
            // 致命的
            this->_printf(false, LOG_TYPE_FORMAT, "fatal");
            break;
        default:
            // same to LOG_OUT
            this->_printf(false, UNKNOWN_LOG_TYPE_FORMAT, type);
            break;
    }
    this->vprintf(newline, fmt, arglist);
#endif
}

void Logger::logTitleLine(const char *title, int line_length)
{
    int i, n, r;
    size_t str_len = strlen(title);
    // printf("==             %s            ==\r\n", title);
    for (i=0; i<LOG_TITLE_EDGE_LENGTH; ++i)
        this->printf(LOG_TITLE_STR_REPEAT);
    if (str_len > 0) {
        n = (line_length - str_len - LOG_TITLE_MIN_LENGTH) / 2;
        for (i=0; i<n; ++i)
            this->printf(" ");
        this->printf("%s", title);
        r = line_length - n - str_len - (LOG_TITLE_MIN_LENGTH - 2);
        for (i=0; i<r; ++i)
            this->printf(" ");
    }
    else {
        n = line_length - (LOG_TITLE_MIN_LENGTH - 2);
        for (i=0; i<n; ++i)
            this->printf(" ");
    }
    for (i=0; i<LOG_TITLE_EDGE_LENGTH; ++i)
        this->printf(LOG_TITLE_STR_REPEAT);
    this->printf(STRING_CRLF);
}

void Logger::logTitleSection(const char *title, bool isBegin)
{
    char *log_title;
    if (title == NULL)
        log_title = m_title;
    else
        log_title = (char *)title;

    int i, n, r;
    size_t str_len = strlen(log_title);
    static const int title_begin_len    = strlen(LOG_TITLE_STR_BEGIN);
    static const int title_end_len      = strlen(LOG_TITLE_STR_END);
    static const int title_max_add_len  = JIMI_MAX(strlen(LOG_TITLE_STR_BEGIN), strlen(LOG_TITLE_STR_END));
    static const int title_str_repeat_len = strlen(LOG_TITLE_STR_REPEAT_SEC);
    int line_length = LOG_TITLE_LINE_LENGTH;

    if (line_length < (str_len + title_max_add_len + LOG_TITLE_MIN_LENGTH))
        line_length = str_len + title_max_add_len + LOG_TITLE_MIN_LENGTH;

    // printf("\r\n");
    this->printf(STRING_CRLF);

    // printf("===========================================\r\n");
    n = line_length / title_str_repeat_len;
    r = line_length % title_str_repeat_len;
    for (i=0; i<n; ++i)
        this->printf(LOG_TITLE_STR_REPEAT_SEC);
    for (i=0; i<r; ++i)
        this->printf(LOG_TITLE_STR_REPEAT);
    this->printf(STRING_CRLF);

    // printf("==                                   ==\r\n");
    logTitleLine("", line_length);

    // printf("==            %s Begin/End          ==\r\n", title);
    char *main_title = NULL;
    int main_title_len;
    if (isBegin) {
        // printf("==             %s Begin            ==\r\n", title);
        main_title_len = str_len + title_begin_len + 2;
        main_title = new char[main_title_len];
        if (main_title) {
            main_title[0] = '\0';
            n = sprintf_s(main_title, main_title_len, "%s %s", log_title, LOG_TITLE_STR_BEGIN);
            if (n >= 0)
                logTitleLine(main_title, line_length);
        }
    }
    else {
        // printf("==             %s End            ==\r\n", title);
        main_title_len = str_len + title_end_len + 2;
        main_title = new char[main_title_len];
        if (main_title) {
            main_title[0] = '\0';
            n = sprintf_s(main_title, main_title_len, "%s %s", log_title, LOG_TITLE_STR_END);
            if (n >= 0)
                logTitleLine(main_title, line_length);
        }
    }
    if (main_title) {
        delete[] main_title;
        main_title = NULL;
    }

    // printf("==                                   ==\r\n");
    logTitleLine("", line_length);

    // printf("==            TimeStamp: %s          ==\r\n", timestamp);
    int timestamp_len = 256;
    char *timestamp_str = new char[timestamp_len];
    if (timestamp_str) {
        timestamp_str[0] = '\0';
        char timestamp[256];
        n = get_datetime_str(timestamp, _countof(timestamp));
        if (n >= 0) {
            n = sprintf_s(timestamp_str, timestamp_len, "TimeStamp: %s", timestamp);
            if (n >= 0)
                logTitleLine(timestamp_str, line_length);
        }
    }
    if (timestamp_str) {
        delete[] timestamp_str;
        timestamp_str = NULL;
    }

    // printf("==                                   ==\r\n");
    logTitleLine("", line_length);

    // printf("===========================================\r\n");
    n = line_length / title_str_repeat_len;
    r = line_length % title_str_repeat_len;
    for (i=0; i<n; ++i)
        this->printf(LOG_TITLE_STR_REPEAT_SEC);
    for (i=0; i<r; ++i)
        this->printf(LOG_TITLE_STR_REPEAT);
    this->printf(STRING_CRLF);

    // printf("\r\n");
    this->printf(STRING_CRLF);
}

void Logger::log_begin(const char *title)
{
    logTitleSection(title, true);

    this->printf("\r\n");
    this->printf("/* Compiler definitions. */\r\n");
    for (int i = 0; i < sizeof(s_compilers) / sizeof(s_compilers[0]); ++i) {
        this->printf("#define %-16s\t%s\r\n", s_compilers[i].name, s_compilers[i].value);
    }
    this->printf("\r\n");
}

void Logger::log_end(const char *title)
{
    logTitleSection(title, false);
}

NS_JIMI_UTIL_END

NS_JIMI_END
