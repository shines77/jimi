@echo.
@echo ���÷�����������: �Զ�-auto, ����-disabled, �ֶ�-demand (ע: start=��������һ���ո�!)
@echo.
sc config IocpServd start= auto
@echo.
@echo Start Service...
@echo.
net start IocpServd
pause
