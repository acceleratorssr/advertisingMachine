#ifndef __SINGLELIST__
#define __SINGLELIST__
#include <stdbool.h>

typedef char * ElemType;
typedef struct node
{
   ElemType data;     // 数据域
   struct node *next; // 指针域
} LNode, *LinkList;

bool InitList(LinkList *L);
bool TailInsert(LinkList L, ElemType e);
bool ListDelete(LinkList L, int i);
bool GetElem(LinkList L, int i, ElemType *e);
void ListShow(LinkList L);
void ListDestory(LinkList *L);

#endif
