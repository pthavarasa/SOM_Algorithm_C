// gcc data_reader.c -std=c99 -lm
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>

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

void free_vectors(vec_t * vecs, int nb_vec){
    int i;
    for(i = 0; i < nb_vec; i++)
        free(vecs[i].v);
    free(vecs);
}

void normalize_vector(vec_t * vecs, int nb_vec){
    int i, j;
    double max, min;
    max = vecs[0].v[0];
    min = vecs[0].v[0];
    // finding max value of vector
    for(i = 0; i < nb_vec; i++){
        for (int j = 0; j < vecs[i].v_len; j++){
            if(max < vecs[i].v[j]) max = vecs[i].v[j];
            if(min > vecs[i].v[j]) min = vecs[i].v[j];
        }
    }
    // normalize
    for (i = 0; i < nb_vec; i++){
        for (int j = 0; j < vecs[i].v_len; j++){
            vecs[i].v[j] = (vecs[i].v[j] - min) / (max - min);
        }
    }
}

void shuffle_vectors(vec_t * vecs, int nb_vec){
    int i;
    int random;
    vec_t temp;
    for(i = 0; i < nb_vec; i++){
        random = rand() % nb_vec;
        temp = vecs[i];
        vecs[i] = vecs[random];
        vecs[random] = temp;
    }
}

typedef struct node{
    double * w;
    double activation;
    char * label;
}node_t;

typedef struct bmu bmu_t;
struct bmu{
    int row, column;
    bmu_t * next;
};

typedef struct net{
    int nb_node;
    int nb_row;
    int nb_column;
    int nb_iter;
    double alpha;
    int nb_nhd; //neighborhood
    int vec_size;
    node_t * map;
    bmu_t * bmu; // best matching unit
}net_t;

void add_bmu(net_t * net, int row, int column){
    if(net->bmu == NULL){
        net->bmu = (bmu_t*)malloc(sizeof(bmu_t));
        net->bmu->row = row;
        net->bmu->column = column;
        net->bmu->next = NULL;
    }else{
        bmu_t * ptr = net->bmu;
        while(ptr->next != NULL){
            ptr = ptr->next;
        }
        ptr->next = (bmu_t*)malloc(sizeof(bmu_t));
        ptr->next->row = row;
        ptr->next->column = column;
        ptr->next->next = NULL;
    }
}

void delete_all_bmu(net_t * net){
    if(net->bmu == NULL) return;
    bmu_t * ptr = net->bmu;
    bmu_t * delete;
    while(ptr != NULL){
        delete = ptr;
        ptr = ptr->next;
        free(delete);
    }
    net->bmu = NULL;
}

int count_bmu(net_t * net){
    bmu_t * ptr = net->bmu;
    int count = 0;
    while(ptr != NULL){
        count++;
        ptr = ptr->next;
    }
    return count;
}

bmu_t get_bmu(net_t * net){
    int count = count_bmu(net);
    bmu_t bmu;
    if(count == 0){
        printf("something error, bmu = 0\n");
        exit(EXIT_FAILURE);
    }
    if(count == 1){
        bmu.row = net->bmu->row;
        bmu.column = net->bmu->column;
        return bmu;
    }
    int random = rand() % count;
    bmu_t * ptr = net->bmu;
    while (random != 0){
        random--;
        ptr = ptr->next;
    }
    bmu.row = ptr->row;
    bmu.column = ptr->column;
    return bmu;
}

void init_network(net_t * config){
    config->map = (node_t *)malloc(config->nb_row * config->nb_column * sizeof(node_t));
    int i, j;
    for(i = 0; i < config->nb_row * config->nb_column; i++){
        //printf("%lf %d \n", (double)rand() / (double)RAND_MAX, i);
        config->map[i].w = (double *)malloc(config->vec_size * sizeof(double));
        assert(config->map[i].w != NULL);
        for(j = 0; j < config->vec_size; j++)
            config->map[i].w[j] = (double)rand() / (double)RAND_MAX, i;
    }
    //printf("%lf\n", weight[0].w[0]);
}

void print_network(net_t config){
    int i, j;
    for(i = 0; i < config.nb_row * config.nb_column; i++){
        if(i%config.nb_column == 0) printf("\n");
        for(j = 0; j < config.vec_size; j++){
            printf("%lf  ", config.map[i].w[j]);
        }
        printf("\n");
    }
}

void network_config(net_t * config, int nb_vec, int vec_size){
    // 5 * sqrt(nb_vectors)
    config->nb_node = 5 * (int)sqrt(nb_vec);
    config->nb_row = 6;
    config->nb_column = 10;
    // 500 * nb_vectors | 1st => 188 ( nb_iter/4 ) | 2nd => 562 ( nb_iter - (nb_iter/4) )
    config->nb_iter = 500 * vec_size;
    // 50% of nb_node => nb_node * 0.5 => 60 * 0.5 = 30
    // 8*n+1 => (8*0+1) + (8*1+1) + (8*2+1) + (8*3+1) = 49 (49>30) => n=3
    config->nb_nhd = 3;
    config->alpha = 0.7;
    config->vec_size = 4;
    init_network(config);
    //printf("%lf\n", config->map[0].w[0]);
    config->bmu = NULL;
}

void free_network(net_t net){
    int i,j;
    for(i = 0; i < net.nb_row * net.nb_column; i++){
        free(net.map[i].w);
    }
    free(net.map);
}

double euclidean_distance(double * vec, double * weight, int size){
    int i;
    double sum = 0.0;
    for(i = 0; i < size; i++){
        sum += (weight[i] - vec[i]) * (weight[i] - vec[i]);
    }
    return sqrt(sum);
}


void find_best_matching_unit(vec_t vec, net_t * net){
    int i, j;
    double min_dist = 1<<20, dist;
    for(i = 0; i < net->nb_row; i++){
        for(j = 0; j < net->nb_column; j++){
            dist = euclidean_distance(vec.v, net->map[i * net->nb_row + j].w, net->vec_size);
            if(dist < min_dist){
                min_dist = dist;
                delete_all_bmu(net);
                add_bmu(net, i, j);
            }else if(dist < min_dist){
                add_bmu(net, i, j);
            }
        }
    }
}

void alter_weight(vec_t vec, net_t * net, bmu_t bmu, int iter){
    double alpha;
    if(iter > net->nb_iter/4)
        alpha = net->alpha/10;
    else
        alpha = net->alpha;
    alpha = alpha * (1 - iter / net->nb_iter);
    double * weight;
    weight = net->map[bmu.row * net->nb_row + bmu.column].w;
    int i;
    for(i = 0; i < net->vec_size; i++){
        weight[i] = weight[i] + alpha * (vec.v[i] - weight[i]);
    }
}

int is_neighborhood(bmu_t bmu_pos, int weight_row, int weight_column, int nhd_dist){
    if(sqrt((bmu_pos.row - weight_row) * (bmu_pos.row - weight_row)) <= nhd_dist &&
        sqrt((bmu_pos.column - weight_column) * (bmu_pos.column - weight_column)) <= nhd_dist) return 1;
    return 0;
}

void training_network(vec_t * vecs, net_t * net, int nb_vecs){
    int i, j, iter, nhd_dist;
    vec_t vec;
    bmu_t pos;
    bmu_t bmu;
    for(iter = 0; iter < net->nb_iter; iter++){
        vec = vecs[iter%nb_vecs];
        find_best_matching_unit(vec, net);
        bmu = get_bmu(net);
        for(i = 0; i < net->nb_row; i++){
            for(j = 0; j < net->nb_column; j++){
                if(iter > net->nb_iter/4)
                    nhd_dist = 1;
                else
                    nhd_dist = 3;
                pos.row = i;
                pos.column = j;
                if(is_neighborhood(bmu, i, j, nhd_dist))
                    alter_weight(vec, net, pos, iter);
            }
        }
    }
}

int main(){
    vec_t * vecs;
    int nb_vec;
    srand(time(NULL));
    fetch_iris_data(&vecs, &nb_vec);
    //print_vectors(vecs, nb_vec);
    normalize_vector(vecs, nb_vec);
    //print_vectors(vecs, nb_vec);
    shuffle_vectors(vecs, nb_vec);
    //print_vectors(vecs, nb_vec);

    net_t network;
    network_config(&network, nb_vec, 4);
    //printf("%d\n", network.nb_iter);
    //printf("%lf\n", network.map[0].w[0]);

    print_network(network);
    //add_bmu(&network, 1, 4);
    //add_bmu(&network, 2, 4);
    //printf("%d\n", get_bmu(&network).row);
    //printf("%d\n", count_bmu(&network));

    //printf("%lf\n", euclidean_distance(vecs[0].v, network.map[0].w, 4));

    //find_best_matching_unit(vecs[0], &network);
    //printf("%d\n", get_bmu(&network).row);
/*
    bmu_t b;
    b.row = 4;
    b.column = 4;
    print_network(network);
    printf("------------------");
    alter_weight(vecs[0], &network, b, 0);
    print_network(network);*/

    //printf("%d\n", is_neighborhood(b, 2,3,1));

    printf("-------------------------------------------");
    training_network(vecs, &network, nb_vec);
    print_network(network);





    free_vectors(vecs, nb_vec);
    free_network(network);
    return 1;
}