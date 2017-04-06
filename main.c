#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/queue.h>

#include "vector.h"

#define MAX_PATH_LENGTH 256

typedef struct Config {
	char* username;
	int port;
	char* ip;
	char* log_file;
}Config;

typedef struct SharingList {
	int list_count;
} SharingList;

Config create_config(const char* file_name) {
	Config new_config;
	new_config.username = "default";
	new_config.port = 8080;
	new_config.ip = "192.168.1.1";
	new_config.log_file = "assignment.log";

	FILE* config_file = fopen(file_name, "w");
	if (config_file != NULL) {

		fprintf(config_file, "%s%s\n", "USERNAME:", new_config.username);
		fprintf(config_file, "%s%i\n", "PORT:", new_config.port);
		fprintf(config_file, "%s%s\n", "IP:", new_config.ip);
		fprintf(config_file, "%s%s\n", "LOG_FILE:", new_config.log_file);

		fclose(config_file);
	}
	return new_config;
}

Config load_config() {
	const char* config_name = "settings.conf";
	// Check for the existance of the file
	if (access(config_name, F_OK) == 0) {
		FILE* config_file = fopen(config_name, "r");
		struct Config loaded_config;

		loaded_config.username = malloc(sizeof(char) * 255);
		loaded_config.ip = malloc(sizeof(char) * 255);
		loaded_config.log_file = malloc(sizeof(char) * 255);

		// Detect string length instead of assumming a max of 255
		// http://stackoverflow.com/questions/1621394/how-to-prevent-scanf-causing-a-buffer-overflow-in-c

		fscanf(config_file, "USERNAME:%255s\n", loaded_config.username);
		fscanf(config_file, "PORT:%i\n", &(loaded_config.port));
		fscanf(config_file, "IP:%255s\n", loaded_config.ip);
		fscanf(config_file, "LOG_FILE:%255s\n", loaded_config.log_file);

		printf("%s\n", loaded_config.username);
		printf("%i\n", loaded_config.port);
		printf("%s\n", loaded_config.ip);
		printf("%s\n", loaded_config.log_file);

		fclose(config_file);
		return loaded_config;
	}
	else {
		switch (errno) {
		case ENOENT:
			// File does not exist
			return create_config(config_name);
			break;
		default:
			// https://linux.die.net/man/2/access
			printf("Unable to open log file");
		}
	}
}

void free_config(Config loaded_config) {
	free(loaded_config.username);
	free(loaded_config.ip);
	free(loaded_config.log_file);
}

void save_sharing_list() {

}

void load_sharing_list() {

}


typedef struct DirectoryNode {
	void* next; // DirectoryNode
	char path[MAX_PATH_LENGTH];
} DirectoryNode;

typedef struct FileEntry {
	char path[MAX_PATH_LENGTH];

} FileEntry;

void scan_directory(char* path) {

	Vector* entry_list = CreateVector();
	DirectoryNode* dir_list = malloc(sizeof(DirectoryNode));
	strcpy(dir_list->path, path);
	dir_list->next = NULL;

	DIR* directory;
	directory = opendir(dir_list->path);
	while (dir_list->next != NULL) {
		// If directory is valid,
		// add other directories to the list and process entries
		if (directory != 0) {
			struct dirent* entry = readdir(directory);
			if (entry != NULL) {
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
					strcpy(new_entry->path, entry->d_name);
				}
							 break;
				}
			}
		}
		// Move onto the next directory
		DirectoryNode* old = dir_list;
		dir_list = dir_list->next;
		free(old);
		closedir(directory);
		directory = opendir(dir_list->path);
	}
}

int main(int argc, char* argv[]) {
	/*
	Add folder to sharing list: -a [(file)|(folder)]
	Remove folder from sharing list: -r [(file)|(folder)]


	*/
	  
	// Config loaded_config = load_config();
	 
	int i = 0;
	for (; i < argc; ++i)
		printf("%s", argv[i]);
	 
	//free_config(loaded_config);
	return 0;
}