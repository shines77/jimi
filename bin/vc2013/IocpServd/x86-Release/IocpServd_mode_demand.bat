@echo.
@echo 设置服务启动类型: 自动-auto, 禁用-disabled, 手动-demand (注: start=后面必须跟一个空格!)
@echo.
sc config IocpServd2 start= demand
@echo.
@echo Stop Service...
@echo.
net stop IocpServd2
pause
