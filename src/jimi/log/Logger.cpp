
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include <share.h>

#include <jimi/log/Logger.h>
#include <jimic/string/jm_strings.h>

#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#include <windows.h>

NS_JIMI_BEGIN

NS_JIMI_LOG_BEGIN

_Utils  utils;
_System System;

Logger *Logger::s_Log = NULL;

static char *s_log_filename = NULL;
static char *s_log_title    = NULL;

/* Compilers */
const JIMI_MACRO_T s_compilers[] = {
    { "[Compiler]", "" },

#ifdef _MSC_VER         /* Visual C++ */
    JIMI_MAKE_STRING(_MSC_VER),
#endif

#ifdef _DLL             /* _DLL */
    JIMI_MAKE_STRING(_DLL),
#endif

#ifdef _LIB             /* _LIB */
    JIMI_MAKE_STRING(_LIB),
#endif

#ifdef JIMI_USE_STATIC    /* JIMI_USE_STATIC */
    JIMI_MAKE_STRING(JIMI_USE_STATIC),
#endif

#ifdef JIMI_LOG_USE_STATIC    /* JIMI_LOG_USE_STATIC */
    JIMI_MAKE_STRING(JIMI_LOG_USE_STATIC),
#endif
    
#ifdef __FILE__    /* __FILE__ */
    JIMI_MAKE_STRING(__FILE__),
#endif
};

LogType_Node default_LogTypeList[] = {
    /* tag_id       tag_name   enabled  file   screen   trace  remote */
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
    src_len = ::jm_strlen(src_str);
    while (::jm_strstr(dup_buf, src_str) != NULL) {
        dup_buf = ::jm_strstr(dup_buf, src_str) + src_len;
        i++;
        if (!repeat)
            break;
    }

    if (i != 0) {
        dup_buf = (char *)str_buf;
        str_len = ::jm_strlen(str_buf);
        dst_len = ::jm_strlen(dst_str);
        len = (str_len - (src_len - dst_len) * i + 1);
        new_str = (char *)::malloc(len * sizeof(char));
        if (new_str != NULL) {
            new_str[0] = '\0';
            if (len > 1)
                new_str[len - 1] = '\0';
            //memset(new_str, 0, len * sizeof(char));
            while ((next = ::jm_strstr(dup_buf, src_str)) != NULL) {
                new_str = ::jm_strncat(new_str, len, dup_buf, (next - dup_buf));
                //new_str = ::jm_strncat(new_str, len, dst_str);
                new_str = ::jm_strncat(new_str, len, dst_str, dst_len);
                dup_buf = next + src_len;
                if (!repeat)
                    break;
            }
            new_str = ::jm_strcat(new_str, len, dup_buf);
            return new_str;
        }
    }
    else {
        str_len = ::jm_strlen(str_buf);
        len = (str_len + 1);
        new_str = (char *)::malloc(len * sizeof(char));
        if (new_str != NULL) {
            new_str[0] = '\0';
            //memset(new_str, 0, len * sizeof(char));
            new_str = ::jm_strcpy(new_str, len, str_buf);
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
        n = jm_snprintf(datetime, bufsize, bufsize - 1, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
            sysTime.wYear,
            sysTime.wMonth,
            sysTime.wDay,
            sysTime.wHour,
            sysTime.wMinute,
            sysTime.wSecond,
            sysTime.wMilliseconds);
    }
    else {
        n = jm_snprintf(datetime, bufsize, bufsize - 1, "%04d-%02d-%02d %02d:%02d:%02d",
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
: infoList(NULL)
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

    if (infoList != NULL) {
        delete[] infoList;
        infoList = NULL;
    }

    LogConf_NodeVecIter iter;
    for (iter = typeList.begin(); iter != typeList.end(); ++iter) {
        delete (*iter);
    }

    typeList.clear();
}

int LogConfig::get_max_tag_len()
{
    int max_tag_len = 0;
    int max_list_length = jm_countof(default_LogTypeList);
    for (int i = 1; i < max_list_length; ++i) {
        int len = jm_strlen(default_LogTypeList[i].tag_name);
        if (len > max_tag_len) {
            max_tag_len = len;
        }
    }
    return max_tag_len;
}

int LogConfig::get_tags_info()
{
    int i;
    int min_tag_id = INT_MAX;
    int max_tag_id = 0;
    int max_tag_len = 0;
    int max_list_length = jm_countof(default_LogTypeList);
    for (i = 0; i < max_list_length; ++i) {
        int tag_id = default_LogTypeList[i].tag_id;
        if (tag_id < min_tag_id)
            min_tag_id = tag_id;
        if (tag_id > max_tag_id)
            max_tag_id = tag_id;
        int len = jm_strlen(default_LogTypeList[i].tag_name);
        if (len > max_tag_len) {
            max_tag_len = len;
        }
    }

    global.min_tag_id  = min_tag_id;
    global.max_tag_id  = max_tag_id;
    global.max_tag_len = max_tag_len;

    int max_tag_cnt = 0;
    LogType_NodeInfo *_pTypeInfoList = new LogType_NodeInfo[max_tag_id + 1];
    if (_pTypeInfoList != NULL) {
        for (i = 0; i < max_list_length; ++i) {
            int tag_id = default_LogTypeList[i].tag_id;
            _pTypeInfoList[tag_id].tag_id = tag_id;
            _pTypeInfoList[tag_id].tag_index = i - 1;
            jm_strcpy(_pTypeInfoList[tag_id].tag_name, jm_countof(_pTypeInfoList[tag_id].tag_name),
                default_LogTypeList[i].tag_name);
            max_tag_cnt++;
        }
        infoList = _pTypeInfoList;
    }
    global.max_tag_cnt = max_tag_cnt;
    return max_tag_cnt;
}

bool LogConfig::init(const char *filename)
{
    LogType_Node *_default = &default_LogTypeList[0];
    JIMI_LOG_ASSERT_TRUE(_default == NULL);

    // Init Globals
    memset((void *)&global, 0, sizeof(global));

    // "iocp_global.log"
    jm_strcpy(global.node.filename, jm_countof(global.node.filename), DEFAULT_LOG_FILENAME_GLOBAL);
    // "Global Log"
    jm_strcpy(global.node.title, jm_countof(global.node.title), DEFAULT_LOG_TITLE_GLOBAL);
    // "[$datetime] [$tag] $msg"
    jm_strcpy(global.node.format, jm_countof(global.node.format), DEFAULT_LOG_FORMAT);

    // tag_id
    global.node.tag_id    = _default->tag_id;
    memcpy(global.node.tag_name, _default->tag_name, sizeof(_default->tag_name));

    global.node.enabled    = _default->enabled;
    global.node.to_file    = _default->to_file;
    global.node.to_screen  = _default->to_screen;
    global.node.to_trace   = _default->to_trace;
    global.node.to_remote  = _default->to_remote;
    global.node.create_new = true;

    global.node.max_log_file_size   = 0;
    global.node.log_flush_threshold = 0;

    // Globals remote
    global.tag_mask = 0;
    // Get tags info
    get_tags_info();
    global.max_tag_len = 0;

    jm_strcpy(global.remote_ip, jm_countof(global.remote_ip), "");
    global.remote_port = 0;

    // Init Nodes
    int max_list_length = jm_countof(default_LogTypeList);
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
    JIMI_LOG_ASSERT_TRUE(_default == NULL);
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
        _node->tag_id    = _default->tag_id;
        memcpy(_node->tag_name, _default->tag_name, sizeof(_default->tag_name));
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
//, m_filename(NULL)
//, m_title(NULL)
//, m_format(NULL)
{
    //memset(m_format, 0, sizeof(m_format));
    jm_strcpy(m_format, jm_countof(m_format), DEFAULT_LOG_FORMAT);

    if (!this->open(filename, title)) {
        memset(m_filename, 0, sizeof(m_filename));
        memset(m_title, 0, sizeof(m_title));
        //jm_strcpy(m_title, jm_countof(m_title), DEFAULT_LOG_TITLE);
    }
}

Logger::Logger(Logger &src)
{
    jm_strcpy(this->m_filename, jm_countof(this->m_filename), src.m_filename);
}

Logger::~Logger()
{
    this->close();
}

Logger *Logger::getInstance(const char *filename /*= NULL*/, const char *title /*= NULL*/, unsigned int attrib /*= 0*/)
{
    if (s_Log == NULL) {
        if (s_log_filename == NULL)
            s_log_filename = (char *)malloc(MAX_LOG_PATH * sizeof(char));
        if (filename == NULL) {
            //s_log_filename = DEFAULT_LOG_FILENAME_GLOBAL;   // "iocp_global.log"
            jm_strcpy(s_log_filename, MAX_LOG_PATH, DEFAULT_LOG_FILENAME_GLOBAL);
        }
        else {
            //s_log_filename = (char *)filename;
            jm_strcpy(s_log_filename, MAX_LOG_PATH, filename);
        }

        if (s_log_title == NULL)
            s_log_title = (char *)malloc(MAX_LOG_TITLE * sizeof(char));
        if (title == NULL) {
            //s_log_title = DEFAULT_LOG_TITLE_GLOBAL;         // "Global Log"
            jm_strcpy(s_log_title, MAX_LOG_TITLE, DEFAULT_LOG_TITLE_GLOBAL);
        }
        else {
            //s_log_title = (char *)title;
            jm_strcpy(s_log_title, MAX_LOG_TITLE, title);
        }

        s_Log = new Logger(s_log_filename, s_log_title, attrib);
    }
    return s_Log;
}

void Logger::destroy()
{
    if (s_Log != NULL) {
        delete s_Log;
        s_Log = NULL;
    }
    if (s_log_title != NULL) {
        free(s_log_title);
        s_log_title = NULL;
    }
    if (s_log_filename != NULL) {
        free(s_log_filename);
        s_log_filename = NULL;
    }
}

size_t Logger::get_app_path(char *app_path, size_t buf_len)
{
    if (app_path == NULL || buf_len <= 0)
        return 0;

    app_path[0] = '\0';
    size_t ret_len = GetModuleFileNameA(NULL, app_path, buf_len);
    if (ret_len <= 0) {
        // Can't get the app path
        return 0;
    }

    int token_cnt = 0;
    size_t len = jm_strlen(app_path);
    char *start = app_path;
    char *end = app_path + len;
    char *cur = end - 1;
    while ((cur != NULL) && (cur >= start)) {
        // find the last pos of the sign '\' or '/'
        if ((*cur) == '\\' || (*cur) == '/') {
            if (cur < end)
                *(cur + 1) = '\0';
            token_cnt++;
            break;
        }
        cur--;
    }

    // check app path's tail, last char must be '\' or '/'
    len = jm_strlen(app_path);
    if (len > 0 && (app_path[len - 1] != '\\' && app_path[len - 1] != '/')) {
        app_path[len - 1] = '\\';
        len++;
    }

    return len;
}

size_t Logger::make_app_filename(const char *filename, char **new_filename)
{
    JIMI_LOG_ASSERT_TRUE(filename == NULL);
    if (filename == NULL) {
        if (new_filename != NULL)
            *new_filename = NULL;
        return 0;
    }

    size_t name_len = jm_strlen(filename);
    size_t buf_len = MAX_LOG_PATH + name_len + 2;
    size_t len = 0;
    char *app_path = (char *)malloc(buf_len * sizeof(char));
    if (app_path != NULL) {
        size_t path_len = get_app_path(app_path, buf_len);
        if (path_len > 0) {
            jm_strcat(app_path, buf_len, filename);
            if (new_filename != NULL)
                *new_filename = app_path;
            len = jm_strlen(app_path);
        }
    }
    return len;
}

size_t Logger::get_app_filename(const char *filename, char *new_filename, size_t buf_len)
{
    size_t len = 0;
    if (new_filename != NULL) {
        size_t path_len = get_app_path(new_filename, buf_len);
        if (path_len > 0) {
            if (filename != NULL)
                jm_strcat(new_filename, buf_len, filename);
            len = jm_strlen(new_filename);
        }
        else {
            new_filename[0] = '\0';
        }
    }
    return len;
}

size_t Logger::get_log_filename(const char *filename, char **new_filename)
{
    size_t len = 0;
    if (filename == NULL) {
        if (new_filename != NULL)
            *new_filename = NULL;
        return len;
    }

    bool isRootPath = false;
    char asc2, asc3, asc4;
    char *tmp_name = (char *)filename;
    char asc1 = *tmp_name;
    if (asc1 == '\\' || asc1 == '/') {
        // is from root path (windows or linux)
        isRootPath = true;
    }
    else if ((asc1 >= 'A' && asc1 <= 'Z') || (asc1 >= 'a' && asc1 <= 'z')) {
        tmp_name++;
        asc2 = *tmp_name;
        if (asc2 == ':') {
            tmp_name++;
            asc3 = *tmp_name;
            tmp_name++;
            asc4 = *tmp_name;
            if ((asc3 == '\\' && asc4 == '\\') || (asc3 == '/' && asc4 == '/')) {
                // is from root path (windows)
                isRootPath = true;
            }
        }
    }

    if (!isRootPath) {
        size_t name_len = jm_strlen(filename);
        size_t buf_len = MAX_LOG_PATH + name_len + 2;
        char *app_filename = (char *)malloc(buf_len * sizeof(char));
        if (get_app_filename(filename, app_filename, buf_len) <= 0) {
            if (new_filename != NULL)
                *new_filename = NULL;

            if (app_filename != NULL)
                free(app_filename);
        }
        else {
            if (new_filename != NULL)
                *new_filename = app_filename;

            len = jm_strlen(app_filename);
        }
    }
    else {
        if (new_filename != NULL)
            *new_filename = (char *)filename;

        len = jm_strlen(filename);
    }
    return len;
}

bool Logger::open(const char *filename, const char *title /*= NULL*/)
{
    char *new_filename = NULL;
    get_log_filename(filename, &new_filename);

    FILE *log_file = this->open_file(new_filename);
    if (log_file != NULL) {
        m_log_file = log_file;
        //m_filename = (char *)new_filename;
        jm_strcpy(m_filename, jm_countof(m_filename), new_filename);
        if (title == NULL) {
            //m_title = DEFAULT_LOG_TITLE;        // "Noname Log"
            jm_strcpy(m_title, jm_countof(m_title), DEFAULT_LOG_TITLE);
        }
        else {
            //m_title = (char *)title;
            jm_strcpy(m_title, jm_countof(m_title), title);
        }
    }
    if (new_filename != filename && new_filename != NULL) {
        free(new_filename);
        new_filename = NULL;
    }
    return (log_file != NULL && log_file != INVALID_HANDLE_VALUE);
}

FILE *Logger::open_file(const char *filename)
{
    FILE *log_file = NULL;
    if (filename != NULL) {
        // if log is open, close it first.
        this->close();

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
            m_filename[0] = '\0';
            m_title[0] = '\0';
            m_format[0] = '\0';
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

void Logger::vprintf(bool newline, const char *fmt, va_list arglist)
{
    return this->vprintf(newline, NULL, NULL, 0, fmt, arglist);
}

void Logger::vprintf(bool newline, const char *tag_format, const char *tag_name,
                     size_t tag_len, const char *fmt, va_list arglist)
{
#if defined(DO_JIMI_LOG) && (DO_JIMI_LOG != 0)
    int n;
    char msg_buf[MAX_LOG_TEXT_LEN];    
    n = jm_vsnprintf(msg_buf, jm_countof(msg_buf), MAX_LOG_TEXT_LEN - 1, fmt, arglist);

#if defined(JIMI_LOG_TO_SCREEN) && (JIMI_LOG_TO_SCREEN != 0)
    ::printf(msg_buf);
    if (newline)
        ::printf(STRING_CRLF);
#endif  /* JIMI_LOG_TO_SCREEN */

#if defined(JIMI_LOG_TO_FILE) && (JIMI_LOG_TO_FILE != 0)
    {
        if (tag_format == NULL) {
            n = jm_vsnprintf(msg_buf, jm_countof(msg_buf), MAX_LOG_TEXT_LEN - 1, fmt, arglist);
        }
        else {
            char fmt_buf[512];
            char *tag_format_new = NULL;
            char *tag_format_ = (char *)tag_format;
            bool has_tag = false;
            int tag_case = 0;
            if (::jm_strstr(tag_format_, "$datetime") != NULL) {
                char datetime[256];
                n = get_datetime_str(datetime, jm_countof(datetime));
                tag_format_new = str_replace(tag_format_, "$datetime", datetime);
                JIMI_LOG_ASSERT_TRUE(tag_format_new == NULL);
                if (tag_format_ != tag_format)
                    free(tag_format_);
                tag_format_ = tag_format_new;
            }
            if (::jm_strstr(tag_format_, "$type") != NULL) {
                if (tag_name != NULL) {
                    char tag_fmt[64];
                    // "[% 7s]" or "[%-7s]"
                    jm_sprintf(tag_fmt, jm_countof(tag_fmt), "%% %ds", m_config.global.max_tag_len);
                    tag_format_new = str_replace(tag_format_, "$type", tag_fmt);
                    has_tag = true;
                    tag_case = 1;
                }
                else
                    tag_format_new = str_replace(tag_format_, "$type", "");
                JIMI_LOG_ASSERT_TRUE(tag_format_new == NULL);
                if (tag_format_ != tag_format)
                    free(tag_format_);
                tag_format_ = tag_format_new;
            }
            else if (::jm_strstr(tag_format_, "$TYPE") != NULL) {
                if (tag_name != NULL) {
                    char tag_fmt[64];
                    // "[% 7s]" or "[%-7s]"
                    jm_sprintf(tag_fmt, jm_countof(tag_fmt), "%% %ds", m_config.global.max_tag_len);
                    tag_format_new = str_replace(tag_format_, "$TYPE", tag_fmt);
                    has_tag = true;
                    tag_case = 2;
                }
                else {
                    tag_format_new = str_replace(tag_format_, "$TYPE", "");
                }
                JIMI_LOG_ASSERT_TRUE(tag_format_new == NULL);
                if (tag_format_ != tag_format)
                    free(tag_format_);
                tag_format_ = tag_format_new;
            }
            else if (::jm_strstr(tag_format_, "$Type") != NULL) {
                if (tag_name != NULL) {
                    char tag_fmt[64];
                    // "[% 7s]" or "[%-7s]"
                    jm_sprintf(tag_fmt, jm_countof(tag_fmt), "%% %ds", m_config.global.max_tag_len);
                    tag_format_new = str_replace(tag_format_, "$Type", tag_fmt);
                    has_tag = true;
                    tag_case = 0;
                }
                else {
                    tag_format_new = str_replace(tag_format_, "$Type", "");
                }
                JIMI_LOG_ASSERT_TRUE(tag_format_new == NULL);
                if (tag_format_ != tag_format)
                    free(tag_format_);
                tag_format_ = tag_format_new;
            }
            if (::jm_strstr(tag_format_, "$msg") != NULL) {
                tag_format_new = str_replace(tag_format_, "$msg", "%s");
                JIMI_LOG_ASSERT_TRUE(tag_format_new == NULL);
                if (tag_format_ != tag_format)
                    free(tag_format_);
                tag_format_ = tag_format_new;
            }
            if (has_tag) {
                if (tag_case == 2)
                    n = jm_snprintf(fmt_buf, jm_countof(fmt_buf), jm_countof(fmt_buf) - 1, tag_format_, jm_strupr((char *)tag_name, tag_len), fmt);
                else if (tag_case == 1)
                    n = jm_snprintf(fmt_buf, jm_countof(fmt_buf), jm_countof(fmt_buf) - 1, tag_format_, jm_strlwr((char *)tag_name, tag_len), fmt);
                else
                    n = jm_snprintf(fmt_buf, jm_countof(fmt_buf), jm_countof(fmt_buf) - 1, tag_format_, tag_name, fmt);
            }
            else
                n = jm_snprintf(fmt_buf, jm_countof(fmt_buf), jm_countof(fmt_buf) - 1, tag_format_, fmt);
            n = jm_vsnprintf(msg_buf, jm_countof(msg_buf), MAX_LOG_TEXT_LEN - 1, fmt_buf, arglist);
            if (tag_format_ != tag_format) {
                free(tag_format_);
                tag_format_ = NULL;
            }
            tag_format_new = NULL;
        }
        if (newline)
            jm_strcat(msg_buf, jm_countof(msg_buf), STRING_CRLF);
        if (m_log_file == NULL || m_log_file == INVALID_HANDLE_VALUE)
            this->open(DEFAULT_LOG_FILENAME);
        if (m_log_file)
            ::_write((int)m_log_file, msg_buf, jm_strlen(msg_buf));
    }
#endif  /* JIMI_LOG_TO_FILE */
#endif  /* DO_JIMI_LOG */
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

void Logger::out(int tag, const char *fmt, ...)
{
    va_list arglist;
    va_start(arglist, fmt);
    this->outv(tag, true, fmt, arglist);
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

void Logger::outv(int tag, bool newline, const char *fmt, va_list arglist)
{
#if 1
    int index;
    JIMI_LOG_ASSERT_TRUE(m_config.infoList == NULL);
    JIMI_LOG_ASSERT_FALSE(tag >= m_config.global.min_tag_id && tag <= m_config.global.max_tag_id);

    if (tag == LOG_PRINT) {
        // 直接输出
        this->vprintf(newline, fmt, arglist);
    }
    else {
        if (m_config.infoList != NULL
            && tag >= m_config.global.min_tag_id && tag <= m_config.global.max_tag_id) {
            index = m_config.infoList[tag].tag_index;
            this->vprintf(true, (const char *)m_config.typeList[index]->format,
                (const char *)m_config.infoList[tag].tag_name,
                jm_countof(m_config.infoList[tag].tag_name),
                fmt, arglist);
            return;
        }
        else {
            index = -1;
            return;
        }
    }
#else
    switch (tag) {
        case LOG_OFF:
            // do nothing
            return;
        case LOG_PRINT:
            // 直接输出
            this->vprintf(newline, fmt, arglist);
            return;
        case LOG_OUT:
            // 日志(Log)
            this->_printf(false, LOG_TAG_FORMAT, "log");
            break;
        case LOG_CONFIG:
            // 配置
            this->_printf(false, LOG_TAG_FORMAT, "config");
            break;
        case LOG_INFO:
            // 信息
            this->_printf(false, LOG_TAG_FORMAT, "info");
            break;
        case LOG_VERBOSE:
            // 详细
            this->_printf(false, LOG_TAG_FORMAT, "verbose");
            break;
        case LOG_TRACE:
            // 跟踪
            this->_printf(false, LOG_TAG_FORMAT, "trace");
            break;
        case LOG_DEBUG:
            // 调试
            this->_printf(false, LOG_TAG_FORMAT, "debug");
            break;
        case LOG_WARN:
            // 警告
            this->_printf(false, LOG_TAG_FORMAT, "warning");
            break;
        case LOG_ERROR:
            // 错误
            this->_printf(false, LOG_TAG_FORMAT, "error");
            break;
        case LOL_FATAL:
            // 致命的
            this->_printf(false, LOG_TAG_FORMAT, "fatal");
            break;
        default:
            // same to LOG_OUT
            this->_printf(false, UNKNOWN_LOG_TAG_FORMAT, tag);
            break;
    }
    this->vprintf(newline, fmt, arglist);
#endif
}

void Logger::log_title_line(const char *title, int line_length)
{
    int i, n, r;
    size_t str_len = jm_strlen(title);
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

void Logger::log_title_section(const char *title, bool isBegin)
{
    char *log_title;
    if (title == NULL)
        log_title = m_title;
    else
        log_title = (char *)title;

    // 先调用这个函数, 当是全局log变量时, 这将会保证先打开默认log文件,
    // 否则后面将会出错(因为全局log文件并未指定文件名, 只是第一次输出的时候才为其设置一个默认文件名)
    // printf("\r\n");
    this->printf(STRING_CRLF);

    int i, n, r;
    size_t str_len = jm_strlen(log_title);
    static const int title_begin_len    = jm_strlen(LOG_TITLE_STR_BEGIN);
    static const int title_end_len      = jm_strlen(LOG_TITLE_STR_END);
    static const int title_max_add_len  = JIMI_LOG_MAX(jm_strlen(LOG_TITLE_STR_BEGIN), jm_strlen(LOG_TITLE_STR_END));
    static const int title_str_repeat_len = jm_strlen(LOG_TITLE_STR_REPEAT_SEC);
    int line_length = LOG_TITLE_LINE_LENGTH;

    if (line_length < ((int)str_len + title_max_add_len + LOG_TITLE_MIN_LENGTH))
        line_length = str_len + title_max_add_len + LOG_TITLE_MIN_LENGTH;

    // printf("===========================================\r\n");
    n = line_length / title_str_repeat_len;
    r = line_length % title_str_repeat_len;
    for (i=0; i<n; ++i)
        this->printf(LOG_TITLE_STR_REPEAT_SEC);
    for (i=0; i<r; ++i)
        this->printf(LOG_TITLE_STR_REPEAT);
    this->printf(STRING_CRLF);

    // printf("==                                   ==\r\n");
    log_title_line("", line_length);

    // printf("==            %s Begin/End          ==\r\n", title);
    char *main_title = NULL;
    int main_title_len;
    if (isBegin) {
        // printf("==             %s Begin            ==\r\n", title);
        main_title_len = str_len + title_begin_len + 4;
        main_title = new char[main_title_len];
        if (main_title) {
            main_title[0] = '\0';
            n = jm_sprintf(main_title, main_title_len, "%s %s", log_title, LOG_TITLE_STR_BEGIN);
            if (n >= 0)
                log_title_line(main_title, line_length);
        }
    }
    else {
        // printf("==             %s End            ==\r\n", title);
        main_title_len = str_len + title_end_len + 4;
        main_title = new char[main_title_len];
        if (main_title) {
            main_title[0] = '\0';
            n = jm_sprintf(main_title, main_title_len, "%s %s", log_title, LOG_TITLE_STR_END);
            if (n >= 0)
                log_title_line(main_title, line_length);
        }
    }
    if (main_title) {
        delete[] main_title;
        main_title = NULL;
    }

    // printf("==                                   ==\r\n");
    log_title_line("", line_length);

    // printf("==            TimeStamp: %s          ==\r\n", timestamp);
    int timestamp_len = 256;
    char *timestamp_str = new char[timestamp_len];
    if (timestamp_str) {
        timestamp_str[0] = '\0';
        char timestamp[256];
        n = get_datetime_str(timestamp, jm_countof(timestamp));
        if (n >= 0) {
            n = jm_sprintf(timestamp_str, timestamp_len, "TimeStamp: %s", timestamp);
            if (n >= 0)
                log_title_line(timestamp_str, line_length);
        }
    }
    if (timestamp_str) {
        delete[] timestamp_str;
        timestamp_str = NULL;
    }

    // printf("==                                   ==\r\n");
    log_title_line("", line_length);

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
    log_title_section(title, true);

    this->printf("\r\n");
    this->printf("/* Compiler definitions. */\r\n");
    for (int i = 0; i < sizeof(s_compilers) / sizeof(s_compilers[0]); ++i) {
        this->printf("#define %-16s\t%s\r\n", s_compilers[i].name, s_compilers[i].value);
    }
    this->printf("\r\n");
}

void Logger::log_end(const char *title)
{
    log_title_section(title, false);
}

NS_JIMI_LOG_END

NS_JIMI_END
