#ifndef GLOBALS_H
#define GLOBALS_H

#define __USE_FILE_OFFSET64

#include <stdbool.h> 

#define MAX_PATH_LENGTH 256
#define MAX_SERVER_CONNECTIONS 64
#define NET_BUF_SIZE 1024

#define CLOSE_MESSAGE "close_connection"

typedef struct DirectoryNode {
	void* next; // DirectoryNode
	char path[MAX_PATH_LENGTH];
} DirectoryNode;

typedef struct FileEntry {
	char path[MAX_PATH_LENGTH];
	char name[MAX_PATH_LENGTH];
	int permissions;
} FileEntry;

typedef struct Peer {
	char addr[128];
	int port;
	char name[64];
	int permissions;
} Peer;

typedef struct Config {
	char* username;
	int port;
	char* ip;
	char* log_file;
} Config;

static bool VERBOSE = true;
static Config Configuration;

static __thread char LogBuffer[256]; 

#endif