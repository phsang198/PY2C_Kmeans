#ifndef MTHREAD
#define MTHREAD
#include "resource.h"
#include "kmeans2.h"

#define NUM_THREADS 4

typedef struct {
    point_t* init_centroid; 
    kmeans_t* kmeans;
    FILE* output_file;
    pthread_mutex_t* mutex;
} ThreadData;

void* threadFunction(void* arg) 
{
    ThreadData* data = (ThreadData*)arg;

   /* for (int i = 0; i < data->kmeans->k; i++)
    {
        printf("(%d %d ), ", data->init_centroid[i].dimen[0], data->init_centroid[i].dimen[1]);
    }
    printf("\n");*/

    multi_kmeans_run(data->mutex, data->init_centroid, *data->kmeans, data->output_file);

    pthread_mutex_lock(data->mutex);
    vector_destroy(&data->init_centroid, data->kmeans->k); 
    pthread_mutex_unlock(data->mutex);

    return NULL;
}
#endif // MTHREAD