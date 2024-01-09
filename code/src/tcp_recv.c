#include "tcp_recv.h"
#include "search_file.h"
#include "singleLinkedList.h"

#define RECV_DATA_SIZE 1024 // 接收最大字节
#define SERVER_PORT 8888
#define SERVER_IP "192.168.160.243"

int st = 0;

/**
 * @brief 响应接收是否成功
 * @param socketfd
 * @param ack 1：接收数据成功  0接收数据失败
 */
void send_ack(int socketfd, char ack)
{
	char sendbuf[2] = {0};
	*(uint8_t *)sendbuf = 0xBB;
	sendbuf[1] = ack;
	send(socketfd, sendbuf, sizeof(sendbuf), 0); // 发送数据
}

/**
 * @brief 接收文件头
 * @param socketfd 套接字
 * @param filesize 获取文件的大小
 * @param filename 获取文件的名字
 * @return -1失败 0成功
 */
int recv_file_header(int socketfd, long *filesize, char *filename)
{
	char *recvbuf = malloc(RECV_DATA_SIZE);
	memset(recvbuf, 0, RECV_DATA_SIZE);
	recv(socketfd, recvbuf, RECV_DATA_SIZE, 0);

	if (*((uint8_t *)recvbuf) != 0xFF)
	{
		// printf("接收文件头出错\n");
		return -1;
	}
	*filesize = *(long *)(recvbuf + 1);			 // 文件大小
	short filenamelen = *(short *)(recvbuf + 9); // 文件名长度
	memcpy(filename, recvbuf + 11, filenamelen); // 文件名
	free(recvbuf);
	return 0;
}

/**
 * @brief 接收文件
 * @param socketfd
 */
void recv_file(int socketfd)
{
	long filesize = 0;
	char *filename = malloc(RECV_DATA_SIZE - 11 + 1);

	/*********接收文件头： 文件名 文件大小**********/
	if (recv_file_header(socketfd, &filesize, filename) == 0)
	{
		printf("文件名：%s, 大小：%ld\n", filename, filesize);
		send_ack(socketfd, 1);
	}
	else
	{
		printf("接收文件头出错\n");
		send_ack(socketfd, 0);
		free(filename);
		return;
	}

	char *filepath = malloc(RECV_DATA_SIZE - 11 + 1 + 2);
	sprintf(filepath, "./medium/%s", filename); // 需要用mkdir medium创建文件夹
	free(filename);

	FILE *fp = fopen(filepath, "wb+"); // 创建文件
	free(filepath);
	if (fp == NULL)
	{
		perror("open file failed...");
		return;
	}

	long head_recv = 0;
	short recvsize = 0;
	char *recvbuf = malloc(RECV_DATA_SIZE);
	while (filesize > head_recv)
	{
		memset(recvbuf, 0, RECV_DATA_SIZE);
		recv(socketfd, recvbuf, RECV_DATA_SIZE, 0);
		if (*((uint8_t *)recvbuf) != 0xAA)
		{
			printf("接收文件数据出错\n");
			send_ack(socketfd, 0);
			goto exit;
		}
		send_ack(socketfd, 1);
		recvsize = *(short *)(recvbuf + 1);
		printf("recvsize=%d\n", recvsize);
		fwrite(recvbuf + 3, recvsize, 1, fp); // 将接收到的数据写入到文件里
		head_recv += recvsize;
	}
exit:
	free(recvbuf);
	fclose(fp);
}

/**
 * @brief 接受命令
 * @param socketfd
 * @return 0：接受文件； 1：播放图片; 2:播放视频; -1 失败
 */
int recv_cmd(int socketfd)
{
	unsigned char cmd[2] = {0};
	while (1)
	{
		recv(socketfd, cmd, sizeof(cmd), 0);
		printf("cmd[0]=%x, cmd[1]=%d\n", cmd[0], cmd[1]);
		if (cmd[0] == 0xCC)
		{
			switch (cmd[1])
			{
			case 33:// 播放图片
				system("killall -9 mplayer"); // 杀死后台进程
				send_ack(socketfd, 1);
				return 1;
				break;
			case 22:// 播放视频
				send_ack(socketfd, 1);
				return 2;
				break;
			case 44:
				send_ack(socketfd, 1);
				return 0;
				break;
			case 55:// 暂停/播放视频
				send_ack(socketfd, 1);
				return 5;
				break;
			case 66:// pre视频
				send_ack(socketfd, 1);
				return 6;
				break;
			case 77:// next视频
				send_ack(socketfd, 1);
				return 7;
				break;
			case 88:
				send_ack(socketfd, 1);
				return 8;
				break;
			default:
				send_ack(socketfd, 0);
				return -1;
				break;
			}
		}
	}
}

// 客户接收
void *client_recv(void *arg)
{
	LinkList L;
	int ret = 0;
	struct sockaddr_in serveraddr = {0};
	// 服务器socket描述符
	int cfd = socket(AF_INET, SOCK_STREAM, 0);
	// 连接服务器地址和端口
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVER_PORT);					// 端口网络字节序
	inet_pton(AF_INET, SERVER_IP, &serveraddr.sin_addr.s_addr); // IP转网络字节序
	if (connect(cfd, (struct sockaddr *)&serveraddr,
				sizeof(struct sockaddr)) != 0)
	{
		perror("connect failed...");
		return NULL;
	}

	extern int m_flag;
	while (1)
	{
		ret = recv_cmd(cfd);
		switch (ret)
		{
		case 0:
			recv_file(cfd);
			st=99;
			break;
		case 1:
			m_flag = 0; // 播放图片
			break;
		case 2:
			m_flag = 1; // 播放视频
			break;
		case 5:
			st = 1; // 暂停/播放视频
			break;
		case 6:
			st = 2; // pre视频
			break;
		case 7:
			st = 3; // next视频
			break;
		case 8:
			InitList(&L);
			char filedir[1024] = "medium/";
			search_file(filedir,L,".bmp",4);
			search_file(filedir,L,".avi",4);
			LinkList cur = L;
			char allfile[1024];
			memset(allfile,0,1024);
			while(cur != NULL)
			{
				char tmp[1024];
				memset(tmp,0,1024);
				snprintf(tmp,1024,"%s",allfile);
				snprintf(allfile,1024,"%s %s",tmp,cur->data);
				cur = cur->next;
			}
			send(cfd,allfile,1024,0);
			char aimfile[1024];
			memset(aimfile,0,1024);
			recv(cfd,aimfile,1024,0);
			char command[1024];
			memset(command,0,1024);
			snprintf(command,1024,"rm -f %s",aimfile);
			system(command);
			break;
		default:
			printf("接收命令失败\n");
			break;
		}
	}
	close(cfd);
}