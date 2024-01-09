#include "show_bmp.h"
#include "search_file.h"

#define LCD_WITH 800
#define LCD_HIGH 480

// 解决图片镜像翻转的问题
/**
 * char *bmp_path显示图片的路径
 */
int show_bmp(char *bmp_path)
{
   char bmpbuf[LCD_WITH * LCD_HIGH * 3] = {0};
   unsigned int temp[LCD_WITH] = {0};              // 临时保存一行的数据
   unsigned int lcdbuf[LCD_WITH * LCD_HIGH] = {0}; // 保存lcd数据
   int bmpfd = open(bmp_path, O_RDONLY);           // 打开24位bmp图片
   if (bmpfd < 0)
   {
      printf("打开%s失败, 失败原因%s\n", bmp_path, strerror(errno));
      return -1;
   }
   int lcdfd = open("/dev/fb0", O_RDWR); // 打开lcd
   if (lcdfd < 0)
   {
      printf("打开%s失败, 失败原因%s\n", "/dev/fb0", strerror(errno));
      return -1;
   }
   unsigned int *lcd_p = mmap(NULL, LCD_WITH * LCD_HIGH * 4,
                              PROT_WRITE | PROT_READ, MAP_SHARED, lcdfd, 0); // 内存映射
   if (lcd_p == MAP_FAILED)
   {
      perror("内存映射失败\n");
      return -1;
   }
   lseek(bmpfd, 54, SEEK_SET);          // 偏移图片的54个字节其他数据
   read(bmpfd, bmpbuf, sizeof(bmpbuf)); // 将LCD_WITH * LCD_HIGH的图片数据读出来

   for (int i = 0; i < LCD_WITH * LCD_HIGH; i++)
   {
      lcdbuf[i] = bmpbuf[i * 3 + 0] | (bmpbuf[i * 3 + 1] << 8) | (bmpbuf[i * 3 + 2] << 16);
   }

   // 解决图片镜像翻转的问题
   for (int j = 0; j < LCD_HIGH / 2; j++)
   {
      memset(temp, 0, sizeof(temp));
      memcpy(temp, &lcdbuf[j * LCD_WITH], sizeof(temp));                          // 临时保存前面的一行的数据
      memcpy(&lcdbuf[j * LCD_WITH], &lcdbuf[(479 - j) * LCD_WITH], LCD_WITH * 4); // 后面的替换到前面
      memcpy(&lcdbuf[(479 - j) * LCD_WITH], temp, sizeof(temp));
   }
   // for (int i = 0; i < 800 * 480; i++)
   // {
   //    *(lcd_p + i) = lcdbuf[i];
   // }
   memcpy(lcd_p, lcdbuf, sizeof(lcdbuf));
   munmap(lcd_p, LCD_WITH * LCD_HIGH * 4); // 解除映射

   close(lcdfd);
   close(bmpfd);
   return 0;
}

// 循环显示图片
int show_bmp_loop()
{
   // extern int st;
   LinkList bmp_L = NULL;
   int i = 0;
   InitList(&bmp_L);                          // 初始化单链表
   search_file("./medium", bmp_L, ".bmp", 4); // 搜索bmp图片所在路径，并保存到链表里
   // 遍历单链表打印文件路径
   ElemType e = {0};
   extern int m_flag;
   while (m_flag == 0)
   {
      if (st == 99)
      {
         InitList(&bmp_L);
         search_file("./medium", bmp_L, ".bmp", 4);
      }

      if (GetElem(bmp_L, ++i, &e)) // 从链表取数据：bmp图片路径
      {
         printf("%s\n", e);
         // 显示图片
         show_bmp(e);
         sleep(3); // 延时1秒#include <unistd.h>
      }
      else
      {
         i = 0; // 循环显示图片
         continue;
         ;
      }
   }

   // 销毁单链表
   ListDestory(&bmp_L);
   return 0;
   return 0;
}