@echo.
@echo ���÷�����������: �Զ�-auto, ����-disabled, �ֶ�-demand (ע: start=��������һ���ո�!)
@echo.
sc config IocpServd2 start= demand
@echo.
@echo Stop Service...
@echo.
net stop IocpServd2
pause
