#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>


int main() {
	char buf[1000];
    char* bufPtr;
    char* cmd;
	int size;
	char* fixedPath;
	char path[1000];
	char pathToken[100];
	char* pathTokenPtr;
	DIR *d;
	struct dirent *dir;

	for(;;) {
		printf("     ☮ >");
		fflush(stdout);
		size = read(0, buf, 999);
		buf[size] = 0;
        cmd = strtok_r(buf, " ", &bufPtr); // _r important
		printf("%s", buf);
		fixedPath = getenv("PATH");
		strcpy(path, fixedPath);
		pathTokenPtr = strtok(path, ":");
		fflush(stdout);
		if(pathTokenPtr != NULL)
			strcpy(pathToken,pathTokenPtr);
		else
			continue;
		d = opendir(".");
		if (d) {
			while ((dir = readdir(d)) != NULL) {
				printf("%s\n", dir->d_name);
			}
			closedir(d);
		}
		for(;;) {
			printf("%s\n", pathToken);
			pathTokenPtr = strtok(NULL, ":");
			if(pathTokenPtr != NULL)
				strcpy(pathToken,pathTokenPtr);
			else
				break;
		}
	}
	return 0;
}

