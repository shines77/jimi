
#include <jimic/system/sys_timer.h>

jmc_timestamp jmc_get_timestamp()
{
    return jmc_get_now();
}

jmc_timestamp jmc_get_millisec()
{
    return jmc_get_now_ms();
}

jmc_timestamp jmc_now_time()
{
    return jmc_get_now_ms();
}

jmc_timefloat jmc_now_timef(void)
{
    return jmc_get_nowf();
}

