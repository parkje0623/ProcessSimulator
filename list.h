#ifndef _LIST_H
#define _LIST_H
typedef struct node {
	void * data;
	struct node * next;
	struct node * prev;
} NODE;

typedef struct list{
	NODE * head;
	NODE * tail;
	NODE * current;
} LIST;
int PrintList(LIST *list);
LIST * ListCreate();
int ListCount(LIST * list);
void *ListFirst(LIST * list);
void *ListLast(LIST * list);
void *ListNext(LIST * list);
void *ListPrev(LIST * list);
void *ListCurr(LIST * list);
int ListAdd(LIST * list, void * item);
int ListInsert(LIST * list, void * item); //or node_t *item?
int ListAppend(LIST * list, void * item);
int ListPrepend(LIST * list, void * item);
void *ListRemove(LIST * list);
void ListConcat(LIST * list1, LIST * list2);
void ListFree(LIST * list, void (* itemFree)(void *)); //or node_t *itemFree?
void *ListTrim(LIST * list);
void *ListSearch(LIST * list, int (* comparator_function)(void * value_x, void * value_y), void * comparisonArg); //both comparator and comparisonArg may be note_t *, instead of void *
#endif

