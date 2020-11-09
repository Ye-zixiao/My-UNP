# My-UNP

## 1.配置原书相关文件

首先需要进入unpv13e文件夹配置文件：

```shell
$> ./configure
```

生成config.h头文件后编译生成原书程序所需要的静态链接库文件：

```shell
$> cd lib
$> make
$> cd ../libfree
$> make
$> #此时就会在unpv13e这个目录下生成一个名为libunp.a的静态链接库文件
```

为了能够在全局使用这些东西，需要将相关的文件移动到/usr/lib和/usr/include文件夹中

```shell
$> sudo cp libunp.a /usr/lib#可能还要复制到/usr/lib64中
$> sudo cp config.h /usr/include
$> sudo cp lib/unp.h /usr/include
$> #由于unp.h中有一句“#include "../config.h""来引用config.h头文件
$> #所以需要对unp.h中的../进行删除
```

完成上述操作就可以随意找个文件试一试，以官方的intro目录中的daytimetcp程序为例（需要注意官方给出的daytimetcp相关程序使用的是端口号13，这个并不应该给自定义程序使用，且不改动的话执行时会出错返回显示权限拒绝。所以下面都是我将其改成12000端口号的前提下操作的）

```shell
$> cd intro
$> gcc -o daytimetcpcli daytimetcpcli.c -O -Wall -lunp
$> gcc -o daytimetcpsrv daytimetcpsrv.c -O -Wall -lunp
$> ./daytimetcpsrv &
$> ./daytimetcpcli 127.0.0.1
Mon Nov  9 16:10:20 2020
```

