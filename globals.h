#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdbool.h>

#define MAX_PATH_LENGTH 256


typedef struct DirectoryNode {
	void* next; // DirectoryNode
	char path[MAX_PATH_LENGTH];
} DirectoryNode;

typedef struct FileEntry {
	char path[MAX_PATH_LENGTH];
	char name[MAX_PATH_LENGTH];
	char permissions[MAX_PATH_LENGTH];

} FileEntry;

typedef struct Config {
	char* username;
	int port;
	char* ip;
	char* log_file;
} Config;

static bool VERBOSE = false;
static Config Configuration;

#endif