#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
	char *args[] = {"/usr/bin/ls", "-l", NULL};
	pid_t pid = fork();
	if (pid == 0) {
		printf("In child pid = %i\n", getpid());
		sleep(5);
		execv("/usr/bin/ls", args);
	} else {
		/* wait(NULL); */
		printf("In parent pid = %i\n", getpid());
	}
}
