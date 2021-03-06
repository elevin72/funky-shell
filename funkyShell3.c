#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <wait.h>

struct command {
	char **argv;
};

char** listify(const char* str, const char* symbol, int* n);
void freeListified(char** list);
char* findInPath(const char* cmd);
char** formatCommand(const char* command);
struct command* formatCommands(const char* command);
void freeCommands(struct command* list);
void spawnProcess(int in, int out, const struct command *cmd);

// turn a space string of words into an array of char*, each containing one of the words
char** listify(const char* str, const char* symbol, int* n) {
	char* tempList[1000]; 
	char strLocal[800], *token, *str_saveptr;
	strcpy(strLocal, str); // never edit the real string
	token = strtok_r(strLocal, symbol, &str_saveptr);
	int i;
	for(i = 0; token != NULL; ++i) {
		int size = strlen(token) + 1;
		tempList[i] = malloc((strlen(token) + 1) * sizeof(char));
		strcpy(tempList[i], token);
		token = strtok_r(NULL, symbol, &str_saveptr);
	}
	char** returnList = calloc(i+1, sizeof(char*));
	for(int j = 0; j < i; ++j) {
		returnList[j] = tempList[j];
	}
	returnList[i] = NULL; // end the list with a NULL
	*n = i;
	return returnList;
}

// free a list created by listified()
void freeListified(char** list) {
	for(int i = 0; list[i] != NULL; ++i) {
		free(list[i]);
	}
	free(list);
}

// returns a string containing the full path of the executable in cmd
// returns NULL if not found
char* findInPath(const char* cmd) {
	char dirInPath[100]; // individual directory in the path
	char path[1000];
	char* fixedPath; // pointer to value of path as stored in the system
	char* pathTokenPtr; // pointer to section of path. Used when extracting directories from the path
	char* path_saveptr; // for internal use of strtok_r. Use when tokenizing path
	char* fullExe; // pointer to heap memory containing the full path of cmd
	DIR *d; // A directory stream??
	struct dirent *executableFile; // individual files in dir

	fixedPath = getenv("PATH");
	strcpy(path, fixedPath); // never edit the fixedPath
	pathTokenPtr = strtok_r(path, ":", &path_saveptr);
	while (pathTokenPtr && strcpy(dirInPath, pathTokenPtr)) { // loop over dirs in path
		d = opendir(dirInPath);
		if (d) {
			while ((executableFile = readdir(d))) { // loop over executables in directory
				if(strcmp(executableFile->d_name, cmd) == 0) {
					strcat(dirInPath, "/");
					strcat(dirInPath, cmd);
					fullExe = malloc((strlen(dirInPath)+1) * sizeof(char));
					strcpy(fullExe, dirInPath);
					closedir(d);
					return fullExe;
				}
			}
		}
		pathTokenPtr = strtok_r(NULL, ":", &path_saveptr); // get next dir in path
		closedir(d);
	}
	return NULL;
}

// Format a single command.
char** formatCommand(const char* command) {
	int n;
	char** commandList = listify(command, " ", &n);
	char* executable = findInPath(commandList[0]);
	if(!executable) {
		printf("Executable '%s' not found.", commandList[0]);
		return NULL;
	}
	commandList[0] = realloc(commandList[0], strlen(executable) + 1);
	strcpy(commandList[0], executable);
	free(executable);
	return commandList;
}

// takes a string and returns an array of commands, each one representing one of the pipes in the entire command
struct command* formatCommands(const char* command) {
	int numPipes;
	struct command cur;
	char** pipes = listify(command, "|", &numPipes);
	struct command* commandList = calloc(numPipes + 1, sizeof(struct command*));
	int i;
	for(i = 0; i < numPipes; ++i) {
		cur.argv = formatCommand(pipes[i]);
		commandList[i] = cur;
	}
	freeListified(pipes);
	commandList[i].argv = NULL;
	return commandList;
}

// delete the list created by formatCommand()
void freeCommands(struct command* list) {
	for(int i = 0; list[i].argv != NULL; ++i) {
		freeListified(list[i].argv);
	}
	free(list);
}

// fork and execv, using the specified fd's for in and out
void spawnProcess(int in, int out, const struct command* cmd) {
	pid_t pid;
	if ((pid = fork ()) == 0) {
		if (in != 0) {
			dup2(in, 0);
			close(in);
		}
		if (out != 1) {
			dup2(out, 1);
			close(out);
		}
		execv(cmd->argv[0], cmd->argv);
		fprintf(stderr, "WHOOPSIES"); // should never be here
	}
	wait(NULL);
}

int main () {
	char buffer[1000]; // buffer which stores the entire command
	int bufferSize; // The size of buf
	int in, fd[2];

	//main loop
	for(;;) {
		printf("??? > ");
		fflush(stdout);
		bufferSize = read(0, buffer, 999);
		buffer[bufferSize-1] = 0;
		struct command* commands = formatCommands(buffer);
		if(commands[0].argv == NULL) {
			printf("\n");
			continue;
		}
		int in = fileno(stdin);
		int i;
		for(i = 0; commands[i+1].argv != NULL; ++i) { // do all the pipes
			pipe(fd);
			spawnProcess(in, fd[1], commands + i);
			close(fd[1]);
			in = fd[0]; // save where we outputted this previous command
		}
		spawnProcess(in, 1, commands + i); //output final command to stdout
		freeCommands(commands); // cleanup
	}
}

