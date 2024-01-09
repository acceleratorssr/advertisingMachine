#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "singleLinkedList.h"

/**
 * @brief 初始化单链表
 *
 * @param L 链表
 * @return true
 * @return false
 */
bool InitList(LinkList *L)
{
   *L = (LinkList)malloc(sizeof(LNode));     // 在堆空间申请一个结构体大小的内存，作为头结点
   (*L)->data = NULL; // 清空数据域
   (*L)->next = NULL;
   return true;
}

/**
 * @brief 尾插法
 *
 * @param L    头结点
 * @param e     插入的数据
 * @return true
 * @return false
 */
bool TailInsert(LinkList L, ElemType e)
{
   if (L == NULL)
      return false;
   LinkList p = L;
   int size = strlen(e) + 1;     // 文件路径字节大小
   LinkList new_node = (LinkList)malloc(sizeof(LNode)); // 给新结点申请内存
   new_node->data = (ElemType)malloc(size);
   memset(new_node->data, 0, size);
   memcpy(new_node->data, e, size);
   new_node->next = NULL;

   while (p->next != NULL)
   {
      p = p->next;
   }
   p->next = new_node; // 尾插
   return true;
}

/**
 * @brief 删除任意一个位置的结点
 *
 * @param L 头结点
 * @param i 删除的位置(i范围：1~n)
 * @return true
 * @return false
 */
bool ListDelete(LinkList L, int i)
{
   int num = 0;      // 统计链表的结点个数
   LinkList p = L;
   LinkList delete = NULL;
   if (L == NULL)
      return false; 
   while(p != NULL && num < i - 1)     // 找到第i-1个结点
   {
      p = p->next;
      num++;
   }
   if (p == NULL || num + 1 != i)      // 没找到第i-1个结点
      return false;
    
   // 删除结点
   delete = p->next;
   p->next = delete->next;

   if (delete->data != NULL)
      free(delete->data);
   free(delete);
   return true;
}

/**
 * @brief Get the Elem object
 *      单链表的取值
 * @param L 头结点
 * @param i 取值的位置(i范围：1~n)
 * @param e 保存取值后的数据
 * @return true
 * @return false
 */
bool GetElem(LinkList L, int i, ElemType *e)
{
   int num = 1; // 统计链表的结点个数
   LinkList p = L->next;
   LinkList delete = NULL;
   if (L == NULL)
      return false;
   while (p != NULL && num < i) // 找到第i个结点
   {
      p = p->next;
      num++;
   }
   if (p == NULL || num != i) // 没找到第i个结点
      return false;
   *e = p->data;// 取值
   return true;
}


/**
 * @brief 遍历显示数据元素
 *
 * @param L
 */
void ListShow(LinkList L)
{
   if (L == NULL)
      return;
   LinkList p = L->next;
   while (p != NULL)
   {
      printf("%s\n", p->data);
      p = p->next;
   }
}

/**
 * @brief 单链表的销毁
 *
 * @param L 头结点
 */
void ListDestory(LinkList *L)
{
   LinkList next = NULL;
   LinkList p = *L;
   if (*L == NULL)
      return;
   while (p != NULL)
   {
      next = p->next;
      if (p->data != NULL)
         free(p->data);
      free(p);
      p = next;
   }
   *L = NULL;
}