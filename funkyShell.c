#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


int main() {
    char buf[1000];
    int size;
    char* path;
    char pathToken[100];
    char* pathTokenPtr;

    for(;;) {
        printf(">");
        fflush(stdout);
        size = read(0, buf, 999);
        buf[size-1] = 0;
        printf("%s\n", buf);
        path = getenv("PATH");
        pathTokenPtr = strtok(path, ":");
        if(pathTokenPtr != NULL)
            strcpy(pathToken,pathTokenPtr);
        else 
            continue;
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

