#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include "globals.h"
#include "logger.h"
#include "vector.h"
 
Config create_config(const char* file_name) {
	Config new_config;
	new_config.username = "default";
	new_config.port = 8080;
	new_config.ip = "127.0.0.1";
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

		if(VERBOSE){
			printf("Configuration");
			printf("%s\n", loaded_config.username);
			printf("%i\n", loaded_config.port);
			printf("%s\n", loaded_config.ip);
			printf("%s\n", loaded_config.log_file);
		}
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
			log_entry("Unable to open config file"); 
		}
	}
}

void free_config(Config loaded_config) {
	free(loaded_config.username);
	free(loaded_config.ip);
	free(loaded_config.log_file);
}
#endif