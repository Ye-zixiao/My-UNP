#### 程序结构：

```
webclient -------> main.c             web客户端主程序
              ---> home_page.c        向服务器发送一个GET请求，并读取服务器响应数据
              ---> write_get_cmd.c	  向服务器发送GET请求
              ---> do_get_read.c      向服务器发送TCP连接请求，并在连接建立之后发送GET请求
              						  ，然后读取发送回来的结果，最后通过条件变量通知主线程获
              						  取返回的状态信息
```

#### 执行流程：

<img src="web.jpg" alt="web" style="zoom: 50%;" />

我个人使用nginx服务器来作为测试使用的服务器程序