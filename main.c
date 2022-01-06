#include "som.h"

int main(){
    vec_t * vecs;
    int * vec;
    int nb_vec, nb_dimension;
    srand((unsigned int)time(NULL));
    load_data(&vecs, "iris.data", &nb_vec, &nb_dimension, ",");
    normalize_vector(vecs, nb_vec, nb_dimension);
    init_random_vector(&vec, nb_vec);

    net_t network;
    network.nb_vecs = nb_vec;
    network.vec_size = nb_dimension;
    network.nb_row = 6;
    network.nb_column = 10;
    network_config(&network, vecs);
    save_network(&network, "net_before");

    training_network(vecs, vec, &network, nb_vec);

    print_and_save_result(vecs, &network);

    save_network(&network, "net_after");


    delete_all_bmu(&network);
    free_vectors(vecs, nb_vec);
    free_network(network);
    free(vec);
    return 1;
}