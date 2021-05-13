/*
	Name: Rushabh Rathod
	Student ID: 110031267
*/

#include "smallsh.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <sys/wait.h>

//checking if "cd" command is entered
int checkCD(char **cline);

//checking if typed quit
void checkQuit(char **cline);

//function from classes
int join(char *com1[], char *com2[]);

//checking for pipe
int checkPipe(char **cline); 

//redirection output to file
int checkRedirection(char **cline);

//finding if token exists
int find(char **cline, char *tok);

//check if user want to open calculator
int checkCalc(char **line);


int runcommand(char **cline, int where)
{
	// printf("Running command %s\n", cline[0]);
	pid_t pid;
	int status;
	// printf("temp: %ld\n", sizeof(cline)/sizeof(cline[0]));
	switch (pid = fork()) {
		case -1:
			perror("smallsh");
			return (-1);
		case 0:
			
			if(checkCalc(cline) == 1) {
				execl("./calculator", "calculator", (char *)0);
				perror("calculator error");
				return -1;
			}

			

			if(checkCD(cline) == 1) {
				return -1;
			}



			checkQuit(cline);

			

			if(checkPipe(cline))
				return -1;

			
			int fd;
			if( (fd = checkRedirection(cline)) == -1) 
				return -1;
			else {
								
				if (!fork()) {
					close(1);//Close stdout
					dup(fd);
					// execlp(cline[0], );
					// perror(*cline);
					// exit(1);
				} else {
					close(fd);
					wait(NULL);
					exit(1);
				}
				
			}
			


			execvp(cline[0], cline);
			perror(*cline);
			exit(1);
	}
	//code for parents
	if (where == BACKGROUND)
	{
		printf("[process id %d]\n", pid);
		return (0);
	}
	
	if (waitpid(pid, &status, 0) == -1)
		return (-1);
	else
		return (status);
}

int join(char *com1[], char *com2[]) {

	int p[2], status;

	switch (fork()) {
		case -1: 
			perror("1st fork call in join");
			exit(3);
		case 0: 
			break;
		default: 
			wait(&status);
			return(status);
	}
	
	//child process's code
	if (pipe(p) == -1){
		perror("pipecall in join ");
		exit(4);
	}

	switch (fork()) {
		case -1:
			perror("2nd fork call failed in join");
			exit(5);
		case 0:  //the writing process (grandchild procss)
			sleep(1);
			// printf("Child\n");
			dup2(p[1], 1);  //redirect stdout to the pipe, writing end.
				
			close(p[0]);
			close(p[1]);

			execvp(com1[0], com1);

			perror("1st execvp call failed in join");
			exit(6);
		default: //the "parent" process's code
			// printf("parent\n");
			dup2(p[0], 0); //redirect stdin to the pipe, reading end

			close(p[0]);
			close(p[1]);
			execvp(com2[0], com2);

			perror("2nd execvp call failed in join");
			exit(7);
		}
}


int checkCD(char **cline) {
	if(strcmp(cline[0], "cd") == 0){
		if(cline[1] == NULL) {
			chdir(getenv("HOME"));
		}
		else {
			chdir(cline[1]);
		}
		return 1;
	}
	return 0;
}

void checkQuit(char **cline) {
	if( (strcmp(cline[0], "quit") == 0) || (strcmp(cline[0], "exit") == 0)  ){
		printf("Command: %s\n", cline[0]);
		kill(0, SIGKILL);
	}
	// return 0;
}

int checkPipe(char **cline) {
	// printf("Pipe: '%s'\n", cline[1]);
	int i;
	
	if( (i = find(cline, "|")) != -1 ) {
	// if( (i = find(cline, "|")) != -1 ) {
		// printf("\nFound at %d\n", i);
		int j = 0, k = 0;
		char *cmd1[10], *cmd2[10];

		// printf("looping '%s' %d\n", cline[3], cline[3] != NULL);

		// while( strcmp(cline[j], "") != 0 ) {
		while( cline[j] != NULL ) {
			// printf("%d '%s'\t", j , cline[j]);
			if( j == i) {
				// printf("equal\t");
				cmd1[k] = NULL;
				k = 0;
				j++;
				continue;
			}
			if( j < i){
				// strcpy(cmd1[k], cline[j]);
				cmd1[k] = cline[j];
			}
			else {
				// strcpy(cmd2[k], cline[j]);
				cmd2[k] = cline[j];
			}
			j++;
			k++;

			// printf("%ld %ld\n", sizeof(cmd1)/sizeof(cmd1[0]), sizeof(cmd1)/sizeof(cmd1[0]) );
		}
		cmd2[k] = NULL;

		i = 0;
		// printf("\ncmd1 '%s %d'\n", cmd1[i], cmd1[i] != NULL);
		while(cmd1[i] != NULL) {
			// printf("%d: %s\n", i, cmd1[i]);
			i++;
		}

		i = 0;
		// printf("\ncmd1 '%s %d'\n", cmd2[i], cmd2[i] != NULL);
		while(cmd2[i] != NULL) {
			// printf("%d: %s\n", i, cmd2[i]);
			i++;
		}

		// printf("\n");
		// getc(stdin);
		join(cmd1, cmd2);
		return 1;
	}
	return 0;
	// if( strcmp(cline[1], "|") == 0) {
		
	// 	if(cline[3] != NULL ) {
	// 		printf("Cannot take more than 2 argumentless commands\n");
	// 		return 1;
	// 	}
	// 	char *cmd1[2] = {cline[0], NULL};
	// 	char *cmd2[2] = {cline[2], NULL};
	// 	join(cmd1, cmd2);
	// 	return 1;
	// }
	// return 0;
}

int checkRedirection(char **cline) {
	int i;
	if(  (i = find(cline, ">")) != -1 ) {
		// printf("Found\n");
		cline[i] = (char *)0;
		// if(cline[3] != NULL) {
		// 	printf("Cannot take more than 2 argumentless commands\n");
		// 	return -1;
		// }
		int fd = open(cline[i+1], O_WRONLY | O_CREAT , 0777);
		return fd;
	}
	return 0;
}

int find(char **cline, char *tok) {
	int i = 0;
	// printf("\n Finding\n");
	while(cline[i] != NULL) {
		// printf("%d %s\n", i, cline[i]);
		if(strcmp(cline[i], tok) == 0) {
			return i;
		}
		i++;
	}

	return -1;
}

int checkCalc(char **cline) {
	if(  (strcmp(cline[0], "calculator") == 0)  || (strcmp(cline[0], "./calculator") == 0) ) {
		return 1;
	}
	return 0;
}