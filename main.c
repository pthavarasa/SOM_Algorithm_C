#include "som.h"

int main(){
    vec_t * vecs;
    int * vec;
    int nb_vec;
    srand((unsigned int)time(NULL));
    fetch_iris_data(&vecs, &nb_vec);
    normalize_vector(vecs, nb_vec, 4);
    shuffle_vectors(&vec, nb_vec);

    net_t network;
    network_config(&network, nb_vec, 4, avarage_vector(vecs, nb_vec, 4));
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
        if(!strcmp(vecs[vec[i]].label, "Iris-setosa"))
            v[bmu.row * 10 + bmu.column] = 1;
        else if(!strcmp(vecs[vec[i]].label, "Iris-versicolor"))
            v[bmu.row * 10 + bmu.column] = 2;
        else if(!strcmp(vecs[vec[i]].label, "Iris-virginica"))
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