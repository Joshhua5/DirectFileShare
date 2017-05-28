#ifndef MAIN_C
#define MAIN_C

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> 
#include <signal.h>

#include "vector.h"
#include "config.h"
#include "logger.h"
#include "vector.h"
#include "peers.h" 
#include "server.h"
#include "client.h"
#include "console.h"
#include "sharinglist.h"


static bool Running = true;

void Init(){ 
	// Initialise the program
	// Load configuration and allocate 
	// sharing structre
	init_sharing_list();
	init_peer_list();
	Configuration = load_config();
	start_logging(Configuration.log_file);
	load_sharing_list();
	load_peer_list();

	// Start server 
	start_server();
}

void Cleanup(){
	// Save SharingList and free allocations
	stop_server();
 	save_sharing_list();
 	save_peer_list();
	free_config(Configuration);
	free_sharing_list();
	free_peer_list();
}

static void signal_handeler(int signal){
	switch(signal){
		case SIGTERM:
			Running = false;
			log_entry("SIGTERM Received, terminating");
			break;
		case SIGHUP:
			log_entry("Termainal reset started");

			Cleanup();
			Init();
			break;
		default:
			return;
	}
	return;
}

int main(int argc, char* argv[]) {  
	// Register a signal handeler
	signal(SIGTERM, signal_handeler);
	signal(SIGABRT, signal_handeler);
	signal(SIGFPE, signal_handeler);
	signal(SIGILL, signal_handeler);
	signal(SIGSEGV, signal_handeler);
	signal(SIGHUP, signal_handeler);
	signal(SIGINT, signal_handeler);
 
	Init();

	// User Interface
	printf("Legendary File Sharing\n");    
	char input[256];
	char ignore[64];

	while(Running){
		enum ConsoleInputType type = GetInput(input);

		switch(type){
			case CIT_ADDUSER:
			{ 
				Peer* peer = malloc(sizeof(Peer));
				memset(peer, 0, sizeof(Peer));
				int matches = sscanf(input, "%64s%s%s%i%i", ignore, peer->name, peer->addr, &peer->port, &peer->permissions);
				if(matches == 5)
					add_to_peer_list(peer);
				else{
					printf(">> Missing arguments for adduser");
					free(peer);
				}
			}
			break;
			case CIT_ADD:
			{ 
				int permission = 0;
				char target[254];
				int matches = sscanf(input, "%64s %254s %i", ignore, target, &permission);
				if(matches == 3) // We were unable to get the permissions to add to the file
					add_to_sharing_list(target, permission); 
				else
					printf(">> Missing arguments for add\n");
			}
			break;
			case CIT_REMOVE:
			{
				char target[254];
				if(0 <= sscanf(input, "%64s %254s", ignore, target)) 
					remove_from_sharing_list(target);
			}
			break;
			case CIT_LISTFILE :
			{
				int i = 0;
				for(; i < SharingList->count; ++i){
					FileEntry* entry = (FileEntry*)(SharingList->data[i]);
					printf("%i: %s %i\n",i, entry->path, entry->permissions);
				}
			}
			break;
			case CIT_LISTUSER:
			{
				int i = 0;
				for(; i < PeerList->count; ++i){
					Peer* peer = (Peer*)(PeerList->data[i]);
					printf("%i: %s %s:%i %i\n",i,
						peer->name, peer->addr, peer->port, peer->permissions);
				}
			}
			break;
			case CIT_LISTSHARE:
			{
				char peer_name[64];
				sscanf(input, "%64s %64s", ignore, peer_name);
				connection_t* connection = ConnectTo(peer_name);
				if(connection != NULL){
					GetSharedList(connection);
					free(connection); 
				}
			}
			break;
			case CIT_DOWNLOAD:
			{
				char peer_name[64];
				char file_name[254];
				sscanf(input, "%64s %64s %254s", ignore, peer_name, file_name);
				connection_t* connection = ConnectTo(peer_name);
				if(connection != NULL){
					GetFile(connection, file_name);
					free(connection); 
				}
			}
			break;
			case CIT_UNKNOWN:
			case CIT_HELP:
				printf("\tadduser NAME ADDRESS PORT PERMISSIONS \n");
				printf("\tadd [(file)|(folder)] \n");
				printf("\tremove [(file)|(folder)] \n"); 
				printf("\tlist [(users)|(files)]\n");
				printf("\tlistshare NAME\n");
				printf("\tdownload NAME FILE\n");
				printf("\texit\n");
				break;
			case CIT_EXIT:
				Running = false;
				break;
		}    
	}

	Cleanup();

	stop_logging();
	return 0; 
}

#endif