
#include "search_file.h"
/**
 * @brief 搜索文件
 * @param path 搜索的目录
 * @param L 文件路径保存到的链表
 * @param front 搜索文件的后缀
 * @param len 搜索文件后缀的长度
 */
void search_file(char *path, LinkList L, char *front, int len)
{
    struct dirent *message = NULL;
    DIR *dir_p = opendir(path);
    if (NULL == dir_p)
    {
        printf("打开目录%s失败.", path);
        return;
    }
    while (1)
    {
        message = readdir(dir_p);
        if (NULL == message)
        {
            perror("readdir");
            break;
        }
        if (message->d_type == DT_REG &&
            strcmp(front, message->d_name + (strlen(message->d_name) - len)) == 0)       // 指定搜索的文件
        {
            // +1 保存'\0', +1 保存'/'
            char *file_path = malloc(strlen(path) + strlen(message->d_name) + 1 + 1);
            if (path[strlen(path) - 1] == '/')
                sprintf(file_path, "%s%s", path, message->d_name); // 拼接文件路径
            else
                sprintf(file_path, "%s/%s", path, message->d_name); // 拼接文件路径
            printf("搜索到的文件的路径为：%s\n", file_path);
            TailInsert(L, file_path);      // 插入到链表
            free(file_path);
            
        }
        else if (message->d_type == DT_DIR && message->d_name[0] != '.')
        {
            char *dir_path = malloc(strlen(path) + strlen(message->d_name) + 1 + 1);
            if (path[strlen(path) - 1] == '/')
                sprintf(dir_path, "%s%s", path, message->d_name); // 拼接目录路径
            else
                sprintf(dir_path, "%s/%s", path, message->d_name); // 拼接目录路径
            search_file(dir_path, L, front, len);
            free(dir_path);
        }
    }
    closedir(dir_p);
}
