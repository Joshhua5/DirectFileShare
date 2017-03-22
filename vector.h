typedef struct Vector {
	void* data;
	unsigned int size_of;
	unsigned int count;
	unsigned int capacity;
} Vector;

Vector* CreateVector(int size_of){
	Vector* vector = malloc(sizeof(Vector));
	vector->data = malloc(size_of * 10);
	vector->count = 0;
	vector->capacity = 0;
	vector->size_of = size_of;
	return vector;
}

void ExpandVector(Vector* vector, int expanded_count){
	data* new_data = malloc(size_of * (expanded_count + capacity));
	memcpy(vector->data, vector->new_data, vector->size_of * capacity);
	memset(vector->data, 0, vector->size_of * capacity);
	vector->capacity += expanded_count;
	free(vector->data);
	vector->data = new_data;
}

void DestroyVector(Vector* list){
	while(list->start != list->end){
		free(list->start->data);
		list->start = list->start->next;
	} 
	free(list);
}

void* NextVector(Vector* list){ 
	if( list->start == NULL ||
		list->current == list->end)
		return NULL; 
	list->current = list->current->next;
	return list->current->data;
}

void PrependVector(Vector* list, void* data){
	Vector* node = malloc(sizeof(Vector));
	node->next = list->start;
	list->start = node;

	// Initialise the new node
	if(list->end == NULL){
		list->end = node;
		list->current = node;
	}
}

void ResetVector(Vector* list){
	list->current = list->start;
}