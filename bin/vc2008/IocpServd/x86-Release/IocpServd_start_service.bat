@echo.
@echo 设置服务启动类型: 自动-auto, 禁用-disabled, 手动-demand (注: start=后面必须跟一个空格!)
@echo.
sc config IocpServd start= auto
@echo.
@echo Start Service...
@echo.
net start IocpServd
pause
