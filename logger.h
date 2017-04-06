#ifndef LOGGER_H
#define LOGGER_H

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include "globals.h"

static FILE* logger_file = NULL;
// Incase we move to a few more threads
static bool valid_file_lock = false;
static pthread_mutex_t file_lock;
 
void log_entry(char* msg){ 
	if(valid_file_lock)
		pthread_mutex_lock(&file_lock);

	if(VERBOSE)
		printf("<< %s \n", msg);

	if(logger_file != NULL)
  		fprintf(logger_file, "%s \n", msg);
  		
	if(valid_file_lock)
  		pthread_mutex_unlock(&file_lock);
}

void start_logging(char* log_file){
	if(logger_file != NULL){
		log_entry("");
	}

	// if init is invalid then valid file_lock is false and
	// we inverse to return true
	if(!(valid_file_lock = pthread_mutex_init(&file_lock, NULL) == 0))
		printf("Unable to create file lock");  


	logger_file = fopen(log_file, "a");

	if(logger_file == NULL){ 
		printf("Log file error, unable to open, errno %i", errno); 
		VERBOSE = true;
	}
}

void stop_logging(){
	pthread_mutex_destroy(&file_lock);
	fclose(logger_file);
	logger_file = NULL;
	valid_file_lock = false;
}

#endif