#### 程序结构：

```
webclient -------> main.c             web客户端主程序
              ---> home_page.c        向服务器发送一个GET请求，并读取服务器响应数据
              ---> start_connect.c    为获取其他的文件，向web服务器发起TCP连接，若直接建立则调用
                   					  write_get_cmd函数向服务器发送GET;否则让select监控，并在
                   					  有效时返回，再发送GET请求
              ---> write_get_cmd.c	  向服务器发送GET请求，并在发送完成后将相关的套接字描述符加入到
                   					  select的读感兴趣套接字描述符集中
```

我个人使用nginx服务器来作为测试使用的服务器程序
