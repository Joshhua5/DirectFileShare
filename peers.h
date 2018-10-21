#ifndef PEERS_H
#define PEERS_H 

#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "vector.h"
#include "globals.h"

static Vector* PeerList;

void init_peer_list(){
	PeerList = CreateVector();
}

void free_peer_list(){
	DestroyVector(PeerList, free);
}
  
Peer* get_peer(char* name){
	int i = 0;
	for(; i < PeerList->count; ++i){ 
		Peer* entry = PeerList->data[i];
		if(entry != NULL) { 
			if(strcmp(entry->name, name) == 0)
				return (Peer*)(PeerList->data[i]); 
		}
	}
	return NULL;
} 


bool peer_list_contains(char* name){
	return get_peer(name) != NULL;
}
  
bool add_to_peer_list(Peer* peer){ 
	if(strlen(peer->name) == 0 || strlen(peer->addr) == 0)
		return false;
	return AppendVector(PeerList, peer); 
}

void save_peer_list() { 
	// Check for the existance of the file
	FILE* peer_file = fopen("peer.list", "w"); 
	// line entry for file
	// "path name permissions"
	//CompactVector(SharingList);
	int i = 0;
	for(; i < PeerList->count; ++i){
		// Check entries
		Peer* entry = PeerList->data[i];
		if(entry != NULL)
			fprintf(peer_file, "%s %s %i %i\n", entry->name, entry->addr, entry->port, entry->permissions); 
	}
	fclose(peer_file);  
}

void load_peer_list() { 
	// Check for the existance of the file
	if (access("peer.list", F_OK) == 0) { 
		FILE* peer_file = fopen("peer.list", "r");

		// line entry for file
		// "path name permissions"

		while(!feof(peer_file)){
			// Check entries
			Peer* entry = malloc(sizeof(Peer));
			int result = fscanf(peer_file, "%s%s%i%i\n", entry->name, entry->addr, &entry->port, &entry->permissions);
 			
 			// Validate Peer
 			if(result == 4)
 				AppendVector(PeerList, entry); 
 			else{
 				free(entry);
 				log_entry("Unable to load user");
 			}
		}
 
		fclose(peer_file); 
	} 
} 

#endif