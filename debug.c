#include "som.h"

int main(){
    vec_t * vecs;
    int * vec;
    int nb_vec, nb_dimension;
    srand((unsigned int)time(NULL));
    load_data(&vecs, &nb_vec, &nb_dimension, ",");
 
    //fetch_iris_data(&vecs, &nb_vec);
    //print_vectors(vecs, nb_vec);
    normalize_vector(vecs, nb_vec, 4);
    //print_vectors(vecs, nb_vec);
    //printf("%lf\n", avarage_vector(vecs, nb_vec, 4));
    init_random_vector(&vec, nb_vec);
    shuffle_vector(vec, nb_vec);

    net_t network;
    network_config(&network, nb_vec, 4, avarage_vector(vecs, nb_vec, 4));
    //printf("%d\n", network.nb_iter);
    //printf("%lf\n", network.map[0].w[0]);

    //print_network(network);
    //add_bmu(&network, 1, 4);
    //add_bmu(&network, 2, 4);
    //printf("%d\n", get_bmu(&network).row);
    //printf("%d\n", count_bmu(&network));

    //printf("%lf\n", euclidean_distance(vecs[0].v, network.map[0].w, 4));

    //find_best_matching_unit(vecs[0], &network);
    //printf("%d\n", get_bmu(&network).row);

/*
    bmu_t b;
    b.row = 3;
    b.column = 3;
    //print_network(network);
    printf("------------------");
    alter_weight_nhd(vecs[0], &network, b, 0, 1);
    //print_network(network);
*/
    //printf("%d\n", is_neighborhood(b, 2,3,1));
    //save_network(&network);
    //print_network(network);
    //printf("-------------------------------------------");
    training_network(vecs, vec, &network, nb_vec);
    //print_network(network);
    //save_network(&network);

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