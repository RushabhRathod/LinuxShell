/*
	Name: Rushabh Rathod
	Student ID: 110031267
*/

#include "smallsh.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define TIMEOUT 10

static int timed_out = 0;

char *sernameAndHostName();

void handler(int dummy);

int main()
{
	//some additional code for terminal prompt
	char *uname = sernameAndHostName(NULL);
		
	char prompt[PATH_MAX + HOST_NAME_MAX + LOGIN_NAME_MAX + 10] = "", HOME_DIR[PATH_MAX] = "";
	strcpy(HOME_DIR, getenv("HOME"));
		
	
	char wd[PATH_MAX];
	if( getcwd(wd, PATH_MAX) == NULL) {
		perror("Cannot find current working directory");
		exit(2);
	}

	strcat(prompt, uname);
	strcat(prompt, wd);
	strcat(prompt, "$ ");

	//alarm signal for 10 second notification
	signal(SIGALRM, handler);

	alarm(TIMEOUT);
	while (userin(prompt) != EOF) 	{
		alarm(0);
		if(timed_out == 1) {
			int *temp;
			scanf("%m", temp);
			timed_out = 0;
			alarm(TIMEOUT);
			continue;
		}
		procline();

		if( getcwd(wd, PATH_MAX) == NULL) {
			perror("Cannot find current working directory");
			exit(2);
		}
		strcpy(prompt, "");
		strcat(prompt, uname);
		strcat(prompt, wd);
		strcat(prompt, "$ ");
		alarm(TIMEOUT);
	}
		
	
}

char *sernameAndHostName() {
	static char prompt[LOGIN_NAME_MAX + HOST_NAME_MAX + PATH_MAX + 10]  = "";
	char  *login, hostname[HOST_NAME_MAX];
	

	if( (login = getlogin()) == NULL ) {
		perror("Could not get login name");
		exit(1);
	}

	if (gethostname(hostname, HOST_NAME_MAX) == -1) {
		perror("Could not get the hostname\n");
		exit(1);
	}
	
	sprintf(prompt, "%s@%s:", login, hostname);
	return prompt;

}

void handler(int dummy) {
	timed_out = 1;
	fprintf(stderr, "\nPress any key to continue...");
}