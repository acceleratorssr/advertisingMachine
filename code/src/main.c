#include "show_bmp.h"
#include "video.h"
#include "tcp_recv.h"
#include <pthread.h>

int  m_flag = 0; // 0:播放图片；1:播放视频
int  touch_x = 0, touch_y = 0;
void *play_medium(void *arg)
{
    while(1)
    {
        if (m_flag)
        {
            play_video();
        }
        else
        {
            show_bmp_loop();
        }
    }
}

void *get_xy_routine(void *arg)
{
    while(1)
    {
        get_xy(&touch_x, &touch_y);
    }
}


int main()
{
    system("killall -9 mplayer"); 
    pthread_t thread1;
    pthread_create(&thread1, NULL, client_recv, NULL);        // 创建线程接收网络数据
    pthread_t thread2;  
    pthread_create(&thread2, NULL, play_medium, NULL);        // 创建线程播放图片或者视频
    pthread_t thread3;  
    pthread_create(&thread3, NULL, get_xy_routine, NULL);     // 获取坐标
    while(1)
    {
        sleep(1);
    }
    return 0;
}
