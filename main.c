#include "som.h"

int main(){
    vec_t * vecs;
    int * vec;
    int nb_vec, nb_dimension;
    srand((unsigned int)time(NULL));
    load_data(&vecs, "iris.data", &nb_vec, &nb_dimension, ",");
    normalize_vector(vecs, nb_vec, 4);
    init_random_vector(&vec, nb_vec);

    net_t network;
    network.nb_vecs = nb_vec;
    network.vec_size = nb_dimension;
    network.nb_row = 6;
    network.nb_column = 10;
    network_config(&network, vecs);

    training_network(vecs, vec, &network, nb_vec);

    int v[60];
    int i;
    bmu_t bmu;
    for(i = 0; i < 60; i++){
        v[i] = 0;
    }
    for(i = 0; i < nb_vec; i++){
        find_best_matching_unit(vecs[i], &network);
        bmu = get_bmu(&network);
        if(!strcmp(vecs[i].label, "Iris-setosa"))
            v[bmu.row * 10 + bmu.column] = 1;
        else if(!strcmp(vecs[i].label, "Iris-versicolor"))
            v[bmu.row * 10 + bmu.column] = 2;
        else if(!strcmp(vecs[i].label, "Iris-virginica"))
            v[bmu.row * 10 + bmu.column] = 3;
    }
    for(i = 0; i < 60; i++){
        if(i%10 == 0)printf("\n");
        printf("%d", v[i]);
    }

    delete_all_bmu(&network);
    free_vectors(vecs, nb_vec);
    free_network(network);
    return 1;
}