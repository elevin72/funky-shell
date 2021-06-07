#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
	/* char buf[1000]; // buffer which stores the entire command */
	/* char** arglist; // list of args to command */
    /* char* cmd_saveptr; // for internal use of strtok_r. Use when tokenizing buf */
    /* char* cmd; // The first word in buf */
	/* int bufsize; // The size of buf */
	/* bufsize = read(0, buf, 999); */
	/* buf[bufsize-1] = 0; */
	/* cmd = strtok_r(buf, " ", &cmd_saveptr); */

	/* write(fileno(stdout), "Hello\nGoodbye\n", 14); */
	/* execlp("wc", "wc", (char*)NULL); */




	/* pipe4.c */
	int pipefds[2];
	pid_t pid;
	if(pipe(pipefds) == -1){
		perror("pipe");
		exit(EXIT_FAILURE);
	}
	pid = fork();
	if(pid == -1){
		perror("fork");
		exit(EXIT_FAILURE);
	}
	if(pid == 0){
		//replace stdout with the write end of the pipe
		dup2(pipefds[1],STDOUT_FILENO);  
		//close read to pipe, in child    
		close(pipefds[0]);               
		execlp("ls","ls",NULL);
	}else{
		//Replace stdin with the read end of the pipe
		dup2(pipefds[0],STDIN_FILENO);  
		//close write to pipe, in parent
		close(pipefds[1]);               
		execlp("wc","wc",NULL);
	}   
}
