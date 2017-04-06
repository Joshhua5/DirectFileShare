#ifndef MAIN_C
#define MAIN_C

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> 

#include "vector.h"
#include "config.h"
#include "logger.h"
#include "vector.h"
#include "sharinglist.h"
 
int main(int argc, char* argv[]) { 
	// Initialise the program
	// Load configuration and allocate 
	// sharing structre
	init_sharing_list();
	Configuration = load_config();
	start_logging(Configuration.log_file);
	load_sharing_list(); 

	// User Interface
	printf("Legendary File Sharing\n");
	char user_input[256];
	char ignore[256];
	char argument[256];
	bool running = true;
	while(running){
		// Await user input for action
		printf(">> ");
		fgets(user_input, 256, stdin);
		// Add to SharingList
		if(strstr(user_input, "add")){
			sscanf(user_input, "%s%s", ignore, argument);
			if(argument != NULL)
				add_to_sharing_list(argument);
		}
		// Remove from SharingList
		else if(strstr(user_input, "remove")){
			sscanf(user_input, "%s%s", ignore, argument);
			if(argument != NULL)
				remove_from_sharing_list(argument);
		// Print out usable functions
		}else if(strstr(user_input, "help")){ 
			printf("\tadd [(file)|(folder)] \n");
			printf("\tremove [(file)|(folder)] \n"); 
			printf("\tlist\n"); 
			printf("\texit\n");  
		// Print out Sharinglist
		}else if(strstr(user_input, "list")){
			int i = 0;
			for(; i < SharingList->count; ++i)
				printf("%i: %s\n",i, ((FileEntry*)(SharingList->data[i]))->path);
		}else if(strstr(user_input, "exit")){
			running = false; 
		}
	}
	// Save SharingList and free allocations
 	save_sharing_list();
	free_config(Configuration);
	free_sharing_list();
	stop_logging();
	return 0; 
}

#endif