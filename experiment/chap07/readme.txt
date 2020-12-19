shutdown_rd.pcapng：
	用来测试调用带常量SHUT_RD的shutdown函数之后，底层的TCP会发生什么。
	事实上，根据SHUT_RD的说明我们也可以知道：内核之中的TCP会确认到来的
	分节然后直接丢弃它们
linger.jpg、无TIME_WAIT方式close.png：
	演示了对开启了SO_LINGER选项的套接字执行close的结果