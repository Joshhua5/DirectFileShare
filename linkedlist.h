#include <stdlib.h>

typedef struct LinkedListNode{
	void* data;
	LinkedListNode* next;

} LinkedListNode;

typedef struct LinkedList
{
	LinkedListNode* start;
	LinkedListNode* end;
	LinkedListNode* current; 
	int node_count;
} LinkedList;

LinkedList* CreateLinkedList(){
	LinkedList* list = malloc(sizeof(LinkedList));
	list->start = NULL;
	list->end = NULL;
	list->current = NULL;
	list->node_count = NULL;
	return list;
}

void DestroyLinkedList(LinkedList* list){
	while(list->start != list->end){
		free(list->start->data);
		list->start = list->start->next;
	} 
	free(list);
}

void* NextLinkedList(LinkedList* list){ 
	if( list->start == NULL ||
		list->current == list->end)
		return NULL; 
	list->current = list->current->next;
	return list->current->data;
}

void PrependLinkedList(LinkedList* list, void* data){
	LinkedListNode* node = malloc(sizeof(LinkedListNode));
	node->next = list->start;
	list->start = node;

	// Initialise the new node
	if(list->end == NULL){
		list->end = node;
		list->current = node;
	}
}

void AppendLinkedList(LinkedList* list, void* data){
	LinkedListNode* node = malloc(sizeof(LinkedListNode));
	node->next = list->start;
	list->start = node;

	// Initialise the new node
	if(list->end == NULL){
		list->end = node;
		list->current = node;
	}
}

void ResetLinkedList(LinkedList* list){
	list->current = list->start;
}