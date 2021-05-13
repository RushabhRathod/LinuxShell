/*
	Name: Rushabh Rathod
	Student ID: 110031267
*/

//*****************************************************************
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

//#include <sys/wait.h>



#define EOL 1 
#define ARG 2 

#define AMPERSAND 3 
#define SEMICOLON 4 
#define MAXARG 512 
#define MAXBUF 512 

#define FOREGROUND 0 
#define BACKGROUND 1



int userin(char *p);
void procline(void);

int gettok(char **outptr);
int runcommand(char **cline, int where);

