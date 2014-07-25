
#include <jimic/system/crtdbg_env.h>

/**
  ����CRTDBG�Ļ���(Debugģʽ��, ����ڴ�Խ����ڴ�й©����):

  always_check_bounds ��0��ʾÿ�η���/�ͷ��ڴ�ʱ, ϵͳ���Զ�����_CrtCheckMemory(), ����ڴ�Խ�����
  use_vld ��0��ʾ, ʹ��vld����ڴ�й¶������, ����ʹ��CrtDbg���ڴ�й¶���
*/
void jimi_set_crtdbg_env(int display_memory_leak, int always_check_bounds)
{
    /* ʹ��CRTDBG�������ڴ�Խ������, �����ʹ����vld, �ڴ�й©��Ϣ�ɹر� */
    int dbg_flag = 0;

#if defined(JIMI_USE_CRTDBG_CHECK) && (JIMI_USE_CRTDBG_CHECK != 0)

#if defined(_MSC_VER)

#if defined(_DEBUG) || !defined(NDEBUG)

    // ���� CRT ����ģʽ
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);

    dbg_flag |= _CRTDBG_ALLOC_MEM_DF;

    if (display_memory_leak != 0) {
        // ����Ѿ�����vld.h, ����ʾcrtdbg���ڴ�й©��Ϣ
#ifndef VLD_RPTHOOK_INSTALL
        // �����˳�ʱ, ��ʾ�ڴ�й©��Ϣ
        dbg_flag |= _CRTDBG_LEAK_CHECK_DF;
#endif  /* VLD_RPTHOOK_INSTALL */
    }

    if (always_check_bounds != 0) {
        // _CRTDBG_CHECK_ALWAYS_DF ��ʾÿ�η���/�ͷ��ڴ�ʱ, ϵͳ���Զ�����_CrtCheckMemory(), ����ڴ�Խ�����
        dbg_flag |= _CRTDBG_CHECK_ALWAYS_DF;
    }

    // ����CrtDbgFlag
    if (dbg_flag != 0)
        _CrtSetDbgFlag(dbg_flag);

#endif  /* _DEBUG */

#endif  /* _MSC_VER */

#endif  /* JIMI_USE_CRTDBG_CHECK */
}
