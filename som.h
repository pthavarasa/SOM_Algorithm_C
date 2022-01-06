#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define INT_MAX 2147483647
#define MIN_GAP 0.002
#define MAX_GAP 0.005
#define NODE_COEF 5
#define ITER_COEF 500
#define NHD_COEF 0.5
#define NHD 8
#define ALFHA_INIT 0.7
#define FIRST_STAGE_DIV 5
#define SECOND_STAGE_DIV 10
#define ALLOCATION_FAIL "allocation failure!\n"
#define FILE_NOT_FOUND "File not found!\n"
#define EMPTY_BMU "something error, bmu = 0\n"

typedef struct vec{
    double * v;
    double norm;
    char * label;
}vec_t;

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
    int nb_vecs;
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

void fetch_iris_data(vec_t ** vectors, int * nb_vector);
void load_data(vec_t ** vectors, char * file_name, int * nb_vector, int * nb_dimension, char * separator);
void print_vectors(vec_t * vecs, int nb_vec, int dimension);
void free_vectors(vec_t * vecs, int nb_vec);
void normalize_vector(vec_t * vecs, int nb_vec, int dimension);
void init_random_vector(int ** vec, int nb_vec);
void shuffle_vector(int * vec, int nb_vec);
void add_bmu(net_t * net, int row, int column);
void delete_all_bmu(net_t * net);
int count_bmu(net_t * net);
bmu_t get_bmu(net_t * net);
vec_t avarage_vector(vec_t * vecs, int nb_vecs, int dimension);
int calculate_nb_nhd(int nb_node);
void init_network(net_t * config, vec_t * vecs);
void print_network(net_t config);
void network_config(net_t * config, vec_t * vecs);
void free_network(net_t net);
double euclidean_distance(double * vec, double * weight, int size);
void find_best_matching_unit(vec_t vec, net_t * net);
void alter_weight_by_distance(vec_t vec, net_t * net, bmu_t bmu, int iter);
void alter_weight_nhd(vec_t vec, net_t * net, bmu_t bmu, int iter, int nhd_dist);
int is_neighborhood(bmu_t bmu_pos, int weight_row, int weight_column, int nhd_dist);
void training_network(vec_t * vecs, int * shuf_vec, net_t * net, int nb_vecs);
void print_and_save_result(vec_t * vecs, net_t * network);
void save_network(net_t * net, char * file_name);