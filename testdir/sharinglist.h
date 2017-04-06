#ifndef SHARINGLIST_H
#define SHARINGLIST_H

#include <stdio.h>
#include <errno.h>
#include <dirent.h>

#include "vector.h"

static Vector* SharingList;
 
void add_to_sharing_list(FileEntry* entry){
	add_to_sharing_list
}

void init_sharing_list(){
	SharingList = CreateVector();
}
 

void free_sharing_list(){
	DestroyVector(vector, free);
}

bool sharing_list_contain(char* path){
	int i = 0;
	for(; i < SharingList->count; ++i){
		if(strcmp((FileEntry*)(SharingList->data[i])->path, path))
			return true;
	}
	return false;
}

void add_directory_to_sharing_list(char* path) {
	log_entry("Adding to shared list: ");
	log_entry(path);
 
	DirectoryNode* dir_list = malloc(sizeof(DirectoryNode));
	strcpy(dir_list->path, path);
	dir_list->next = NULL;
 
	do {
		DIR* directory = opendir(dir_list->path); 
		// If directory is valid,
		// add other directories to the list and process entries
		if (directory != 0) {
			// Skip past the 
			struct dirent* entry;
			while((entry = readdir(directory)) != NULL){ 
				// Filter out . and ..
				if( !strcmp(entry->d_name, ".") ||
					!strcmp(entry->d_name, ".."))
					continue;
		
				switch (entry->d_type) {
				case DT_DIR: {
					DirectoryNode* new_dir = malloc(sizeof(DirectoryNode));
					new_dir->next = dir_list->next;
					dir_list->next = new_dir;
					// Append the folder to the current path
					strcpy(new_dir->path, dir_list->path);
					strcat(new_dir->path, "/");
					strcat(new_dir->path, entry->d_name); 
				}
				break;
				case DT_REG: {
					FileEntry* new_entry = malloc(sizeof(FileEntry)); 
					strcpy(new_entry->path, dir_list->path);
					strcat(new_entry->path, "/");
					strcat(new_entry->path, entry->d_name);
					strcpy(new_entry->name, entry->d_name);

					if(sharing_list_contain(new_entry->path) == false)
						AppendVector(SharingList, new_entry);

					if(VERBOSE){
						printf("File Added: %s \n", new_entry->path);
					}
				}
				break;
				}
			}  
			closedir(directory);
		}
		// Move onto the next directory
		DirectoryNode* old = dir_list;
		dir_list = dir_list->next;
		free(old);
	} while (dir_list != NULL); 
} 

void save_sharing_list() {
	const char* sharing_file = "sharing.list";
	// Check for the existance of the file
	FILE* sharing_file = fopen(sharing_file, "w"); 
	// line entry for file
	// "path name permissions"
	CompactVector(SharingList);
	int i = 0;
	for(; i < SharingList->count; ++i){
		// Check entries
		FileEntry* entry = SharingList->data[i]; 
		fprintf(sharing_file, "%s %s %s", entry->path, entry->name, entry->permissions); 
	}
	fclose(sharing_file);  
}

void load_sharing_list() {
	const char* sharing_file = "sharing.list";
	// Check for the existance of the file
	if (access(sharing_file, F_OK) == 0) { 
		FILE* sharing_file = fopen(sharing_file, "r");

		// line entry for file
		// "path name permissions"

		while(!feof(sharing_file)){
			// Check entries
			FileEntry* entry = malloc(sizeof(FileEntry));
			fscanf(sharing_file, "%s %s %s", entry->path, entry->name, entry->permissions);
 			if(access(entry->path, F_OK) == 0)
 				add_to_sharing_list(entry);
 			else{
 				log_entry("Unable to find file ");
 				log_entry(entry->path);
 				log_entry(" removing entry\n");
 				free(entry);
 			}
		}
 
		fclose(sharing_file); 
	}
	else {
		switch (errno) {
		case ENOENT:
			// File does not exist
			save_sharing_list();
			break;
		default:
			// https://linux.die.net/man/2/access
			log_entry("Unable to open config file"); 
		}
	}
}
 
#endif