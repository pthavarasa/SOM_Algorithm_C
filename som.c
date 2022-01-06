#include "som.h"

void allocation_failure_handle(){
    printf(ALLOCATION_FAIL);
    exit(EXIT_FAILURE);
}

void load_data(vec_t ** vectors, char * file_name, int * nb_vector, int * nb_dimension, char * separator){
    // open file in read mode
    FILE *file = fopen(file_name, "r");
    // checking if file exist
    if (file == NULL){
        printf(FILE_NOT_FOUND);
        exit(EXIT_FAILURE);
    }

    int lines = 1;
    // counting the number of lines in the file by finding '\n' count
    while(!feof(file))
        if(fgetc(file) == '\n')
            lines++;
    // set the file position to the beginning of the file
    rewind(file);

    // allocate vector list
    vec_t * vecs = (vec_t *)malloc((size_t)lines * (int)sizeof(vec_t));
    if(vecs == NULL) allocation_failure_handle();

    char *line = NULL;
    size_t len = 0;
    char * strToken;
    int dimension = 0, length = 0, count;
    while(getline(&line, &len, file) != -1) {
        //printf("line length: %zd\n", strlen(line));
        //printf ("%c", line[strlen(line)-1]);
        if(line[strlen(line)-1] == '\n') line[strlen(line)-1] = '\0';
        if(!strlen(line)) continue;
        if(!length){
            char *ptr = line;
            while((ptr = strchr(ptr, separator[0])) != NULL) {
                dimension++;
                ptr++;
            }
            //printf("%d\n", dimension);
        }

        vecs[length].v = (double *)malloc((size_t)dimension * (int)sizeof(double));
        if(vecs[length].v == NULL) allocation_failure_handle();

        strToken = strtok (line, separator);
        count = 0;
        while (strToken != NULL){
            //printf("%s", strToken);
            if(count == dimension){
                vecs[length].label = (char *)malloc((size_t)strlen(strToken) * (int)sizeof(char));
                if(vecs[length].label == NULL) allocation_failure_handle();
                strcpy(vecs[length].label, strToken);
            }else{
                vecs[length].v[count] = atof(strToken);
            }
            strToken = strtok(NULL, separator);
            count++;
        }
        length++;
    }

    fclose(file);

    *vectors = vecs;
    *nb_vector = length;
    *nb_dimension = dimension;
}

void print_vectors(vec_t * vecs, int nb_vec, int dimension){
    int i,j;
    for(i = 0; i < nb_vec; i++){
        for(j = 0; j < dimension; j++)
            printf("%lf, ", vecs[i].v[j]);
        printf("%s\n, ", vecs[i].label);
    }
}

void free_vectors(vec_t * vecs, int nb_vec){
    int i;
    for(i = 0; i < nb_vec; i++)
        free(vecs[i].v);
    free(vecs);
}

void init_random_vector(int ** vec, int nb_vec){
    int i;
    int * v = (int *)malloc((size_t)nb_vec * (int)sizeof(int));
    if(v == NULL) allocation_failure_handle();
    for(i = 0; i < nb_vec; i++)
        v[i] = i;
    *vec = v;
}

void shuffle_vector(int * vec, int nb_vec){
    int i;
    int random, tmp;
    for(i = 0; i < nb_vec; i++){
        random = rand() % nb_vec;
        tmp = vec[i];
        vec[i] = vec[random];
        vec[random] = tmp;
    }
}

void normalize_vector(vec_t * vecs, int nb_vec, int dimension){
    int i,j;
    double sum;
    for(i = 0; i < nb_vec; i++){
        sum = 0.0;
        for (j = 0; j < dimension; j++)
            sum += vecs[i].v[j] * vecs[i].v[j];
        vecs[i].norm = sqrt(sum);
    }
    for (i = 0; i < nb_vec; i++){
        for (j = 0; j < dimension; j++)
            vecs[i].v[j] = vecs[i].v[j] / vecs[i].norm;
    }
}

void add_bmu(net_t * net, int row, int column){
    if(net->bmu == NULL){
        net->bmu = (bmu_t*)malloc(sizeof(bmu_t));
        if(net->bmu == NULL) allocation_failure_handle();
        net->bmu->row = row;
        net->bmu->column = column;
        net->bmu->next = NULL;
    }else{
        bmu_t * ptr = net->bmu;
        while(ptr->next != NULL){
            ptr = ptr->next;
        }
        ptr->next = (bmu_t*)malloc(sizeof(bmu_t));
        if(ptr->next == NULL) allocation_failure_handle();
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
        printf(EMPTY_BMU);
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

vec_t avarage_vector(vec_t * vecs, int nb_vecs, int dimension){
    int i,j;
    vec_t avg_vec;
    avg_vec.v = (double *)malloc((size_t)dimension * (int)sizeof(double));
    if(avg_vec.v == NULL) allocation_failure_handle();
    for(j = 0; j < dimension; j++)
        avg_vec.v[j] = 0;
    for(i = 0; i < nb_vecs; i++)
        for(j = 0; j < dimension; j++)
            avg_vec.v[j] += vecs[i].v[j];
    for(j = 0; j < dimension; j++)
        avg_vec.v[j] /= nb_vecs;
    return avg_vec;
}

void init_network(net_t * config, vec_t * vecs){
    config->map = (node_t *)malloc((size_t)config->nb_row * (size_t)config->nb_column * (int)sizeof(node_t));
    if(config->map == NULL) allocation_failure_handle();
    int i, j;
    vec_t avg_vec = avarage_vector(vecs, config->nb_vecs, config->vec_size);
    vec_t min_vec, max_vec;
    min_vec.v = (double *)malloc((size_t)config->vec_size * (int)sizeof(double));
    if(min_vec.v == NULL) allocation_failure_handle();
    for(j = 0; j < config->vec_size; j++)
        min_vec.v[j] = avg_vec.v[j] - MIN_GAP;

    max_vec.v = (double *)malloc((size_t)config->vec_size * (int)sizeof(double));
    if(max_vec.v == NULL) allocation_failure_handle();
    for(j = 0; j < config->vec_size; j++)
        max_vec.v[j] = avg_vec.v[j] + MAX_GAP;

    for(i = 0; i < config->nb_row * config->nb_column; i++){
        config->map[i].w = (double *)malloc((size_t)config->vec_size * (int)sizeof(double));
        if(config->map[i].w == NULL) allocation_failure_handle();
        for(j = 0; j < config->vec_size; j++)
            config->map[i].w[j] = min_vec.v[j] + fmod((double)rand(), (max_vec.v[j] - min_vec.v[j]));
    }

    for(i = 0; i < config->nb_row * config->nb_column; i++){
        config->map[i].label = NULL;
    }
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

int calculate_nb_nhd(int nb_node){
    nb_node = (int)(nb_node * NHD_COEF);
    int nhd = NHD, count = 0, sum = 0;
    while((sum += nhd * count) + 1 < nb_node)count++;
    return count;
}

void network_config(net_t * config, vec_t * vecs){
    // nb_node_coefficient * sqrt(nb_vectors)
    config->nb_node = NODE_COEF * (int)sqrt(config->nb_vecs);
    if(!(config->nb_node == config->nb_row * config->nb_column)){
        printf("column * row = %d and node_coef * sqrt(nb_vecs) = %d\n (it must be equal)", config->nb_row * config->nb_column, config->nb_node);
        exit(EXIT_FAILURE);
    }
    // 500 * nb_vectors | 1st => ( nb_iter/4 ) | 2nd => ( nb_iter - (nb_iter/4) )
    config->nb_iter = ITER_COEF * config->nb_vecs;
    config->alpha = ALFHA_INIT;
    // 50% of nb_node => nb_node * 0.5 => 60 * 0.5 = 30
    // 8*n+1 => (8*0) + (8*1) + (8*2) + (8*3) + 1 = 49 (49>30) => n=3
    config->nb_nhd = calculate_nb_nhd(config->nb_node);
    init_network(config, vecs);
    config->bmu = NULL;
}

void free_network(net_t net){
    int i;
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
    double min_dist = INT_MAX, dist;
    for(i = 0; i < net->nb_row; i++){
        for(j = 0; j < net->nb_column; j++){
            dist = euclidean_distance(vec.v, net->map[i * net->nb_column + j].w, net->vec_size);
            if(dist < min_dist){
                min_dist = dist;
                delete_all_bmu(net);
                add_bmu(net, i, j);
            }else if(dist == min_dist){
                add_bmu(net, i, j);
            }
        }
    }
}

void alter_weight_nhd(vec_t vec, net_t * net, bmu_t bmu, int iter, int nhd_dist){
    int x,y, row, column;
    double alpha;
    for(x = -nhd_dist; x <= nhd_dist; x++){
        for(y = -nhd_dist; y <= nhd_dist; y++){
            row = bmu.row + x;
            column = bmu.column + y;
            if(row >= 0 && row < net->nb_row && column >= 0 && column < net->nb_column){
                if(iter > net->nb_iter/FIRST_STAGE_DIV)
                    alpha = net->alpha/SECOND_STAGE_DIV;
                else
                    alpha = net->alpha;
                alpha = alpha * (1 - iter / net->nb_iter);
                double * weight;
                weight = net->map[row * net->nb_column + column].w;
                int i;
                for(i = 0; i < net->vec_size; i++){
                    weight[i] = weight[i] + alpha * (vec.v[i] - weight[i]);
                }
            }
        }
    }
}

void training_network(vec_t * vecs, int * shuf_vec, net_t * net, int nb_vecs){
    int iter, nhd_dist;
    vec_t vec;
    bmu_t bmu;
    for(iter = 0; iter < net->nb_iter; iter++){
        if(iter % nb_vecs == 0) shuffle_vector(shuf_vec, nb_vecs);
        vec = vecs[shuf_vec[iter%nb_vecs]];
        find_best_matching_unit(vec, net);
        bmu = get_bmu(net);

        if(iter <= net->nb_iter/FIRST_STAGE_DIV){
            nhd_dist = (int)(net->nb_nhd - iter/((net->nb_iter/FIRST_STAGE_DIV)/net->nb_nhd));
            if(!nhd_dist) nhd_dist = 1;
        }else
            nhd_dist = 1;
        alter_weight_nhd(vec, net, bmu, iter, nhd_dist);
    }
}


void print_and_save_result(vec_t * vecs, net_t * network){
    int i,j;
    bmu_t bmu;

    FILE *fptr;
    fptr = fopen("map","w");
    if(fptr == NULL){
        printf(FILE_NOT_FOUND);
        exit(EXIT_FAILURE);
    }
    fprintf(fptr,"%d\n%d\n%d\n", network->nb_row, network->nb_column, network->vec_size);

    // get all unique label from dataset
    int nb_label_distinct = 0;
    char ** labels;
    int comp;
    for(i = 0; i < network->nb_vecs; i++){
        comp = 0;
        for(j = 0; j < nb_label_distinct; j++){
            if(!strcmp(vecs[i].label, labels[j])){
                comp = 1;
            }
        }
        if(!comp){
            nb_label_distinct++;

            if(nb_label_distinct == 1)
                labels = (char**)malloc((size_t)nb_label_distinct * sizeof(char**));
            else
                labels = (char**)realloc(labels, (size_t)nb_label_distinct * sizeof(char**));

            labels[j] = vecs[i].label;
        }
    }

    int * vec;
    init_random_vector(&vec, network->nb_vecs);
    shuffle_vector(vec, network->nb_vecs);
    for(i = 0; i < network->nb_vecs; i++){
        find_best_matching_unit(vecs[vec[i]], network);
        bmu = get_bmu(network);
        network->map[bmu.row * network->nb_column + bmu.column].label = vecs[vec[i]].label;
    }
    free(vec);
    
    for(i = 0; i < network->nb_node; i++){
        if(i%network->nb_column == 0)printf("\n");
        for(j = 0; j < nb_label_distinct; j++)
            if(network->map[i].label != NULL){
                if(!strcmp(network->map[i].label, labels[j])){
                    printf("%d", j+1);
                    fprintf(fptr,"%d,", j+1);
                }
            }else{
                printf("%d", 0);
                fprintf(fptr,"%d,", 0);
                break;
            }
    }
    printf("\n\n");

    for(j = 0; j < nb_label_distinct; j++)
        printf("%d = %s\n", j+1, labels[j]);
    
    fclose(fptr);
    free(labels);
}

void save_network(net_t * net, char * file_name){
    FILE *fptr;

    fptr = fopen(file_name,"w");

    if(fptr == NULL) allocation_failure_handle();

    int i, j;
    fprintf(fptr,"%d\n%d\n%d\n", net->nb_row, net->nb_column, net->vec_size);
    for(i = 0; i < net->nb_row * net->nb_column; i++){
        for(j = 0; j < net->vec_size; j++){
            fprintf(fptr,"%lf,",net->map[i].w[j]);
        }
    }

    fclose(fptr);
}