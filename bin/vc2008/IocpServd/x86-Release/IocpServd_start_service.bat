@echo.
@echo ���÷�����������: �Զ�-auto, ����-disabled, �ֶ�-demand (ע: start=��������һ���ո�!)
@echo.
sc config IocpServd2 start= auto
@echo.
@echo Start Service...
@echo.
net start IocpServd2
pause
