#ifndef NETWORKING_H
#define NETWORKING_H

#include "globals.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

static __thread char NetBuffer[NET_BUF_SIZE];

typedef struct connection_t {
	int socket;
	struct sockaddr_in address;
} connection_t;

enum CommandType{
	CT_DOWNLOAD = 1,
	CT_LIST = 2,
	CT_INVALID = 3
} CommandType;

// Must be of same length
const char* STR_DOWNLOAD = "DOWN";
const char* STR_LIST = "LIST";

typedef struct NetCommand{
	char* identity;
	enum CommandType type; 

	// Valid for only CommandType::DOWNLOAD
	char* file;
} NetCommand;

NetCommand* CreateCommand(){
	NetCommand* command = malloc(sizeof(NetCommand));
	command->identity = malloc(64);
	command->file = malloc(128);
}
void FreeCommand(NetCommand* command){
	free(command->file);
	free(command->identity);
	free(command);
}
// Checks if server/client is starting to close
// returns true if shutdown started
bool CheckShutdown(int socket){
	char buffer[sizeof(off_t) + 1];
	memset(buffer, ' ', sizeof(off_t) + 1);
	recv(socket, buffer, sizeof(off_t) + 1, MSG_PEEK | MSG_WAITALL);
	return buffer[sizeof(off_t)] == 'c';
}

void SendMessageSize(int socket, off_t size){
	char buffer[sizeof(off_t) + 1];
	memcpy(buffer, &size, sizeof(off_t)); 
	buffer[sizeof(off_t)] = ' ';
	send(socket, buffer, sizeof(off_t) + 1, 0);
}

off_t GetMessageSize(int socket){
	off_t size[2]; 
	recv(socket, size, sizeof(off_t) + 1, MSG_WAITALL);
	if(((char*)size)[sizeof(off_t)] == 'c'){
		log_entry("Invalid termination during command");
	}
	return size[0];
}

// Inform the client/server we are ready to close
void StartShutdown(int socket){
	char buffer[sizeof(off_t) + 1];
	memset(buffer, ' ', sizeof(off_t) + 1);
	buffer[sizeof(off_t)] = 'c';
	send(socket, buffer, sizeof(off_t) + 1, 0);
	// Wait for a responce confirming the close
}

void SendCommand(int socket, NetCommand* command){
	int msg_size = 0;
	switch(command->type){
		case CT_LIST:
			msg_size = sprintf(NetBuffer, "%s %s", Configuration.username, STR_LIST);
			break;
		case CT_DOWNLOAD:
			msg_size = sprintf(NetBuffer, "%s %s %s", Configuration.username, STR_DOWNLOAD, command->file);
			break;
		case CT_INVALID:
			log_entry("Invalid request passed");
			return;
	}
	NetBuffer[msg_size++] = '\0';

	SendMessageSize(socket, msg_size);
	send(socket, NetBuffer, msg_size, 0); 
}

NetCommand* AwaitCommand(int socket){
	NetCommand* command = CreateCommand();
	if(CheckShutdown(socket)){
		command->type = CT_INVALID;
		log_entry("Shutdown received before command");
		return command;
	}
 
	char cmd[4]; 

	off_t msg_size = GetMessageSize(socket);  
	recv(socket, NetBuffer, msg_size, MSG_WAITALL);
	sscanf(NetBuffer, "%64s %4s %128s", command->identity, cmd, command->file);

	if(strcmp(STR_LIST, cmd) == 0){
		command->type = CT_LIST;
	}else if(strcmp(STR_DOWNLOAD, cmd) == 0){
		command->type = CT_DOWNLOAD;
	}else{
		command->type = CT_INVALID;
		log_entry("Invalid command received from server");
	}
	return command;
}


// Send a file to the client
void SendFile(int fp, int socket){
	log_entry("%i: Sending File", pthread_self());
	// Determine file size (64 bit)
	off_t file_size = lseek(fp, 0, SEEK_END);
	lseek(fp, 0, SEEK_SET);
    
	SendMessageSize(socket, file_size);

	// Start sending file to the client 
	ssize_t read_size = 0;
	while((read_size = read(fp, NetBuffer, NET_BUF_SIZE)) != 0){ 
		send(socket, NetBuffer, read_size, 0);
	} 
}

int min(int a, int b){
	return a > b ? b : a;
}

// Receive a file from the server
void ReceiveFile(FILE* fp, int socket){ 
	log_entry("Receiving file");
	off_t file_size = GetMessageSize(socket); 
	off_t remaining = file_size;
	while(remaining > 0){
		size_t size = recv(socket, NetBuffer, min(remaining, NET_BUF_SIZE), 0);
		fwrite(NetBuffer, 1, size, fp);
		remaining -= size;
	}
	memset(NetBuffer, 0, sizeof(NET_BUF_SIZE)); 
}

// Send the sharing list to the client
void SendSharingList(int socket, Peer* peer){
	sprintf(LogBuffer, "%i: Started Sending Sharing List to peer %s", pthread_self(), peer->name); 
	log_entry(LogBuffer);
	int i = 0;
	for(; i < SharingList->count; ++i){
		FileEntry* fe = SharingList->data[i];
		if(fe != NULL){
			if((fe->permissions & peer->permissions) != 0){
						// We have permissions for this file
						// Maximum file na e of 255
				off_t send_size = strlen(fe->name) + 1; // Include the null termination
				SendMessageSize(socket, send_size);
				send(socket, fe->name, send_size, 0);
			}
		}
	}

}

// Receive the sharing list from the server
void ReceiveSharingList(int socket){
	log_entry("Started Receiving Sharing List");
	// While socket is still open
	while(CheckShutdown(socket) == false){
		off_t msg_size = GetMessageSize(socket);
		recv(socket, NetBuffer, msg_size, MSG_WAITALL);

		printf(">> Shared File: %s \n", NetBuffer); 
	} 
} 


connection_t* ConnectTo(char* peer_name){
	Peer* peer = get_peer(peer_name);
	if(peer == NULL){
		sprintf(LogBuffer, "Unable to find peer, connection failed, name: %s", peer_name);
 		log_entry(LogBuffer);
	}else{ 

		sprintf(LogBuffer, "Connecting to server: %s", peer->addr);
		log_entry(LogBuffer);
		// Get peer from list
	 	connection_t* conn = malloc(sizeof(connection_t));
	 	memset(&conn->address, 0, sizeof(struct sockaddr_in));
	 	conn->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	 	conn->address.sin_family = AF_INET;
	 	conn->address.sin_addr.s_addr = inet_addr(peer->addr);
	 	conn->address.sin_port = htons(peer->port);
	 
	 	if(connect(conn->socket, (struct sockaddr*)(&conn->address), sizeof(struct sockaddr_in)) == -1){
	 		sprintf(LogBuffer, "Unable to connect to server, connect error: %i", errno);
	 		log_entry(LogBuffer);
	 	}
	 	else
	 		return conn; 
 		free(conn);
 	}
 	// Only on error does it get to this point
 	// return conn is the return on success
 	return NULL;  
}

#endif