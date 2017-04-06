#ifndef VECTOR_H
#define VECTOR_H

#include <string.h>

typedef struct Vector {
	void** data; 
	unsigned int count;
	unsigned int capacity;
} Vector;

Vector* CreateVector(){
	Vector* vector = malloc(sizeof(Vector));
	vector->data = malloc(sizeof(void*) * 10);
	vector->count = 0;
	vector->capacity = 0; 
	return vector;
}

void ExpandVector(Vector* vector, int expanded_count){
	void* new_data = malloc(sizeof(void*) * (expanded_count + vector->capacity));
	memcpy(new_data, vector->data, sizeof(void*) * vector->capacity);
	memset(vector->data, 0, sizeof(void*) * vector->capacity);
	vector->capacity += expanded_count;
	free(vector->data);
	vector->data = new_data;
}

void DestroyVector(Vector* list){
	free(list->data); 
}
  
// Removes all entries of nullptr
void CompactVector(Vector* list) { 
	int current_shift_count = 0;
	int i = 0;
	for (; i < list->capacity; ++i) {
		while (	list->data[i + current_shift_count] == NULL &&
				i + current_shift_count < list->capacity) {
			current_shift_count++;
		}
		list->data[i] = list->data[i + current_shift_count];
	}

	list->count -= current_shift_count;
}

void RemoveAtVector(Vector* list, int index) {
	list->data[index] = NULL;
	CompactVector(list);
}
     
#endif