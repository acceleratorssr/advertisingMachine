#include "touch.h"

int get_xy(int *x, int *y)
{
    struct input_event touchmsg = {0};

    // 用代码打开一个路径为/dev/input/event0触摸屏文件
    int fd = open("/dev/input/event0", O_RDWR | O_CREAT, 0664);
    if (fd < 0)
    {
        printf("打开文件失败\n");
    }
    
    while(1)
    {
        read(fd, &touchmsg, sizeof(touchmsg)); 
        if (touchmsg.type == EV_KEY && touchmsg.code == BTN_TOUCH && touchmsg.value == 0)
        {
            printf("手指松开了触摸屏\n");
            break;
        }
        if (touchmsg.type == EV_ABS && touchmsg.code == ABS_X)      // x坐标发生改变
        {
            // 黑色底板
            *x = touchmsg.value * 800 / 1024;

            // 蓝色底板
            // *x = touchmsg.value
        }
        if (touchmsg.type == EV_ABS && touchmsg.code == ABS_Y)      // y坐标发生改变
        {
            // 黑色底板
            *y = touchmsg.value * 480 / 600;

            // 蓝色底板
            //  *y = touchmsg.value
        }
        printf("(%d,%d)\n", *x, *y);
    }

    close(fd);
    return 0;
}