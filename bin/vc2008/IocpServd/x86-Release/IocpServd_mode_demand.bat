@echo.
@echo ���÷�����������: �Զ�-auto, ����-disabled, �ֶ�-demand (ע: start=��������һ���ո�!)
@echo.
sc config IocpServd start= demand
@echo.
@echo Stop Service...
@echo.
net stop IocpServd
pause
