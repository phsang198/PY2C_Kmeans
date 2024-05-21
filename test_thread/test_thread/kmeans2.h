#ifndef KMEAN2
#define KMEAN2
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#include "resource.h"
#include "write.h"
#include "kmeans.h"

void multi_kmeans_run(pthread_mutex_t* mutex, point_t* init_centroid, kmeans_t kmeans, FILE* output_file)
{
    cluster_t* cluster = cluster_init(kmeans.k);

    point_t* tmp_init_centroid = vector_init(kmeans.k); 
    // Chọn ngẫu nhiên các trọng tâm ban đầu
    pthread_mutex_lock(mutex);

    for (int i = 0; i < kmeans.k; i++)
    {
        copy_point(init_centroid[i], &cluster[i].centroid);
        copy_point(init_centroid[i], &tmp_init_centroid[i]);
    }
    pthread_mutex_unlock(mutex);

    if (cluster[0].vector == NULL)
    {
        cluster[0].vector_size = kmeans.N;
        cluster[0].vector = vector_init(kmeans.N);
    }
    for (int i = 0; i < kmeans.N; i++)
    {
        copy_point(kmeans.vector[i], &cluster[0].vector[i]);
    }


    bool changed = true;
    int64_t distortion = LLONG_MAX;
    // Chạy vòng lặp cho đến khi không có sự thay đổi nào
    while (changed)
    {
        changed = assign_vectors_to_centroids(kmeans, cluster);
        update_centroids(cluster, kmeans.k);
        int64_t tmp_distortion = kmeans_distortion(cluster, kmeans.k, kmeans.distance);
        if (tmp_distortion < distortion)
            distortion = tmp_distortion;
    }
    // In ra distortion


    pthread_mutex_lock(mutex);

    fprintf(output_file, "\"[");
    for (int i = 0; i < kmeans.k; i++)
    {
        print_vector(tmp_init_centroid[i], output_file);
        if (i < (kmeans.k - 1))
            fprintf(output_file, ", ");
    }
    fprintf(output_file, "]\",");

    fprintf(output_file, "%I64d,", distortion);
    // In ra runned centroid vector
    fprintf(output_file, "\"[");
    for (int i = 0; i < kmeans.k; i++)
    {
        print_vector(cluster[i].centroid, output_file);
        if (i < (kmeans.k - 1))
            fprintf(output_file, ", ");
    }
    fprintf(output_file, "]\",");

    // In ra runned cluster vector
    if (!kmeans.quiet)
    {

        fprintf(output_file, "\"[");
        for (int i = 0; i < kmeans.k; i++)
        {
            fprintf(output_file, "[");
            for (int j = 0; j < cluster[i].vector_size; j++)
            {
                print_vector(cluster[i].vector[j], output_file);
                if (j < (cluster[i].vector_size - 1))
                    fprintf(output_file, ", ");
            }
            fprintf(output_file, "]");
            if (i < kmeans.k - 1)
                fprintf(output_file, ", ");
        }
        fprintf(output_file, "]\"\n\n");
    }

    pthread_mutex_unlock(mutex);
    vector_destroy(&tmp_init_centroid, kmeans.k); 
    cluster_destroy(cluster);
}
#endif // KMEAN2