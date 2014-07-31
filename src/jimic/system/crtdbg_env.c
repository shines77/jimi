
#include <jimic/system/crtdbg_env.h>

/**
  设置CRTDBG的环境(Debug模式下, 检查内存越界和内存泄漏问题):

  always_check_bounds 非0表示每次分配/释放内存时, 系统会自动调用_CrtCheckMemory(), 检查内存越界情况
  use_vld 非0表示, 使用vld检查内存泄露的问题, 而不使用CrtDbg的内存泄露检测
*/
void jimi_set_crtdbg_env(int display_memory_leak, int always_check_bounds)
{
    /* 使用CRTDBG将会检查内存越界问题, 如果你使用了vld, 内存泄漏信息可关闭 */
    int dbg_flag = 0;

#if defined(JIMI_USE_CRTDBG_CHECK) && (JIMI_USE_CRTDBG_CHECK != 0)

#if defined(_MSC_VER)

#if defined(_DEBUG) || !defined(NDEBUG)

    // 设置 CRT 报告模式
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);

    dbg_flag |= _CRTDBG_ALLOC_MEM_DF;

    if (display_memory_leak != 0) {
        // 如果已经引用vld.h, 则不显示crtdbg的内存泄漏信息
#ifndef VLD_RPTHOOK_INSTALL
        // 进程退出时, 显示内存泄漏信息
        dbg_flag |= _CRTDBG_LEAK_CHECK_DF;
#endif  /* VLD_RPTHOOK_INSTALL */
    }

    if (always_check_bounds != 0) {
        // _CRTDBG_CHECK_ALWAYS_DF 表示每次分配/释放内存时, 系统会自动调用_CrtCheckMemory(), 检查内存越界情况
        dbg_flag |= _CRTDBG_CHECK_ALWAYS_DF;
    }

    // 设置CrtDbgFlag
    if (dbg_flag != 0)
        _CrtSetDbgFlag(dbg_flag);

#endif  /* _DEBUG */

#endif  /* _MSC_VER */

#endif  /* JIMI_USE_CRTDBG_CHECK */
}
