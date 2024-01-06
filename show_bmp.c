#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/mman.h>

int main()
{
    char bmpBuf[800 * 480 * 3] = {0};
    unsigned int lcdBuf[800 * 480] = {0};
    int red = 0x00ff0000;
    int purple = (53 << 16) | (72 << 8) | (204 << 0);

    int lcdFd = open("./dev/fb0", O_RDWR | O_CREAT, 0664);
    int bmpFd = open("./a.bmp", O_RDONLY);
    // read(bmpFd, bmpBuf, sizeof(bmpBuf));

    if (lcdFd < 0)
        printf("fail to open file\n");
    printf("success open file");

    // lseek(bmpFd, 54, SEEK_SET);
    read(bmpFd, bmpBuf, sizeof(bmpBuf));

    int *lcd_p = mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, lcdFd, 0);

    for (int i = 0; i < 800 * 480; i++)
        lcd_p[i] = bmpBuf[i * 3 + 0] | (bmpBuf[i * 3 + 1] << 8) | (bmpBuf[i * 3 + 2] << 16);

    close(lcdFd);
    close(bmpFd);
    munmap(lcd_p, 800 * 480 * 4);
    return 0;
}
