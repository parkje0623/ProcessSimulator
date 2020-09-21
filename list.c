#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LIST_NUM 5
#define NODE_NUM 100
LIST all_heads[LIST_NUM];
NODE all_nodes[NODE_NUM];
int node_count = 0;

LIST * ListCreate(){
	LIST * newList = NULL;
	for (int i = 0; i < LIST_NUM; i++){
		if (all_heads[i].head == NULL){
			newList = &all_heads[i];
			newList -> head = NULL;
			newList -> tail = NULL;
			newList -> current = NULL;
		}
	}	
	return newList;
}

void * data(NODE * curr){
	if (curr == NULL) return NULL;
	return curr -> data;
}

int ListCount(LIST * list){
	if (list == NULL){
		return -1;
	}
	NODE * curr = list -> head;
	int count = 0;
	while (curr != NULL){
		count++;
		curr = curr -> next;
	}
	return count;
}

void * ListFirst(LIST * list){
	if (list == NULL){
		return NULL;
	}
	list ->  current = list -> head;
	return list -> current;
}

void * ListLast(LIST * list){
	if (list == NULL){
		return NULL;
	}
	list -> current = list -> tail;
	return list -> current -> data;
}

void * ListNext(LIST * list){
	if (list == NULL){
		return NULL;
	}
	if (list -> current == list -> tail){
		return NULL;
	}
	NODE * tmp_curr = list -> current;
	list -> current = tmp_curr -> next;
	return list -> current;
}

void * ListPrev(LIST * list){
	if (list == NULL){
		return NULL;
	}
	if (list -> current == list -> head){
		return NULL;
	}
	NODE * tmp_curr = list -> current;
	list -> current = tmp_curr -> prev;
	return list -> current;
}

void * ListCurr(LIST * list){
	if (list == NULL){
		return NULL;
	}
	if (list -> current == NULL){
		return NULL;
	}
	return list -> current;
}

int ListAdd(LIST * list, void * item){
	if (list == NULL){
		return -1;
	}
	if (item == NULL){
		return -1;
	}
	if (node_count > NODE_NUM){
		return -1;
	}
	if (list -> head == NULL){
		ListPrepend(list, item);
		return 0;
	}
	if (list -> head == list -> tail){
		ListAppend(list, item);
		return 0;
	}
	if (list -> current == list -> tail){
		ListAppend(list, item);
		return 0;
	}
	NODE * tmp = list -> current;
	NODE * tmp_next = list -> current -> next;
	all_nodes[node_count].data = item;
	all_nodes[node_count].next = list -> current -> next;
	all_nodes[node_count].prev = list -> current;
	tmp -> next = &all_nodes[node_count];		
	tmp_next -> prev = &all_nodes[node_count];
	tmp = &all_nodes[node_count];
	list -> current = list -> current -> next;
	node_count++;
	return 0;
}

int ListInsert(LIST * list, void * item){
	if (list == NULL){
		return -1;
	}
	node_count++;
	if (node_count > NODE_NUM){
		return -1;
	}
	if (list -> head == NULL){
		ListPrepend(list, item);
		return 0;
	}
	if (list -> head == list -> tail){
		ListPrepend(list, item);
		return 0;
	}
	if (list -> current == list -> head){
		ListPrepend(list, item);
		return 0;
	}
	NODE * tmp = list -> current;
	NODE * tmp_prev = list -> current -> prev;
	all_nodes[node_count].data = item;
	all_nodes[node_count].next = list -> current;
	all_nodes[node_count].prev = list -> current -> prev;
	tmp -> prev = &all_nodes[node_count];
	tmp_prev -> next = &all_nodes[node_count];
	list -> current = &all_nodes[node_count];
	node_count++;
	return 0;
}

int ListAppend(LIST * list, void * item){
	if (list == NULL){
		return -1;
	}
	if (node_count > NODE_NUM){
		return -1;
	}
	all_nodes[node_count].data = item;
	all_nodes[node_count].next = NULL;
	all_nodes[node_count].prev = list -> tail;	
	NODE * tmp = list -> tail;
	if (tmp == NULL){
		list -> head = &all_nodes[node_count];
		list -> tail = list -> head;
	}
	else{
		tmp -> next = &all_nodes[node_count];
		list -> tail = tmp -> next;
	}
	list -> current = list -> tail;
	node_count++;
	return 0;
}

int ListPrepend(LIST * list, void * item){
	if (list == NULL){
		return -1;
	}
	if (node_count > NODE_NUM){
		return -1;
	}
	all_nodes[node_count].data = item;
	all_nodes[node_count].next = list -> head;
	all_nodes[node_count].prev = NULL;
	NODE * tmp = list -> head;
	if (tmp == NULL){
		list -> head = &all_nodes[node_count];
		list -> tail = list -> head;
	}
	else{
		tmp -> prev = &all_nodes[node_count];
		list -> head = tmp -> prev;
	}
	list -> current = list -> head;
	node_count++;
	return 0;
}

void *ListRemove(LIST * list){
	if (list == NULL){
		return NULL;
	}
	if (list -> head == NULL){
		return NULL;
	}
	NODE * tmp_remove = list -> current;
	if (list -> head == list -> tail){
		all_nodes[node_count].data = NULL;
		list -> head = NULL;
		list -> tail = NULL;
		list -> current = list -> head;
		node_count--;
		return tmp_remove -> data;
	}
	if (list -> current == list -> tail){
		list -> tail = list -> tail -> prev;
		list -> tail -> next = NULL;
		list -> current = list -> tail;
		//all_nodes[node_count].data = NULL;
		node_count--;
		all_nodes[node_count].data = NULL;
		return tmp_remove -> data;
	}
	if (list -> current == list -> head){
		all_nodes[node_count].data = NULL;
		list -> head = list -> current -> next;
		list -> current = list -> current -> next;
		node_count--;
		return tmp_remove -> data;
	}
	all_nodes[node_count].data = NULL;
	list -> current -> prev -> next = list -> current -> next;
	list -> current -> next -> prev = list -> current -> prev;
	list -> current = list -> current -> next;
	node_count--;
	return tmp_remove -> data;
}

void ListConcat(LIST * list1, LIST * list2){
	if (list1 == NULL || list2 == NULL){
		return NULL;
	}
	if (list2 -> head == NULL){
		list1 -> current = list1 -> current;
		return;
	}
	if (list1 -> head == NULL){
		list1 -> head = list2 -> head;
		list1 -> tail = list2 -> tail;
		list1 -> current = list2 -> current;
		list2 -> head = NULL;
		list2 -> tail = NULL;
		list2 -> current = NULL;
		return;
	}
	list1 -> tail -> next = list2 -> head;
	list2 -> head -> prev = list1 -> tail;
	list1 -> current = list1 -> current;
	list2 -> head = NULL;
	list2 -> tail = NULL;
	list2 -> current = NULL;
	return; 
}

void itemFree(){
	all_nodes[node_count].data = NULL;
	all_nodes[node_count].next = NULL;
	all_nodes[node_count].prev = NULL;
	node_count--;
}

void ListFree(LIST * list, void (*itemFree)(void *)){
	if (list == NULL){
		return NULL;
	}
	NODE * tmp_free = list -> tail;
	while (tmp_free != NULL){
		tmp_free = list -> tail -> prev;
		list -> tail = list -> tail -> prev;
		(*itemFree)(tmp_free);
	}
	list -> head = NULL;
	list -> tail = NULL;
	list -> current = NULL;
}

void *ListTrim(LIST * list){
	if (list == NULL){
		return NULL;
	}
	if (list -> head == NULL){
		return NULL;
	}
	list -> current = list -> tail;
	node_count++;
	return ListRemove(list);
}

int comparator(void * value_x, void * value_y){
	if (value_x == value_y){
		return 1;
	}
	return 0;
}
	
void *ListSearch(LIST * list, int (* comparator)(void * value_x, void * value_y), void * comparisonArg){
	if (list == NULL){
		return NULL;
	} 
	if (list -> head == NULL){
		return NULL;
	}
	if (list -> head == list -> tail){
		if ((*comparator)(list -> head -> data, comparisonArg)){
			return list -> head -> data;
		}
	}
	NODE * tmp = list -> current;
	while (tmp != NULL){
		if ((*comparator)(tmp -> data, comparisonArg)){
			return tmp -> data;
		}
		tmp = tmp -> next;
	}
	return NULL;
	
}

int ListPrint (LIST *list){
	if (list == NULL) return -1;
	
	if (node_count > NODE_NUM){
		printf("Too many nodes, nodes overload!\n");
		return -1;
	}


	NODE * curr = list -> head;
	if (curr == NULL){
		printf("Empty list\n");
		return 0;
	}
				
	while (curr != NULL){
		printf("%d ", (curr -> data));
		curr = curr -> next;
	}
	printf("\n");
}

