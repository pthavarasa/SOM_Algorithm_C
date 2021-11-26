#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct vec{
    double * v;
    int v_len;
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

    int count = 0, dimension = 4;
    int i;
    // reading data by pattern and making vector list
    while (fscanf(file, "%lf,%lf,%lf,%lf,%s", &a, &b, &c, &d, label) != EOF) {
        //printf("%lf,%lf,%lf,%lf,%s\n", a, b, c, d, label);
        // allocation
        // TODO check malloc if not null
        vecs[count].v = (double *)malloc(dimension * sizeof(double));
        vecs[count].label = (char *)malloc(sizeof(char));

        // copy data
        vecs[count].v[0] = a;
        vecs[count].v[1] = b;
        vecs[count].v[2] = c;
        vecs[count].v[3] = d;
        vecs[count].v_len = dimension;
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

void normalize_vector(vec_t * vecs, int nb_vec){
    int i, j;
    double max, min;
    max = vecs[0].v[0];
    min = vecs[0].v[0];
    for(i = 0; i < nb_vec; i++){
        for (int j = 0; j < vecs[i].v_len; j++){
            if(max < vecs[i].v[j]) max = vecs[i].v[j];
            if(min > vecs[i].v[j]) min = vecs[i].v[j];
        }
    }
    for (i = 0; i < nb_vec; i++){
        for (int j = 0; j < vecs[i].v_len; j++){
            vecs[i].v[j] = (vecs[i].v[j] - min) / (max - min);
        }
    }
}

void shuffle_vectors(vec_t * vecs, int nb_vec){
    int i;
    srand(time(NULL));
    int random;
    vec_t temp;
    for(i = 0; i < nb_vec; i++){
        random = rand() % nb_vec;
        temp = vecs[i];
        vecs[i] = vecs[random];
        vecs[random] = temp;
    }
}

int main(){
    vec_t * vecs;
    int nb_vec;
    fetch_iris_data(&vecs, &nb_vec);
    //print_vectors(vecs, nb_vec);
    normalize_vector(vecs, nb_vec);
    //print_vectors(vecs, nb_vec);
    shuffle_vectors(vecs, nb_vec);
    print_vectors(vecs, nb_vec);
    return 1;
}