#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <wait.h>

// turn a space string of words into an array of char*, each containing one of the words
char** listify(const char* _words) {
	char** list = malloc(strlen(_words)); // always overshoot number of commands
	char words[800], *word, *word_saveptr;
	char arg[80];
	strcpy(words, _words);
	word = strtok_r(words, " ", &word_saveptr);
	int i = 1; // leave 0th pointer empty for the initial command path
	for(; word != NULL; ++i) {
		int size = strlen(word) + 1;
		list[i] = malloc(size);
		strcpy(list[i], word);
		word = strtok_r(NULL, " ", &word_saveptr);
	}
	list[i] = NULL; // end the list with a NULL
	return list;
}

int main() {
	char buf[1000]; // buffer which stores the entire command 
	char** arglist; // list of args to command
    char* cmd_saveptr; // for internal use of strtok_r. Use when tokenizing buf
    char* cmd; // The first word in buf
	int bufsize; // The size of buf
	char path[1000]; // buffer which stores the path
	char pathdir[100]; // individual directories in path
	char* fixedPath; // pointer to value of path as stored in the system
	char* pathTokenPtr; // pointer to section of path. Used when extracting directories from the path
	char* path_saveptr; // for internal use of strtok_r. Use when tokenizing path
	DIR *d; // A directory stream??
	struct dirent *executable; // individual files in dir
	for(;;) {
		printf("     ☮ >");
		fflush(stdout);
		bufsize = read(0, buf, 999);
		buf[bufsize-1] = 0;
		// Get first word in command and store it in cmd
		// Subsequent words will be retrieved with strtok_r(NULL, " ", $cmd_saveptr);
		cmd = strtok_r(buf, " ", &cmd_saveptr); // _r important
		// turn buf into an array of char* each pointing to an argument
		arglist = listify(cmd_saveptr);
		// fixedPath shall not be changed
		fixedPath = getenv("PATH");
		strcpy(path, fixedPath);
		// subsequent call will be strtok_r(NULL, ":", &path_saveptr);
		pathTokenPtr = strtok_r(path, ":", &path_saveptr);
		fflush(stdout);
		if(pathTokenPtr != NULL)
			strcpy(pathdir,pathTokenPtr);
		else
			continue;
		for(;;) {
			printf("Looking for requested command in %s\n", pathdir);
			d = opendir(pathdir);
			if (d) {
				while ((executable = readdir(d)) != NULL) {
					if(strcmp(executable->d_name, cmd) == 0) {
						
						pid_t pid = fork ();
						if (pid<0) { // fork has failed 
							perror("fork"); exit(EXIT_FAILURE);
						}
						else if (pid == 0) {
							printf("In child pid = %i\n", getpid());
							printf("Found command in %s\n", pathdir);
							strcat(pathdir, "/");
							strcat(pathdir, cmd);
							printf("%s\n",pathdir);
							arglist[0] = pathdir;
							fflush(stdout);
							char* const arglist2[] = {"/usr/bin/ls", "-l", "-a", NULL};
							execv(pathdir, arglist);
							printf("Uh Oh. Should never be here\n");
						} else {
							printf("In parent pid = %i\n", getpid());
							wait(NULL);
						}
					}
				}
				closedir(d);
			}
			pathTokenPtr = strtok_r(NULL, ":", &path_saveptr);
			if(pathTokenPtr != NULL) // if more directories in path
				strcpy(pathdir,pathTokenPtr); // copy next directory into pathdir
			else
				break;
		}
	}
	return 0;
}

