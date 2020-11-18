针对服务进程终止的情况下
	客户程序所在主机收到的RST分节交流的过程可以到wireshark文件夹中去看下
netstat -atnp
Active Internet connections (servers and established)
Proto Recv-Q Send-Q Local Address           Foreign Address         State       PID/Program name    
tcp        0      0 0.0.0.0:12000           0.0.0.0:*               LISTEN      16266/./echoserv3   
tcp        0      0 0.0.0.0:22              0.0.0.0:*               LISTEN      443/sshd: /usr/bin/ 
tcp        0      0 127.0.0.1:631           0.0.0.0:*               LISTEN      1033/cupsd          
tcp        1      0 192.168.137.215:42126   192.168.137.215:12000   CLOSE_WAIT  16756/./echocli1    
tcp        0      0 192.168.137.215:22      192.168.137.1:1557      ESTABLISHED 12459/sshd: yexinha 
tcp        0      0 192.168.137.215:22      192.168.137.1:11615     ESTABLISHED 1829/sshd: yexinhai 
tcp        0      0 192.168.137.215:12000   192.168.137.215:42126   FIN_WAIT2   -                   
tcp        0      0 192.168.137.215:22      192.168.137.1:1558      ESTABLISHED 12462/sshd: yexinha 
tcp6       0      0 :::1716                 :::*                    LISTEN      1223/kdeconnectd    
tcp6       0      0 :::22                   :::*                    LISTEN      443/sshd: /usr/bin/ 
