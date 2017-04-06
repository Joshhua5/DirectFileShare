#ifndef VECTOR_H
#define VECTOR_H

#include <string.h>
#include "logger.h"

typedef struct Vector {
	void** data; 
	unsigned int count;
	unsigned int capacity;
} Vector;

Vector* CreateVector(){
	Vector* vector = malloc(sizeof(Vector));
	if(vector == NULL){
		log_entry("Error with vector operation, CreateVector:this");		
		return NULL;
	}
	vector->data = malloc(sizeof(void*) * 10);
	if(vector->data == NULL){ 
		log_entry("Error with vector operation, CreateVector:Data");		
		free(vector);
		return NULL;
	}
	vector->count = 0;
	vector->capacity = 0; 
	return vector;
}

// Removes all entries of NULLptr
void CompactVector(Vector* vector) { 
	if(vector == NULL){
		log_entry("Error with vector operation, CompactVector");
		return;
	}
	int current_shift_count = 0;
	int i = 0;
	for (; i < vector->capacity; ++i) {
		// Shift over NULL pointers, until the end of the vector
		while (	vector->data[i + current_shift_count] == NULL &&
				i + current_shift_count < vector->capacity) {
			current_shift_count++;
		}
		vector->data[i] = vector->data[i + current_shift_count];
	}

	vector->count -= current_shift_count;
}

void RemoveAtVector(Vector* vector, int index) {
	vector->data[index] = NULL;
	CompactVector(vector);
}

void ExpandVector(Vector* vector, int expanded_count){
	if(vector == NULL){
		log_entry("Error with vector operation, ExpandVector");
		return;
	}
	void* new_data = malloc(sizeof(void*) * (expanded_count + vector->capacity));
	memcpy(new_data, vector->data, sizeof(void*) * vector->capacity);
	memset(vector->data, 0, sizeof(void*) * vector->capacity);
	vector->capacity += expanded_count;
	free(vector->data);
	vector->data = new_data;

	// Just a precaution
	CompactVector(vector);
}
 
void DestroyVector(Vector* vector, void(*destructor)(void*)){	
	if(vector == NULL){
		log_entry("Error with vector operation, DestroyVector");
		return;
	}
	if(destructor != NULL){
		int i = 0;
		for(; i < vector->count; ++i)
			(*destructor)(vector->data[i]);
	}
	free(vector->data); 
	free(vector);
}

void AppendVector(Vector* vector, void* data){
	if(vector == NULL){
		log_entry("Error with vector operation, AppendVector");
		return;
	}

	if(vector->count >= vector->capacity - 1){
		ExpandVector(vector, ((int)(float)vector->capacity * 0.1f) + 5);
	}
	vector->data[vector->count++] = data;
}
  

     
#endif