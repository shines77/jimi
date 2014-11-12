
#include <jimi/util/cmd_line.h>

#include <string.h>

NS_JIMI_BEGIN

cmd_line *cmd_line::s_cmd_line = NULL;

cmd_line::cmd_line()
: argc(0)
, argv(NULL)
, tokens(CMDLINE_DEFAULT_TOKENS)    // "-/"
, cmd_exe("")
, cmd_opts("")
, caseSensitive(false)
, params()
{
    //
}

cmd_line::cmd_line(int _argc, char *_argv[])
: tokens(CMDLINE_DEFAULT_TOKENS)
, cmd_exe("")
, cmd_opts("")
, caseSensitive(false)
, params()
{

}

cmd_line::~cmd_line()
{
    cmd_param_map::iterator it;
    cmd_param *pParam;
    for (it = params.begin(); it != params.end(); ++it) {
        pParam = dynamic_cast<cmd_param *>(it->second);
        if (pParam)
            delete pParam;
        //delete (*it);
    }
    params.clear();
}

cmd_line &cmd_line::getInstance()
{
    if (s_cmd_line == NULL) {
        s_cmd_line = new cmd_line();
    }
    return *s_cmd_line;
}

std::string cmd_line::get_cmdline()
{
    std::string str_cmdline;
    str_cmdline = cmd_exe + " " + cmd_opts;
    return str_cmdline;
}

std::string cmd_line::getCmdLine()
{
    return get_cmdline();
}

bool cmd_line::hasArgument(const std::string &strArgument)
{
    cmd_param_map::iterator it;
    if (!caseSensitive) {
        std::string strArgumentL = strArgument;
        std::transform(strArgument.begin(), strArgument.end(), strArgumentL.begin(), ::tolower);
        it = params.find(strArgumentL);
    }
    else {
        it = params.find(strArgument);
    }

    return (it != params.end());
}

cmd_param *cmd_line::getArgument(const std::string &strArgument)
{
    cmd_param_map::iterator iter;
    if (!caseSensitive) {
        std::string strArgumentL = strArgument;
        std::transform(strArgument.begin(), strArgument.end(), strArgumentL.begin(), ::tolower);
        iter = params.find(strArgumentL);
    }
    else {
        iter = params.find(strArgument);
    }

    cmd_param *pParam = NULL;
    if (iter != params.end())
        pParam = dynamic_cast<cmd_param *>(iter->second);

    return pParam;
}

int cmd_line::parse(int _argc, char **_argv)
{
    JIMI_ASSERT_TRUE(_argc <= 0);
    JIMI_ASSERT_TRUE(_argv == NULL);
    if (_argc <= 0 || _argv == NULL)
        return -1;

    int param_count = 0;

    argc = _argc;
    argv = _argv;

    char *arg;
    std::string opt, _cmd_opt;
    cmd_param *paramLast = NULL;
    bool isFirstParam = true;
    _cmd_opt = "";
    for (int i = 0; i < _argc; ++i) {
        arg = _argv[i];
        if (i == 0) {
            cmd_exe = _argv[i];
            param_count++;
        }
        else {
            opt = "";
            if (checkToken(i, arg, opt) >= 0) {
                cmd_param *param = new cmd_param(opt);
                if (caseSensitive) {
                    params.insert(cmd_param_pair(opt, param));
                }
                else {
                    std::string opt_lower = opt;
                    std::transform(opt.begin(), opt.end(), opt_lower.begin(), ::tolower);
                    params.insert(cmd_param_pair(opt_lower, param));
                }
                paramLast = param;
                isFirstParam = false;
            }
            else if (paramLast != NULL) {
                paramLast->addValue(arg);
            }
            else if (isFirstParam && paramLast == NULL) {
                if (isFirstParam) {
                    cmd_param *param = new cmd_param(CMDLINE_DEFAULT_PARAM_NAME);   // "$default$"
                    params.insert(cmd_param_pair(CMDLINE_DEFAULT_PARAM_NAME, param));
                    paramLast = param;
                    isFirstParam = false;
                }
                if (paramLast != NULL)
                    paramLast->addValue(arg);
            }
            _cmd_opt += arg;
            _cmd_opt += " ";
            param_count++;
        }
    }
    cmd_opts = _cmd_opt;
    return param_count;
}

int cmd_line::checkToken(int n, const char *arg, std::string &opt)
{
    std::vector<std::string>::iterator it;
    size_t len = strlen(arg);
    size_t l_pos, r_pos;
    if (len == 0)
        return (-1);

    char *start, *end, *parg;
    parg  = (char *)arg;
    start = (char *)arg;
    end   = start + len;
    // Trim at the left
    while ((*parg) && (parg < end)) {
	    // Remove the left space
        if ((*parg) == ' ')
            parg++;
        else
            break;
    }
    l_pos = parg - start;
    if (l_pos >= len)
        l_pos = 0;

    // Trim at the right
    start = parg;
    parg = end;
    while (parg > start) {
	    // Remove the right space
        if ((*parg) == ' ')
            parg--;
        else
            break;
    }
    r_pos = parg - arg;
    if (r_pos < l_pos)
        r_pos = l_pos;
    if (r_pos < 0)
        r_pos = 0;

    // size_t lenTrim = r_pos - l_pos;
    parg = (char *)arg + l_pos;
    end  = (char *)parg + 1;

    int i;
    size_t lenTokens = tokens.size();
    if (lenTokens > 0) {
        for (; parg < end; ++parg) {
            for (i = 0; i < (int)lenTokens; ++i) {
                if (*parg == tokens[i]) {
                    l_pos = parg - arg;
                    size_t lenSub = r_pos - l_pos;
                    opt = arg;
                    if (lenSub < len)
                        opt.substr(l_pos, lenSub);
                    return (int)l_pos;
                }
            }
        }
    }
    return (-1);
}

NS_JIMI_END
