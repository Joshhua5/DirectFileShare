#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#include "globals.h"

enum ConsoleInputType
{
	CIT_ADDUSER,
	CIT_ADD,
	CIT_REMOVE,
	CIT_LISTFILE,
	CIT_LISTUSER,
	CIT_LISTSHARE,
	CIT_DOWNLOAD,
	CIT_HELP,
	CIT_EXIT,
	CIT_UNKNOWN
} ConsoleInputType; 

void strtolower(char* str, const int len){
	int i = 0;
	for(; i < len; ++i)
		str[i] = (char)tolower(str[i]);
}
 
enum ConsoleInputType GetInput(char* out_input){
	char command[16];
	char type[8];
	// Get all user input
	printf(">> ");
	fgets(out_input, 256, stdin);
	//scanf("%s", 256, out_input);
	// Get scan enough to get the command
	sscanf(out_input, "%16s %8s", command, type);
 	strtolower(command, 16); 

	if(strstr(command, "adduser"))
		return CIT_ADDUSER;
	else if(strstr(command, "add"))
		return CIT_ADD;
	else if(strstr(command, "remove"))
		return CIT_REMOVE;
	else if(strstr(command, "help"))
		return CIT_HELP;
	else if(strstr(command, "listshare"))
		return CIT_LISTSHARE;
	else if(strstr(command, "download"))
		return CIT_DOWNLOAD;
	else if(strstr(command, "list")){
		strtolower(type, 8);
		if(strstr(type, "file"))
			return CIT_LISTFILE;
		else if(strstr(type, "user"))
			return CIT_LISTUSER;
	}else if(strstr(command, "exit"))
		return CIT_EXIT;
	return CIT_UNKNOWN;
} 

#endif