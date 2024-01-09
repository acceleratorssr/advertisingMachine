#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

#define RECV_DATA_SIZE 1024 // 接收最大字节
#define SERVER_PORT 8888
#define SERVER_IP "192.168.160.243"

// 获取文件的大小
long getfile_size(FILE *fd)
{
	long cur_position = ftell(fd); // 保存当前的文件偏移量
	fseek(fd, 0, SEEK_END);
	long filesize = ftell(fd);		   // 获取文件的大小
	fseek(fd, cur_position, SEEK_SET); // 恢复原来的文件偏移量
	return filesize;
}

/**
 * @brief 对方是否成功接收到数据
 * @param socketfd
 * @return -1：对方接收数据失败  0：对方成功收到数据
 */
int recv_ack(int socketfd)
{
	char recvbuf[2] = {0};
	recv(socketfd, recvbuf, sizeof(recvbuf), 0);
	if (*(uint8_t *)(recvbuf + 0) != 0xBB && recvbuf[1] != 1)
	{
		return -1;
	}
	return 0;
}

// 发送文件头
void send_file_header(int socketfd, char *file_name, long filesize)
{
	char *sendbuf = malloc(1 + 8 + 2 + strlen(file_name)); // 还没有处理规定一次可以发送最大RECV_DATA_SIZE字节
	memset(sendbuf, 0, 1 + 8 + 2 + strlen(file_name));
	*(uint8_t *)(sendbuf + 0) = 0xFF;						   // 标识
	*(long *)(sendbuf + 1) = filesize;						   // 文件大小
	*(short *)(sendbuf + 9) = strlen(file_name);			   // 文件名长度
	memcpy(sendbuf + 11, file_name, strlen(file_name));		   // 文件名
	send(socketfd, sendbuf, 1 + 8 + 2 + strlen(file_name), 0); // 发送数据
	free(sendbuf);
}

// 发送文件的函数
void send_file(int socketfd, char *path)
{
	char *ptr = strrchr(path, '/'); // 在参数path所指向的字符串中搜索最后一次出现字符'/'的位置,并返回
	if (ptr + 1 == NULL)
	{
		printf("获取文件名失败\n");
		return;
	}

	char *file_name = malloc(strlen(ptr + 1) + 1); // 变量用于保存发送文件的名字
	strcpy(file_name, ptr + 1);

	FILE *fp = fopen(path, "r"); // 打开文件
	if (fp == NULL)
	{
		perror("open file failed...");
		free(file_name);
		return;
	}
	long filesize = getfile_size(fp);				 // 文件大小
	send_file_header(socketfd, file_name, filesize); // 发送文件头：文件名，文件大小
	free(file_name);
	if (recv_ack(socketfd) != 0)
	{
		printf("发送文件名，文件大小失败\n");
		return;
	}

	// 发送文件数据
	size_t readsize = 0;
	long head_send = 0;
	char *sendbuf = malloc(RECV_DATA_SIZE);
	while (filesize > head_send)
	{
		memset(sendbuf, 0, RECV_DATA_SIZE);
		*(uint8_t *)(sendbuf + 0) = 0xAA; // 标识
		readsize = fread(sendbuf + 3, 1, RECV_DATA_SIZE - 3, fp);
		printf("readsize = %ld\n", readsize);

		*(short *)(sendbuf + 1) = readsize;		  // 发送一次数据的大小
		send(socketfd, sendbuf, readsize + 3, 0); // 发送数据
		if (recv_ack(socketfd) != 0)
		{
			printf("发送文件数据失败\n");
			goto exit;
		}
		head_send += readsize;
	}
exit:
	free(sendbuf);
	fclose(fp);
}

void main(void)
{
	unsigned char cmd[2] = {0}; // 保存要发送的命令包

	char path[1024] = {0}; // 保存发送文件的路径
	char client_ip[INET_ADDRSTRLEN] = {0};
	struct sockaddr_in serveraddr = {0};
	struct sockaddr_in clientaddr = {0}; // 保存连接的客户端的ip等信息
	socklen_t addrlen = sizeof(struct sockaddr_in);
	// 服务器socket描述符
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);

	// 端口复用
	int optval = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	// 绑定服务器地址和端口
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVER_PORT);					// 端口网络字节序
	inet_pton(AF_INET, SERVER_IP, &serveraddr.sin_addr.s_addr); // IP转网络字节序
	if (0 != bind(listenfd, (struct sockaddr *)&serveraddr,
				  sizeof(struct sockaddr)))
	{
		perror("server bind failed...");
		return;
	}

	// 请求连接上限
	listen(listenfd, 6);

	// 接受客户端的连接
	int clientfd = accept(listenfd, (struct sockaddr *)&clientaddr, &addrlen); // 阻塞等待连接
	if (-1 == clientfd)
	{
		perror("server accept failed...");
		return;
	}

	while (1)
	{
		printf("请输入要发送的命令：");
		scanf("%hhd", &cmd[1]);
		while (getchar() != '\n')
			;
		// 33放图片, 22放视频, 55暂停/播放视频, 66 pre, 77 next
		if (cmd[1] == 33 || cmd[1] == 22 || cmd[1] == 44 || cmd[1] == 55 || cmd[1] == 66 || cmd[1] == 77  || cmd[1] == 88)
		{
			cmd[0] = 0xCC;
			send(clientfd, cmd, sizeof(cmd), 0); // 发送命令数据
			if (recv_ack(clientfd) == 0)
			{
				printf("已成功发送命令\n");
				if (cmd[1] == 44)
				{
					printf("请输入发送文件的路径：");
					scanf("%s", path);
					// 发送文件
					send_file(clientfd, path);
				}
				else if(cmd[1] == 88)
				{
					char buffer[1024];
					memset(buffer,0,sizeof(buffer));
					recv(clientfd,buffer,1024,0);
					printf("%s\n",buffer);
					char path[2048],filedir[1024] = "medium/";
					memset(path,0,sizeof(path));
					char filename[1024];
					memset(filename,0,sizeof(filename));
					printf("输入要删除的文件：");
					scanf("%s",filename);
					snprintf(path,2048,"%s%s",filedir,filename);
					send(clientfd,path,sizeof(path),0);
					printf("删除成功！");
				}
			}
			else
			{
				continue;
			}
		}
		else
		{
			printf("输入命令错误\n");
		}
	}

	close(clientfd);
	close(listenfd);
}