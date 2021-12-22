#include "som.h"

void allocation_failure_handle(){
    printf("allocation failure!\n");
    exit(EXIT_FAILURE);
}

void fetch_iris_data(vec_t ** vectors, int * nb_vector){
    // open file in read mode
    FILE *file = fopen("iris.data", "r");
    // checking if file exist
    if (file == NULL){
        printf("File not found!\n");
        exit(0);
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

    // allocate vector list
    vec_t * vecs = (vec_t *)malloc((size_t)lines * (int)sizeof(vec_t));
    if(vecs == NULL) allocation_failure_handle();

    int count = 0, dimension = 4;
    // reading data by pattern and making vector list
    while (fscanf(file, "%lf,%lf,%lf,%lf,%s", &a, &b, &c, &d, label) != EOF) {

        // allocation
        vecs[count].v = (double *)malloc((size_t)dimension * (int)sizeof(double));
        if(vecs[count].v == NULL) allocation_failure_handle();
        vecs[count].label = (char *)malloc((int)sizeof(char));
        if(vecs[count].label == NULL) allocation_failure_handle();

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

void load_data(vec_t ** vectors, int * nb_vector, int * nb_dimension){
    // open file in read mode
    FILE *file = fopen("iris.data", "r");
    // checking if file exist
    if (file == NULL){
        printf("File not found!\n");
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
    const char * separator = ",";
    int dimension = 0, length = 0, count;
    while(getline(&line, &len, file) != -1) {
        //printf("line length: %zd\n", strlen(line));
        //printf ("%c", line[strlen(line)-1]);
        if(line[strlen(line)-1] == '\n') line[strlen(line)-1] = '\0';
        if(!strlen(line)) continue;
        if(!length){
            char *ptr = line;
            while((ptr = strchr(ptr, ',')) != NULL) {
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

void shuffle_vectors(int ** vec, int nb_vec){
    int i;
    int random, tmp;
    int * v = (int *)malloc((size_t)nb_vec * (int)sizeof(int));
    if(v == NULL) allocation_failure_handle();
    for(i = 0; i < nb_vec; i++)
        v[i] = i;
    for(i = 0; i < nb_vec; i++){
        random = rand() % nb_vec;
        tmp = v[i];
        v[i] = v[random];
        v[random] = tmp;
    }
    *vec = v;
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

double avarage_vector(vec_t * vecs, int nb_vecs, int dimension){
    int i,j;
    double sum = 0;
    for(i = 0; i < nb_vecs; i++)
        for(j = 0; j < dimension; j++)
            sum += vecs[i].v[j];
    return sum / (nb_vecs * dimension);
}

void init_network(net_t * config, double vec_avg){
    config->map = (node_t *)malloc((size_t)config->nb_row * (size_t)config->nb_column * (int)sizeof(node_t));
    if(config->map == NULL) allocation_failure_handle();
    int i, j;
    double min = vec_avg - 0.002, max = vec_avg + 0.005;
    for(i = 0; i < config->nb_row * config->nb_column; i++){
        config->map[i].w = (double *)malloc((size_t)config->vec_size * (int)sizeof(double));
        if(config->map[i].w == NULL) allocation_failure_handle();
        for(j = 0; j < config->vec_size; j++)
            config->map[i].w[j] = min + fmod((double)rand(), (max - min));
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

void network_config(net_t * config, int nb_vec, int vec_size, double vec_avg){
    // 5 * sqrt(nb_vectors)
    config->nb_node = 5 * (int)sqrt(nb_vec);
    config->nb_row = 6;
    config->nb_column = 10;
    // 500 * nb_vectors | 1st => ( nb_iter/4 ) | 2nd => ( nb_iter - (nb_iter/4) )
    config->nb_iter = 500 * nb_vec;
    // 50% of nb_node => nb_node * 0.5 => 60 * 0.5 = 30
    // 8*n+1 => (8*0+1) + (8*1+1) + (8*2+1) + (8*3+1) = 49 (49>30) => n=3
    config->nb_nhd = 3;
    config->alpha = 0.7;
    config->vec_size = 4;
    init_network(config, vec_avg);
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
    double min_dist = 1<<20, dist;
    for(i = 0; i < net->nb_row; i++){
        for(j = 0; j < net->nb_column; j++){
            dist = euclidean_distance(vec.v, net->map[i * net->nb_column + j].w, net->vec_size);
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

void alter_weight_by_distance(vec_t vec, net_t * net, bmu_t bmu, int iter){
    double alpha;
    if(iter > net->nb_iter/5)
        alpha = net->alpha/10;
    else
        alpha = net->alpha;
    alpha = alpha * (1 - iter / net->nb_iter);
    double * weight;
    weight = net->map[bmu.row * net->nb_column + bmu.column].w;
    int i;
    for(i = 0; i < net->vec_size; i++){
        weight[i] = weight[i] + alpha * (vec.v[i] - weight[i]);
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
                if(iter > net->nb_iter/5)
                    alpha = net->alpha/10;
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

int is_neighborhood(bmu_t bmu_pos, int weight_row, int weight_column, int nhd_dist){
    if(sqrt((bmu_pos.row - weight_row) * (bmu_pos.row - weight_row)) <= nhd_dist &&
        sqrt((bmu_pos.column - weight_column) * (bmu_pos.column - weight_column)) <= nhd_dist) return 1;
    return 0;
}

void training_network(vec_t * vecs, int * shuf_vec, net_t * net, int nb_vecs){
    int iter, nhd_dist;
    vec_t vec;
    bmu_t bmu;
    for(iter = 0; iter < net->nb_iter; iter++){
        vec = vecs[shuf_vec[iter%nb_vecs]];
        find_best_matching_unit(vec, net);
        bmu = get_bmu(net);

        if(iter <= net->nb_iter/5){
            nhd_dist = 3;
            nhd_dist = (int)(nhd_dist - iter/((net->nb_iter/5)/nhd_dist));
            if(!nhd_dist) nhd_dist = 1;
        }else
            nhd_dist = 1;
        alter_weight_nhd(vec, net, bmu, iter, nhd_dist);
        
        /*
        int i,j;
        bmu_t pos;
        for(i = 0; i < net->nb_row; i++){
            for(j = 0; j < net->nb_column; j++){
                if(iter <= net->nb_iter/5){
                    nhd_dist = 3;
                    nhd_dist = (int)(nhd_dist - iter/((net->nb_iter/5)/nhd_dist));
                    if(!nhd_dist) nhd_dist = 1;
                }else
                    nhd_dist = 1;
                pos.row = i;
                pos.column = j;
                if(is_neighborhood(bmu, i, j, nhd_dist))
                    alter_weight(vec, net, pos, iter);
            }
        }
        */
    }
}

void save_network(net_t * net){
    FILE *fptr;

    // use appropriate location if you are using MacOS or Linux
    fptr = fopen("net.txt","w");

    if(fptr == NULL){
        printf("Error!");   
        exit(1);             
    }

    int i, j;
    for(i = 0; i < net->nb_row * net->nb_column; i++){
        for(j = 0; j < net->vec_size; j++){
            fprintf(fptr,"%lf,",net->map[i].w[j]);
        }
    }

    fclose(fptr);
}


/*
void normalize_vector(vec_t * vecs, int nb_vec){
    int i;
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
*/

/*
void shuffle_vectors(int ** vec, int nb_vec){
    int i;
    int random, tmp;
    int * v = (int *)malloc((size_t)nb_vec * (int)sizeof(int));
    if(v == NULL) allocation_failure_handle();
    for(i = 0; i < nb_vec; i++)
        v[i] = i;
    for(i = 0; i < nb_vec; i++){
        random = rand() % nb_vec;
        tmp = v[i];
        v[i] = v[random];
        v[random] = tmp;
    }
    *vec = v;
}
*/

/*
void init_network(net_t * config){
    config->map = (node_t *)malloc((size_t)config->nb_row * (size_t)config->nb_column * (int)sizeof(node_t));
    if(config->map == NULL) allocation_failure_handle();
    int i, j;
    for(i = 0; i < config->nb_row * config->nb_column; i++){
        //printf("%lf %d \n", (double)rand() / (double)RAND_MAX, i);
        config->map[i].w = (double *)malloc((size_t)config->vec_size * (int)sizeof(double));
        if(config->map[i].w == NULL) allocation_failure_handle();
        for(j = 0; j < config->vec_size; j++)
            config->map[i].w[j] = (double)rand() / (double)RAND_MAX;
    }
    //printf("%lf\n", weight[0].w[0]);
}
*/