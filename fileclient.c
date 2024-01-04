#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 9191
char buffer[1024];
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
    char buffer[2048];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0)
    {
        if (send(clientSocket, buffer, bytesRead, 0) == -1)
        {
            perror("Error sending data to server");
            exit(1);
        }
    }
}

int main(int argc, char *argv[])
{

    FILE *file;
    int clientSocket;
    const char *filePath;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        exit(1);
    }

    filePath = argv[1];

    // 打开文件
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

    sendFile(clientSocket, file, filename);

    // 关闭套接字和文件
    fclose(file);
    close(clientSocket);

    return 0;
}
