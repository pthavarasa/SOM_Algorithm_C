#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct vec{
    double * v;
    char * label;
}vec_t;

void fetch_iris_data(vec_t ** vectors, int * nb_vector){
    // open file in read mode
    FILE *file = fopen("iris.data", "r");
    // checking if file exist
    if (file == NULL){
        printf("File not found!\n");
        return;
    }

    double a, b, c, d;
    char label[20];
    int lines = 1;

    // counting the number of lines in the file by finding '\n' count
    while(!feof(file))
        if(fgetc(file) == '\n')
            lines++;
    // set the file position to the beginning of the file
    rewind(file);
    //printf("lines : %d\n", lines);

    // allocate vector list
    vec_t * vecs = (vec_t *)malloc(lines * sizeof(vec_t));

    int count = 0;
    int i;
    // reading data by pattern and making vector list
    while (fscanf(file, "%lf,%lf,%lf,%lf,%s", &a, &b, &c, &d, label) != EOF) {
        //printf("%lf,%lf,%lf,%lf,%s\n", a, b, c, d, label);
        // allocation
        vecs[count].v = (double *)malloc(4 * sizeof(double));
        vecs[count].label = (char *)malloc(sizeof(char));

        // copy data
        vecs[count].v[0] = a;
        vecs[count].v[1] = b;
        vecs[count].v[2] = c;
        vecs[count].v[3] = d;
        strcpy(vecs[count].label, label);
        count++;
    }

    fclose(file);

    *vectors = vecs;
    *nb_vector = count;
}

void print_vectors(vec_t * vecs, int nb_vec){
    int i;
    for(i = 0; i < nb_vec; i++)
        printf("%lf,%lf,%lf,%lf,%s\n", vecs[i].v[0], vecs[i].v[1], vecs[i].v[2], vecs[i].v[3], vecs[i].label);
}

int main(){
    vec_t * vecs;
    int nb_vec;
    fetch_iris_data(&vecs, &nb_vec);
    print_vectors(vecs, nb_vec);
    return 1;
}