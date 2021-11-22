#include <stdio.h>
#include <stdlib.h>

int main(){
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    size_t read;

    fp = fopen("iris.data", "r");
    if (fp == NULL){
        printf("File not found!\n");
        return 0;
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        printf("Retrieved line of length %zu:\n", read);
        printf("%s", line);
    }

    fclose(fp);
    if (line) free(line);
    return 1;
}