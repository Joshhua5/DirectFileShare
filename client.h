#ifndef CLIENT_H
#define CLIENT_H
   
#include "globals.h"
#include "logger.h" 
#include "peers.h" 
#include "networking.h"

// Functionality required

// Handshake with peer (username recieved must match our peer list)

// Query peer for file list
// Serve requests for file lists

// Serve requests for files
// Download file from peer

  
void GetFile(connection_t* conn, char* file_name){
	if(conn == NULL){
		log_entry("Invalid connection for GetFile");
		return;
	} 
  
	NetCommand* command = CreateCommand();
	command->type = CT_DOWNLOAD;
	strcpy(command->identity, Configuration.username);
	strcpy(command->file, file_name);

	SendCommand(conn->socket, command);

	if(CheckShutdown(conn->socket) == false){ 
		FILE* fp = fopen(file_name, "w+");
		ReceiveFile(fp, conn->socket);
		fclose(fp);
	}else{
		log_entry("No file received from server, may not exist");
	}
 
	StartShutdown(conn->socket); 
	// We're free to shutdown the socket
	shutdown(conn->socket, SHUT_RDWR);
	close(conn->socket); 
	FreeCommand(command);  
}

void GetSharedList(connection_t* conn){
	if(conn == NULL){
		log_entry("Invalid connection for GetSharedList");
		return;
	} 

 	if(conn == NULL){
		log_entry("Invalid connection for GetFile");
		return;
	} 
  
	NetCommand* command = CreateCommand();
	command->type = CT_LIST;  
	strcpy(command->identity, Configuration.username); 
 	SendCommand(conn->socket, command);
	ReceiveSharingList(conn->socket); 

	StartShutdown(conn->socket); 
	// We're free to shutdown the socket
	shutdown(conn->socket, SHUT_RDWR);
	close(conn->socket); 
	FreeCommand(command);  
} 
  

#endif