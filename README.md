# My-UNP

### 1. 简介

这个仓库是我根据Richard Steven先生的《UNIX网络编程 卷一：套接字联网API》时写的一些代码和笔记。我当时看完APUE最大的感受就是：自己写程序相关的头文件和常用实现代码，而不是直接使用原书网站给出的头文件、链接库可能对自己学习上的帮助更大。所以这个仓库中的代码完全都是我重新造轮子复现了一遍，其中在编程的过程中也发现了一些书上以及unpbook网站给出代码上的一些错误。总之这个过程还是蛮有意思的，我个人也觉得收获也是不少的。

由于自己看着本书的目的现阶段仅仅是为了学习TCP/UDP的编程，而书上的内容并不是全部需要学习的，所以我仅仅推荐了如下几个章节学习：

1. [第一章：简介](chap01)
2. [第二章：传输层：TCP、UDP和SCTP](chap02)
3. [第三章：套接字编程简介](chap03)
4. [第四章：基本TCP套接字编程](chap04)
5. [第五章：TCP客户/服务器程序示例](chap05)
6. [第六章：select和poll函数](chap06)（不过还得找书学epoll）
7. [第七章：套接字选项](chap07)
8. [第八章：基本UDP套接字编程](chap08)
9. [第十一章：名字与地址转换](chap11)
10. 第十三章：守护进程和inetd超级服务器（这部分内容我没看）
11. [第十四章：高级I/O 函数](chap14)
12. 第十五章：Unix域协议（我没看与APUE重复）
13. [第十六章：非阻塞式I/O](chap16)
14. [第二十六章：线程](chap26)_
15. [第三十章：客户/服务器程序设计范式](chap30)

反正基本上就选择TCP/UDP、一些常用的内容，什么SCTP、IP层的东西目前基本用不上的初学者就不看了哈！



### 2. 实验平台

```shell
██████████████████  ████████   xxxxxxxx@Localhost 
██████████████████  ████████   ------------------ 
██████████████████  ████████   OS: Manjaro Linux x86_64 
██████████████████  ████████   Host: HP ENVY Notebook 
████████            ████████   Kernel: 5.8.18-1-MANJARO 
████████  ████████  ████████   Uptime: 6 hours, 22 mins 
████████  ████████  ████████   Packages: 1345 (pacman), 6 (snap) 
████████  ████████  ████████   Shell: zsh 5.8 
████████  ████████  ████████   Resolution: 1920x1080, 2560x1440 
████████  ████████  ████████   DE: Plasma 5.20.4 
████████  ████████  ████████   WM: KWin 
████████  ████████  ████████   WM Theme: McMojave-light 
████████  ████████  ████████   Theme: McMojaveLight [Plasma], Breath [GTK2/3] 
████████  ████████  ████████   Icons: McMojave-circle-blue [Plasma], McMojave-circle-blue [GTK2/3] 
                               Terminal: konsole 
                               CPU: Intel i5-6200U (4) @ 2.800GHz 
                               GPU: Intel Skylake GT2 [HD Graphics 520] 
                               GPU: NVIDIA GeForce GTX 950M 
                               Memory: 1649MiB / 15911MiB 

```





### 3. 开始使用

- 仓库中的最重要的头文件MyUNP.h放置在include这个文件夹中，这个文件夹还放了一些其他的头文件，包括array_stack.h这些自己写的基础组件头文件
- 而上述头文件中指出的函数实现文件我都防止在lib这个文件夹中，在编译的时候这个文件夹下会自动生成一个动态链接库文件，其他chap各章节下的源文件编译之后只需要与之链接就可以了

说了这么多，其实很简单：若想要编译链接chap30中的C代码，其实只需要键下`make`就ok了！



### 4. 参考资料

1. UNIX网络编程 卷一：套接字联网API

