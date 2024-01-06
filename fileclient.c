#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 9191

#define MAX_PATH_LENGTH 256

void receiveFile(int clientSocket, const char *savePath)
{
    FILE *file = NULL; // 初始化文件指针
    char buffer[1024];
    size_t bytesRead;

    // 接收文件大小和文件名
    long fileSize;
    recv(clientSocket, &fileSize, sizeof(fileSize), 0);
    printf("文件大小: %ld\n", fileSize);

    // 接收文件名长度
    size_t filenameLength;
    ssize_t receivedBytes = recv(clientSocket, &filenameLength, sizeof(filenameLength), 0);
    if (receivedBytes != sizeof(filenameLength))
    {
        perror("Error receiving filename length");
        exit(1);
    }
    printf("文件名字长度: %zu\n", filenameLength);

    char filename[MAX_PATH_LENGTH];
    recv(clientSocket, filename, filenameLength, 0);
    filename[strlen(filename)] = '\0';
    printf("文件名字: %s\n", filename);

    // 构建保存文件的完整路径
    char filePath[MAX_PATH_LENGTH];

    snprintf(filePath, sizeof(filePath), "%s%s", savePath, filename);

    // 创建文件并准备接收文件内容
    file = fopen(filePath, "wb");
    if (file == NULL)
    {
        perror("Error opening file for writing");
        exit(1);
    }

    // 接收文件内容
    while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0)
    {
        if (strcmp(buffer, "zz^Y^zz") == 0)
        {
            printf("get EOF\n");
            break;
        }
        fwrite(buffer, 1, bytesRead, file);
    }

    fclose(file);

    printf("File received and saved as %s\n", filePath);
}

int main(int argc, char *argv[])
{
    char buffer[1024];
    FILE *file = NULL; // 初始化文件指针
    int clientSocket;
    const char *filePath;
    char order;

    // 创建套接字
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1)
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

    // 连接到服务器
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("Error connecting to server");
        exit(1);
    }

    printf("connect successful\n");

    while (1)
    {
        ssize_t receivedBytes;
        char buffer[1024] = {0}; // 初始化所有元素为0

        // 记得-1
        if ((receivedBytes = recv(clientSocket, &buffer, sizeof(buffer), 0)) == -1)
        {
            perror("recv");
        }
        printf("receivedBytes: %ld\n", receivedBytes);

        // buffer[receivedBytes] = '\0';

        printf("buffer[0]: %c\n", buffer[0]);

        if (strcmp(buffer, "1") == 0)
        {
            printf("hello\n");
        }
        else if (strcmp(buffer, "2") == 0)
        {
            printf("world\n");
        }
        else if (strcmp(buffer, "3") == 0)
        {
            receiveFile(clientSocket, "./recv/");
            printf("out of receiveFile\n");
        }
        else
            break;
    }

    // 关闭套接字和文件
    close(clientSocket);

    return 0;
}
