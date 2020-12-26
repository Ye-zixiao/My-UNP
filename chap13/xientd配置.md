## Xinetd配置

为了实现让xinetd来创建指定的监听套接字并在客户请求到来时启动相应的服务器进程，我们必须在此之前配置好xinetd。xinetd最重要的配置文件为/etc/xinetd.conf，其次是文件夹/etc/xinetd.d中的相关文件。此次，我直接在/etc/xinetd.conf文件中进行编辑。

为了提供getdaytime时间获取服务（端口号：12000，绑定到通配地址），需要在/etc/xinetd.conf加入如下片段：

```shell
#前提是为12000在/etc/services中加入了相应字段
service myserver
{
	socket_type		= stream
	protocol 		= tcp
	wait			= no
	user			= root
	server			= 服务器程序的绝对路径
}
```

然后使用systemctl启动或者重启即可。

