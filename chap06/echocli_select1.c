#include "MyUNP.h"


int main(int argc, char* argv[])
{
	struct sockaddr_in svaddr;
	int sockfd;

	if (argc < 2)
		err_quit("usage: %s <ip-address>", argv[0]);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_sys("socket error");
	bzero(&svaddr, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(12000);
	if (inet_pton(AF_INET, argv[1], &svaddr.sin_addr) <= 0)
		err_sys("inet_pton convert string %s to address error", argv[1]);
	if (connect(sockfd, (struct sockaddr*)&svaddr, sizeof(svaddr)) == -1)
		err_sys("connect error");

	str_cli1(sockfd, stdin);
	if (close(sockfd) == -1)
		err_sys("close error");
	exit(EXIT_SUCCESS);
}


/*

str_cli1()函数存在的问题在于：
	1、标准I/O函数fgets()每一次都会从指定的流中读取标准I/O缓冲区大小的数据，
	但是总只会返回一行数据到用户进程的缓冲区中。而且读取受到select函数的控制，
	当STDIN_FILENO可用时才会有机会被调用，这就意味着fgets()函数实际只会得到
	大约（文件大小/缓冲区大小）的调用次数（这也正是客户进程向服务器发送的文本
	行数）
	2、无独有偶，对于我们自己的readline函数由于套接字描述符可用时，才会由select
	函数给机会调用一次，这就意味着客户进程回射回来的文本行数也受到readine函数的
	调用次数（不过由于服务进程每次总是会返回一行，因此回射回来的文本行数量大约
	就是客户进程发送的文本行数量。我只是推测，并没有试验）
	3、还有一个问题在于str_cli1()函数中的标准输入stdin。若stdin被重定向到了
	文件流身上，文本数据可能很快就得到输入，并让fgets遇到EOF文件结束标志，因此
	str_cli1不得不返回，并最终导致客户进程终止。
由上，我们可以知道str_cli1()函数对于重定向文本数据可能实际发送的数据和接收的文本行
远远小于实际我们重定向输入的文本行数据。因此我们的改进方向应该是：
	1、避免使用带缓冲的标准I/O，而是直接使用read/write系统调用，不过这样会导致
		因系统调用而造成的性能下降！
	2、当标准输入遇到文件结束标志EOF时进行半关闭，并同时保证不从str_cli函数中返回

*/
