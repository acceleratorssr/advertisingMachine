#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define PORT 9191

void sendFile(int clientSocket, FILE *file, const char *filename)
{
    // 获取文件大小
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // 发送文件大小和文件名
    send(clientSocket, &fileSize, sizeof(fileSize), 0);
    size_t filenameLength = strlen(filename); // strlen不包含\0
    filenameLength++;

    send(clientSocket, &filenameLength, sizeof(filenameLength), 0);

    send(clientSocket, filename, filenameLength, 0);

    // 发送文件内容
    char buffer[1024];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0)
    {
        if (send(clientSocket, buffer, bytesRead, 0) == -1)
        {
            perror("Error sending data to server");
            exit(1);
            // break;
        }
    }

    printf("sended EOF\n");

    // 确保此时客户端将接收的数据已经写入文件后，再发送停止符
    // 否则客户端无法正常接收到停止符
    // 大概至少：
    usleep(10000);

    if (send(clientSocket, "zz^Y^zz", sizeof("zz^Y^zz"), 0) == -1)
    {
        perror("Error sending end of file marker to server");
        exit(1);
    }

    fclose(file);
}

int main(int agrc, char *argv[])
{
    int serverSocket, clientSocket;
    int order = 0;
    char buffer[1024];
    FILE *file;
    char filePath[64] = "../send/";

    // 创建套接字
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        perror("Error creating socket");
        exit(1);
    }

    // 设置服务器地址
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // 设置SO_REUSEADDR选项
    int reuseAddr = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr)) == -1)
    {
        perror("Error setting SO_REUSEADDR option");
        exit(1);
    }

    // 绑定套接字到地址
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("Error binding socket");
        exit(1);
    }

    // 监听连接
    if (listen(serverSocket, 5) == -1)
    {
        perror("Error listening for connections");
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    // 接受连接
    clientSocket = accept(serverSocket, NULL, NULL);
    if (clientSocket == -1)
    {
        perror("Error accepting connection");
        exit(1);
    }

    while (order != 0x99)
    {
        printf("0x99 == quit\n");
        scanf("%x", &order);
        // getchar();
        memset(buffer, 0, sizeof(buffer));
        if (order != 0x01 && order != 0x02 && order != 0x03 && order != 0x04 && order !=0x05)
            break;
        snprintf(buffer, sizeof(buffer), "%x", order);

        // printf("%c\n", buffer[0]);

        if (send(clientSocket, &buffer, sizeof(buffer), 0) == -1)
        {
            perror("send");
        }
        printf("send over\n");

        if (recv(clientSocket, &buffer, sizeof(buffer), 0) > 0)
        {
            int i = 0;
            while (buffer[i] != '\0')
            {
                printf("%c", buffer[i]);
                i++;
            }
            printf("\n");
        }
        else
            perror("last recv err\n");

        if (order == 0x03)
        {
            // 打开文件
            char f[32];
            scanf("%s", f);
            strcat(filePath, f);
            file = fopen(filePath, "rb");
            if (file == NULL)
            {
                perror("Error opening file");
                exit(1);
            }

            const char *filename = strrchr(filePath, '/'); // 获取文件名
            if (filename == NULL)
                filename = filePath; // 如果没有斜杠，整个路径就是文件名
            else
                filename++; // 移动到文件名的起始位置

            // 打开文件
            file = fopen(filePath, "rb");
            if (file == NULL)
            {
                perror("Error opening file");
                exit(1);
            }

            sendFile(clientSocket, file, filename);
        }
    }

    // 关闭套接字
    close(clientSocket);
    close(serverSocket);

    return 0;
}
