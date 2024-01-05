#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define PORT 9191
#define MAX_PATH_LENGTH 256

void receiveFile(int clientSocket, const char *savePath)
{
    FILE *file;
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
        fwrite(buffer, 1, bytesRead, file);
    }

    fclose(file);

    printf("File received and saved as %s\n", filePath);
}

int main()
{
    int serverSocket, clientSocket;
    char buffer[1024];

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

    receiveFile(clientSocket, "./recv/");

    // 关闭套接字
    close(clientSocket);
    close(serverSocket);

    return 0;
}
