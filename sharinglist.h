#ifndef SHARINGLIST_H
#define SHARINGLIST_H

#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "vector.h" 
#include "globals.h"

static Vector* SharingList;
  

void init_sharing_list(){
	// Create the SharingList
	SharingList = CreateVector();
}
 

void free_sharing_list(){
	// Destroy the vector and call free on each element
	DestroyVector(SharingList, free);
} 
FileEntry* get_from_sharing_list(char* name){
	int i = 0;
	for(; i < SharingList->count; ++i){
		if(SharingList->data[i] != NULL)
			if(strcmp(((FileEntry*)(SharingList->data[i]))->name, name) == 0)
				return (FileEntry*)(SharingList->data[i]);
	}
	return NULL;
} 

bool sharing_list_contains(char* name){
	// Iterate over all entries and return true on the first match
	return get_from_sharing_list(name) != NULL; 
}

void remove_from_sharing_list(char* name){
	// Iterate over all entries and return true on the first match
	int i = 0;
	for(; i < SharingList->count; ++i){
		if(SharingList->data[i] != NULL){
			if(strcmp(((FileEntry*)(SharingList->data[i]))->name, name) == 0){
				free(SharingList->data[i]);
				SharingList->data[i] = NULL;
			}
		}
	} 
}

void add_to_sharing_list(char* path, int perm) {
	log_entry("Adding to shared list: ");
	log_entry(path);
 
	DirectoryNode* dir_list = malloc(sizeof(DirectoryNode));
	strcpy(dir_list->path, path);
	dir_list->next = NULL;
 
 	// Check if the path if a file
 	struct stat path_stat;
	stat(path, &path_stat);
	if(S_ISREG(path_stat.st_mode) == false){ 
		// Iterate the dir_list linked list until the end is reached
		// when ever a directory is discovered, it's added as the next element
		do {
			// Open the directory
			DIR* directory = opendir(dir_list->path); 
			// If directory is valid,
			// add other directories to the list and process entries
			if (directory != 0) { 
				struct dirent* entry;
				while((entry = readdir(directory)) != NULL){ 
					// Filter out . and ..
					if( !strcmp(entry->d_name, ".") ||
						!strcmp(entry->d_name, ".."))
						continue; 

					switch (entry->d_type) {
					case DT_DIR: { // If entry is a folder
						DirectoryNode* new_dir = malloc(sizeof(DirectoryNode));
						memset(new_dir, 0, sizeof(DirectoryNode));
						// Add the new directory as the 
						// next directory to be processed
						new_dir->next = dir_list->next;
						dir_list->next = new_dir;
						// Append the folder to the current path
						strcpy(new_dir->path, dir_list->path);
						strcat(new_dir->path, "/");
						strcat(new_dir->path, entry->d_name); 
					}
					break;
					case DT_REG: { // If a entry is a file
						// Create and set the entry with a name,
						// path and permission
						FileEntry* new_entry = malloc(sizeof(FileEntry));
						memset(new_entry, 0, sizeof(FileEntry));
						strcpy(new_entry->path, dir_list->path);
						strcat(new_entry->path, "/");
						strcat(new_entry->path, entry->d_name);
						strcpy(new_entry->name, entry->d_name);
						new_entry->permissions = perm; 
						// If this file does not already exist
						// add it into the sharing list
						if(sharing_list_contains(new_entry->path) == false)
							AppendVector(SharingList, new_entry);

						if(VERBOSE){
							printf("File Added: %s \n", new_entry->path);
						}
					}
					break;
					}
				} 
				// Close the directory as we're finished with it
				closedir(directory);
			}
			// Move onto the next directory
			DirectoryNode* old = dir_list;
			dir_list = dir_list->next;
			free(old);
			// While we're not at the end of the list
		} while (dir_list != NULL); 
	}
	else if(access(path, F_OK) == 0)
	{ 
		// Add the individual file into the list
		FileEntry* new_entry = malloc(sizeof(FileEntry));
		memset(new_entry, 0, sizeof(FileEntry)); 
		strcpy(new_entry->path, path);
		char* offset = strrchr(path, '/');
		if(offset == NULL)
			strcpy(new_entry->name, path);
		else
			strcpy(new_entry->name, offset); 
		new_entry->permissions = perm;
		if(sharing_list_contains(new_entry->path) == false)
			AppendVector(SharingList, new_entry);

		if(VERBOSE){
			printf("File Added: %s \n", new_entry->path);
		}
	}else{
		log_entry("Unable to add file to directory");
	}

} 

void save_sharing_list() { 
	// Check for the existance of the file
	FILE* sharing_file = fopen("sharing.list", "w"); 
	// line entry for file
	// "path name permissions"
	//CompactVector(SharingList);
	int i = 0;
	for(; i < SharingList->count; ++i){
		// Check entries
		FileEntry* entry = SharingList->data[i];
		if(entry != NULL)
			fprintf(sharing_file, "%s %s %i \n", entry->path, entry->name, entry->permissions); 
	}
	fclose(sharing_file);  
}

void load_sharing_list() { 
	// Check for the existance of the file
	if (access("sharing.list", F_OK) == 0) { 
		FILE* sharing_file = fopen("sharing.list", "r");

		// line entry for file
		// "path name permissions"

		while(!feof(sharing_file)){
			// Check entries
			FileEntry* entry = malloc(sizeof(FileEntry));
			if(fscanf(sharing_file, "%s%s%i\n", entry->path, entry->name, &entry->permissions) != 3){
				free(entry);
				continue; 
			}
 			// If the file exists then add it into the sharing list
 			if(access(entry->path, F_OK) == 0)
 				AppendVector(SharingList, entry); 
 			else{
 				log_entry("Unable to find file ");
 				log_entry(entry->path);
 				log_entry(" removing entry\n");
 				free(entry);
 			}
		}
 
		fclose(sharing_file); 
	} 
}
 
#endif