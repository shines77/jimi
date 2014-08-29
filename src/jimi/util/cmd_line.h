
#ifndef _CMDLINE_PARSER_H_
#define _CMDLINE_PARSER_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdarg.h>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <algorithm>

#include <jimi/core/jimi_def.h>

using namespace std;

////////////////////////////////////////////////////////////////

#if _MSC_VER
    #define snprintf    _snprintf
    #define snprintf_s  _snprintf_s
#if _MSC_VER <= 1400
    #define vsnprintf   _vsnprintf
    #define vsnprintf_s _vsnprintf_s
#endif
#endif

#define CMDLINE_DEFAULT_TOKENS          "-/"
#define CMDLINE_DEFAULT_PARAM_NAME      "$default$"

////////////////////////////////////////////////////////////////

NS_JIMI_BEGIN

NS_JIMI_UTIL_BEGIN

typedef enum Param_Type
{
    PARAM_TYPE_UNKNOWN = 0,
    PARAM_TYPE_INTEGER,
    PARAM_TYPE_BOOL,
    PARAM_TYPE_STRING
} Param_Type_t;

//
// Case insensitive string comparison in C++
// From: http://stackoverflow.com/questions/11635/case-insensitive-string-comparison-in-c
//
struct ci_char_traits : public char_traits<char> {

    static bool eq(char c1, char c2) { return ::toupper(c1) == ::toupper(c2); }
    static bool ne(char c1, char c2) { return ::toupper(c1) != ::toupper(c2); }
    static bool lt(char c1, char c2) { return ::toupper(c1) <  ::toupper(c2); }

    static int compare(const char* s1, const char* s2, size_t n) {
        while (n-- != 0) {
            if (::toupper(*s1) < ::toupper(*s2)) return -1;
            if (::toupper(*s1) > ::toupper(*s2)) return 1;
            ++s1; ++s2;
        }
        return 0;
    }

    static const char* find(const char* s, int n, char a) {
        while (n-- > 0 && ::toupper(*s) != ::toupper(a)) {
            ++s;
        }
        return s;
    }
};

typedef std::basic_string<char, ci_char_traits> ci_string;

class JIMI_API cmd_param
{
public:
	std::string                 name;
    std::vector<std::string>    values;
	bool		                valueExpected;
	Param_Type	                type;

public:
	cmd_param()
	{
		// Initialize values
		name = "";
		//values.clear();
		valueExpected = true;
		type = PARAM_TYPE_UNKNOWN;
	}

	cmd_param(const std::string &_name)
	{
		// Initialize values
		name = _name;
		//values.clear();
		valueExpected = true;
		type = PARAM_TYPE_UNKNOWN;
	}

    ~cmd_param()
    {
        values.clear();
    }

    size_t sizeOfValues() {
        return values.size();
    }

    size_t addValue(std::string value)
    {
        values.push_back(value);
        return values.size();
    }

    int removeValue(int nIndex)
    {
        return 0;
    }

    //
    // Case insensitive string comparison in C++
    // From: http://stackoverflow.com/questions/11635/case-insensitive-string-comparison-in-c
    //
    inline bool i_strcmp(const std::string& str1, const std::string& str2) {
        if (str1.size() != str2.size())
            return false;

        for (std::string::const_iterator c1 = str1.begin(), c2 = str2.begin(); c1 != str1.end(); ++c1, ++c2) {
            if (*c1 != ::tolower(*c2))
                return false;
        }
        return true;
    }

    std::string findValue(std::string &value, bool caseSensitive = false)
    {
        return getValue(value, caseSensitive);
    }

    std::string getValue(std::string &value, bool caseSensitive = false)
    {
        std::string retval = "";
        std::vector<std::string>::iterator it;
        if (!caseSensitive) {
            std::string ivalue = value;
            std::transform(value.begin(), value.end(), ivalue.begin(), ::tolower);
            for (it = values.begin(); it != values.end(); ++it) {
                if (i_strcmp(ivalue, (*it)))
                    return (*it);
            }
        }
        else {
            for (it = values.begin(); it != values.end(); ++it) {
                if ((*it) == value)
                    return value;
            }
        }
        return retval;
    }
};

typedef std::pair<std::string, cmd_param *>  cmd_param_pair;
typedef std::map<std::string, cmd_param *>   cmd_param_map;
typedef std::pair<std::map<std::string, cmd_param *>::iterator, bool> cmd_param_pair_result;

class JIMI_API cmd_line
{
public:
    cmd_line();
    cmd_line(int _argc, char *_argv[]);
    virtual ~cmd_line();

private:
    explicit cmd_line(cmd_line &src);
    explicit cmd_line(int x);
    explicit cmd_line(unsigned x);

public:
    static cmd_line *s_cmd_line;
    static cmd_line &getInstance();

    int checkToken(int n, const char *_arg, std::string &opt);
    int setTokens(const std::string &_tokens) { tokens = _tokens; };
    int addToken(const char *_token);

    std::string get_cmdline();
    std::string getCmdLine();

    int parse(int _argc, char **_argv);

    bool        hasArgument(const std::string &strArgument);
    cmd_param  *getArgument(const std::string &strArgument);

public:
    std::string     tokens;
    std::string     cmd_exe;
    std::string     cmd_opts;
    int             argc;
    char            **argv;
    bool            caseSensitive;
    cmd_param_map   params;
};

typedef cmd_line CommandLine;

NS_JIMI_UTIL_END

NS_JIMI_END

#endif  /* _CMDLINE_PARSER_H_ */
