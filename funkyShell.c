#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <wait.h>
#include <stdbool.h>

// some globals... Bad Eli
int PIPE = 1;
int DONT_PIPE = 0;

struct command {
	char** argv;
};



// turn a space string of words into an array of char*, each containing one of the words
char** listify(const char* _words, const char* symbol) {
	char** list = malloc(strlen(_words)); // always overshoot number of commands
	char words[800], *word, *word_saveptr;
	char arg[80];
	strcpy(words, _words);
	word = strtok_r(words, symbol, &word_saveptr);
	int i = 0;
	for(; word != NULL; ++i) {
		int size = strlen(word) + 1;
		list[i] = malloc(size);
		strcpy(list[i], word);
		word = strtok_r(NULL, symbol, &word_saveptr);
	}
	list[i] = NULL; // end the list with a NULL
	return list;
}

// returns a string containing the full path of the executable in cmd
// returns NULL if not found
char* findInPath(char* cmd) {
	char dirInPath[100]; // individual directories in path
	char* path;
	char* fixedPath; // pointer to value of path as stored in the system
	char* pathTokenPtr; // pointer to section of path. Used when extracting directories from the path
	char* path_saveptr; // for internal use of strtok_r. Use when tokenizing path
	char* fullExe; // pointer to heap memory containing the full path of cmd
	DIR *d; // A directory stream??
	struct dirent *executableFile; // individual files in dir
	fixedPath = getenv("PATH");
	strcpy(path, fixedPath);
	pathTokenPtr = strtok_r(path, ":", &path_saveptr);
	while (pathTokenPtr && strcpy(dirInPath, pathTokenPtr)) { // loop over dirs in path
		d = opendir(dirInPath);
		if (d) {
			while ((executableFile = readdir(d))) { // loop over executables in directory
				if(strcmp(executableFile->d_name, cmd) == 0) {
					strcat(dirInPath, "/");
					strcat(dirInPath, cmd);
					fullExe = malloc(strlen(dirInPath) * sizeof(char));
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

struct command* formatCommands(char* commands) {
	struct command* cmd; 
	char** commandsList = listify(commands, "|");
	for(int i = 0; commandsList[i] != NULL; ++i) {
		

	}
	char* cmd = findInPath(res[0]);
	res[0] = realloc(res[0], strlen(cmd));
	strcpy(res[0], cmd);
	return res;
}

// fork and execv. Child dies before this function returns
void forkAndExecv(char** arglist, int pipeMaybe) {
	pid_t pid = fork ();
	if (pid<0) { // fork has failed 
		perror("fork"); exit(EXIT_FAILURE);
	}
	else if (pid == 0) {
		execv(arglist[0], arglist); // we'll never come back from this call
	} else {
		wait(NULL); // wait for child to die
	}
}

int spawnProcess(int in, int out, char** cmd) {
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
		return execv(cmd[0], cmd);
	}
	return pid;
}

// cleanup memory after each call. 
// Inefficient, but I never managed memory in C before, so it's fun
void cleanup(char** list) {
	for(int i = 0; list[i] != NULL; ++i) {
		free(list[i]);
	}
	free(list);
}

int main() {
	char buffer[1000]; // buffer which stores the entire command
	int bufferSize; // The size of buf
	char** arglist; // list of args to command
    char* command_saveptr; // for internal use of strtok_r. Use when tokenizing buf
    char** command; // The first word in buf
	char** commands;
	int n, trash, in, fd[2];

	char* partOfPipe;
	char* nextPartOfPipe;
	char* pipe_saveptr;


	//main loop
	for(;;) {
		printf(" >");
		fflush(stdout);
		bufferSize = read(0, buffer, 999);
		buffer[bufferSize-1] = 0;
		commands = formatCommands(buffer, "|", &n);
		in = fileno(stdin); // first input should be from stdin
		for(int i = 0; i < n - 1; ++i) {
			pipe(fd);
			command = formatCommand(commands[i]);
			spawnProcess(in, fd[1], command);
			close(fd[1]);
			in = fd[0];

		}
		if (in != 0) {
			dup2(in, fileno(stdin));
		}
		execv()

		forkAndExecv(arglist);
		cleanup(arglist);
	}
	return 0;
}


/*
 * In main loop:
 * 1. Get all sections of pipe
 * 2. for each piped section
 *		format the commnd for execv
 *		call spawnProcess
 *		cleanup command
 *		
 */
