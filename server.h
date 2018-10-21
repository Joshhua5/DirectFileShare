#ifndef SERVER_H
#define SERVER_H 

#include "globals.h"
#include "logger.h"
#include "sharinglist.h"
#include "peers.h"
#include "networking.h"

static pthread_t server_thread = 0;
static pthread_t server_workers[MAX_SERVER_CONNECTIONS];
static bool server_running = true; 

// Functionality required

// Handshake with peer (username recieved must match our peer list)

// Query peer for file list
// Serve requests for file lists

// Serve requests for files
// Download file from peer

typedef struct worker_pack_t {
	int socket;
	struct sockaddr_in* address;
} worker_pack_t;

bool ValidatePeer(worker_pack_t* pack, Peer* peer){
	char address[64]; 
	// Validate the ip address of the user
	inet_ntop(AF_INET, &(pack->address->sin_addr), address, 64); 
	if(peer == NULL){ 
		sprintf(LogBuffer, "Connection failed validation (USERNAME), Username: %s, Address: %s",
			peer->name, address);
		log_entry(LogBuffer);
		return false;
	}
	else if(strcmp(address, peer->addr) != 0){ 
		sprintf(LogBuffer, "Connection failed validation (ADDRESS), Username: %s, Address: %s",
			peer->name, address);
		log_entry(LogBuffer);
		return false;
	}
	return true;
}

void* server_worker(void* args){
	sprintf(LogBuffer, "%i: Starting server worker", pthread_self());
	log_entry(LogBuffer);

	worker_pack_t* pack = (worker_pack_t*)args;
	if(pack->socket == -1){
		log_entry("Unable to start server_worker, socket is -1");
	}

	NetCommand* command = AwaitCommand(pack->socket);
	if(command->type == CT_INVALID){
 		log_entry("Invalid command recieved");
 	}else{ 
		Peer* peer = get_peer(command->identity);
		if(ValidatePeer(pack, peer)) {
			sprintf(LogBuffer, "%i: Peer identified as %s from %s", pthread_self(), peer->name, peer->addr);
			log_entry(LogBuffer);
			switch(command->type){
				case CT_LIST:
					sprintf(LogBuffer, "%i: Peer requesting a sharing list", pthread_self());
					log_entry(LogBuffer);
					SendSharingList(pack->socket, peer);
				break; 
				case CT_DOWNLOAD:
				{
					sprintf(LogBuffer, "%i: Peer requesting file download: %s", pthread_self(), command->file);
					log_entry(LogBuffer);
				 	FileEntry* file = get_from_sharing_list(command->file);
					if(file != NULL){
						// Check Permissions

						// use open since inside SendFile we use lseek for
						// larger than gb files
						int fp = open(file->path, O_RDONLY); 
						SendFile(fp, pack->socket);
					}else{
						sprintf(LogBuffer, "%i: Unable to locate file requested: %s", pthread_self(), command->file);
						log_entry(LogBuffer);
					}
				}
				break;
			} 
		}
		sprintf(LogBuffer, "%i Server worker finished", pthread_self());
		log_entry(LogBuffer); 
	}

	StartShutdown(pack->socket);
	// Wait for the client to respond
	while(CheckShutdown(pack->socket) == false);
	shutdown(pack->socket, SHUT_RDWR);
	close(pack->socket); 

	// Valid request data:
	// USERNAME:LIST-ALL // Returns a list of all content the user has permissions too in the format of sharing-list
	// USERNAME:DOWNLOAD:ENTRY // 
 
 	FreeCommand(command);
	free(pack->address); 
	free(args);  

	return (void*)0;  
}

void* server_listener(void* arg){
	// Start listening for connections
	int server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in sa;
	memset(&sa, 9, sizeof(struct sockaddr_in));

	sa.sin_family = AF_INET;
	sa.sin_port = htons(Configuration.port);
	sa.sin_addr.s_addr = inet_addr(Configuration.ip);

	if(bind(server_socket, (struct sockaddr*) &sa, sizeof(struct sockaddr_in)) == -1){
		log_entry("Unable to start server, due to bind error, starting in client only mode");		
		return (void*) 0;
	}

	listen(server_socket, MAX_SERVER_CONNECTIONS); 

	while(server_running){
		// Listen for a connection, upon connection 
		// Create a worker to handel the connection
		worker_pack_t* pack = malloc(sizeof(worker_pack_t));
		int sockaddr_size = sizeof(struct sockaddr_in);
		pack->address = malloc(sockaddr_size);
		pack->socket = accept(server_socket, (struct sockaddr*)(pack->address), &sockaddr_size);
		if(pack->socket == -1){
			sprintf(LogBuffer, "Unable to accept connection: errno: %i", errno);
			log_entry(LogBuffer);
			free(pack->address);
			free(pack);
		}else{
			pthread_t worker;
			pthread_create(&worker, NULL, server_worker, pack);
		}
	}
	close(server_socket);
	return (void*) 0;
}

void start_server(){
	if(server_thread != 0){
		log_entry("Server already started");
		return;
	}

	if(pthread_create(&server_thread, NULL, server_listener, NULL) != 0)
		log_entry("Unable to start server worker");
}

void stop_server(){
	server_running = false; 
}



#endif