#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <wait.h>
#include <stdbool.h>


// turn a space string of words into an array of char*, each containing one of the words
char** listify(const char* _words, const char* symbol) {
	char** list = malloc(strlen(_words)); // always overshoot number of commands
	char words[800], *word, *word_saveptr;
	char arg[80];
	strcpy(words, _words);
	word = strtok_r(words, symbol, &word_saveptr);
	int i = 1; // leave 0th pointer empty for the initial command path
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
char* findInPath(char* cmd, char* path) {
	char dirInPath[100]; // individual directories in path
	char* fixedPath; // pointer to value of path as stored in the system
	char* pathTokenPtr; // pointer to section of path. Used when extracting directories from the path
	char* path_saveptr; // for internal use of strtok_r. Use when tokenizing path
	char* fullExe; // pointer to heap memory containing the full path of cmd
	DIR *d; // A directory stream??
	struct dirent *executableFile; // individual files in dir
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

// fork and execv. Child dies before this function returns
void execute(char* fullExe, char** arglist) {
	pid_t pid = fork ();
	if (pid<0) { // fork has failed 
		perror("fork"); exit(EXIT_FAILURE);
	}
	else if (pid == 0) {
		printf("%s\n",fullExe);
		fflush(stdout);
		arglist[0] = fullExe;
		execv(fullExe, arglist); // we'll never come back from this call
	} else {
		wait(NULL); // wait for child to die
	}
}

int main() {
	char buf[1000]; // buffer which stores the entire command
	char** arglist; // list of args to command
    char* cmd_saveptr; // for internal use of strtok_r. Use when tokenizing buf
    char* cmd; // The first word in buf
	int bufsize; // The size of buf

	char path[1000]; // buffer which stores the path
	char* fixedPath; // pointer to value of path as stored in the system
	char* fullExe;

	//main loop
	for(;;) {
		printf(" >");
		fflush(stdout);
		bufsize = read(0, buf, 999);
		buf[bufsize-1] = 0;
		// Get first word in command and store it in cmd
		cmd = strtok_r(buf, " ", &cmd_saveptr); // is _r necessary?
		arglist = listify(cmd_saveptr, " ");
		fixedPath = getenv("PATH"); // fixedPath shall not be changed
		strcpy(path, fixedPath);
		fullExe = findInPath(cmd, path);
		execute(fullExe, arglist);
	}
	return 0;
}


// Idea for piping:
// Have yet another strtok_r running, this time breaking on '|' symbol
// For each token, run this main execution loop.
// Somehow get the output of of execv (??) and then pipe to the next thing??
