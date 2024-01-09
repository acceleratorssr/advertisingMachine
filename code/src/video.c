#include "video.h"

int play_video()
{
    LinkList bmp_L = NULL;
    InitList(&bmp_L);                          // 初始化单链表
    search_file("./medium", bmp_L, ".avi", 4); // 搜索bmp图片所在路径，并保存到链表里
    char flag = 0;                             // 0暂停，1播放

    char video_path[10][100];
    ElemType e = {0};
    int i = 0;
    if (GetElem(bmp_L, ++i, &e))
    {
        strcpy(video_path[i], e);
    }

    

    int index = 0; // 数组下标
    char path[255] = {0};
    extern int m_flag;
    extern int touch_x, touch_y;
    while (m_flag == 1)
    {
        if (st == 2) // 上一条视频
        {
            st = 0;
            system("killall -9 mplayer"); // 杀死后台进程
            if (--index < 0)
            {
                index = sizeof(video_path) / sizeof(video_path[0]) - 1;
            }
            memset(path, 0, sizeof(path));
            sprintf(path, "./medium/mplayer -slave -quiet -geometry 0:0 -zoom -x 800 -y 480 %s -loop 0 &", video_path[index]); // &后台播放
            system(path);
            flag = 1;
            touch_x = -1;
            touch_y = -1;
        }
        else if (st == 3)
        {
            st = 0;
            system("killall -9 mplayer"); // 杀死后台进程
            if (++index > sizeof(video_path) / sizeof(video_path[0]) - 1)
            {
                index = 0;
            }
            memset(path, 0, sizeof(path));
            sprintf(path, "./medium/mplayer -slave -quiet -geometry 0:0 -zoom -x 800 -y 480 %s -loop 0 &", video_path[index]); // &后台播放
            system(path);
            flag = 1;
            touch_x = -1;
            touch_y = -1;
        }
        else if (st == 1) // 暂停、播放
        {
            st = 0;
            if (flag == 1)
            {
                system("killall -STOP mplayer"); // 暂停
                flag = 0;
            }
            else
            {
                system("killall -CONT mplayer"); // 继续播放
                flag = 1;
            }
            touch_x = -1;
            touch_y = -1;
        }
    }

    return 0;
}
