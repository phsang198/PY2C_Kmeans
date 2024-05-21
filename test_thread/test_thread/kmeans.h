#ifndef KMEAN
#define KMEAN
#include "resource.h"
#include "write.h"

// Cập nhật các trọng tâm của các cluster
void update_centroids(cluster_t* cluster, int cluster_size)
{
    int dimension = cluster[0].centroid.dimen_size;

    int64_t sum[1000];
    for (int j = 0; j < dimension; j++) sum[j] = 0;

    for (int i = 0; i < cluster_size; i++)
        if (cluster[i].vector_size != 0)
        {
            for (int d = 0; d < dimension; d++)
            {
                for (int j = 0; j < cluster[i].vector_size; j++)
                {
                    sum[d] += cluster[i].vector[j].dimen[d];
                }
            }

            // Cập nhật trung bình vào trọng tâm của cluster
            for (int d = 0; d < dimension; d++)
                if (cluster[i].vector_size != 0)
                {
                    cluster[i].centroid.dimen[d] = sum[d] / cluster[i].vector_size;
                    sum[d] = 0;
                }
        }
}
// Gán các vector vào cluster tương ứng với trọng tâm gần nhất
bool assign_vectors_to_centroids(kmeans_t kmeans, cluster_t* cluster)
{
    bool changed = true;
    cluster_t* tmp_cluster = cluster_init(kmeans.k);
    for (int i = 0; i < kmeans.k; i++)
        {
            if (tmp_cluster[i].vector == NULL)
            {
                tmp_cluster[i].vector_size = 0;
                tmp_cluster[i].vector = vector_init(kmeans.N);
            }
            copy_point(cluster[i].centroid, &tmp_cluster[i].centroid);

        }
    /*for (int i = 0; i < kmeans.N; i++)
    {
        printf("%d %d \n", cluster[0].vector[i].dimen[0],
            cluster[0].vector[i].dimen[1]);
    }*/
    // Duyệt qua từng vector và gán vào cluster gần nhất
    for (int i = 0; i < kmeans.k; i++)
    {
        for ( int k = 0 ; k < cluster[i].vector_size ; k++)
        {
            int64_t min_dist = LLONG_MAX;

            int nearest_cluter_id = -1;
            // Tính khoảng cách từ vector đến tất cả trọng tâm và chọn trọng tâm gần nhất
            for (int j = 0; j < kmeans.k; j++)
            {
                /*if (k == 16 && j == 8)
                {
                    int z = 1;
                }*/
                int64_t dist = dist_squared(kmeans.distance, cluster[i].vector[k], cluster[j].centroid);
                if (dist < min_dist)
                {
                    min_dist = dist;
                    nearest_cluter_id = j;
                }

                
                    /*printf("coord : %d %d , centrod : %d %d , xet : %d %d \n", cluster[i].vector[k].dimen[0],
                        cluster[i].vector[k].dimen[1],
                        cluster[nearest_cluter_id].centroid.dimen[0], cluster[nearest_cluter_id].centroid.dimen[1],
                        cluster[j].centroid.dimen[0], cluster[j].centroid.dimen[1]);*/
            }
            copy_point(cluster[i].vector[k], &tmp_cluster[nearest_cluter_id].vector[tmp_cluster[nearest_cluter_id].vector_size]);
            tmp_cluster[nearest_cluter_id].vector_size++;
            changed = changed && (nearest_cluter_id == i);
            
            /*printf("%d %d : nearest %d %d than %d %d \n", cluster[i].vector[k].dimen[0],
                cluster[i].vector[k].dimen[1],
                cluster[nearest_cluter_id].centroid.dimen[0], cluster[nearest_cluter_id].centroid.dimen[1],
                cluster[i].centroid.dimen[0], cluster[i].centroid.dimen[1]);*/
        }
    }

    if (changed == 1) changed = 0;
    else changed = 1;

    for (int i = 0; i < kmeans.k; i++)
    {
        if (cluster[i].vector != NULL)
        {
            vector_destroy(&cluster[i].vector, cluster[i].vector_size);
        }
        if (cluster[i].vector == NULL)
        {
            cluster[i].vector_size = tmp_cluster[i].vector_size;
            cluster[i].vector = vector_init(tmp_cluster[i].vector_size);
        }
        for (int j = 0; j < cluster[i].vector_size; j++)
        {
            copy_point(tmp_cluster[i].vector[j], &cluster[i].vector[j]);
        }
    }

    cluster_destroy(tmp_cluster);

    return changed;
}
// Tính distortion của thuật toán K-Means
int64_t kmeans_distortion(cluster_t* cluster, int cluter_size, int type_distance)
{
    int64_t sum = 0;
    for (int i = 0; i < cluter_size; i++)
        if (cluster[i].vector_size != 0)
        {
            for (int j = 0; j < cluster[i].vector_size; j++)
            {
                int64_t dist = dist_squared(type_distance, cluster[i].centroid, cluster[i].vector[j]);
                sum += dist;
            }
        }
    return sum;
}

void kmeans_run(point_t* init_centroid, kmeans_t kmeans, FILE* output_file)
{
    cluster_t* cluster = cluster_init(kmeans.k);

    // Chọn ngẫu nhiên các trọng tâm ban đầu
    for (int i = 0; i < kmeans.k; i++)
    {
        copy_point(init_centroid[i], &cluster[i].centroid);
    }
    if (cluster[0].vector == NULL)
    {
        cluster[0].vector_size = kmeans.N;
        cluster[0].vector = vector_init(kmeans.N);
    }
    for (int i = 0; i < kmeans.N; i++)
    {
        copy_point(kmeans.vector[i], &cluster[0].vector[i]);
    }

    // In ra init centroid vector

    fprintf(output_file, "\"[");
    for (int i = 0; i < kmeans.k; i++)
    {
        print_vector(cluster[i].centroid, output_file);
        if (i < (kmeans.k - 1))
            fprintf(output_file, ", ");
    }
    fprintf(output_file, "]\",");


    bool changed = true;
    int64_t distortion = LLONG_MAX;
    // Chạy vòng lặp cho đến khi không có sự thay đổi nào
    while (changed)
    {
        //printf("assigning... \n");

        changed = assign_vectors_to_centroids(kmeans, cluster);

        //printf("assign_vectors_to_centroids \n");

        update_centroids(cluster, kmeans.k);
        //printf("update_centroids \n");

        int64_t tmp_distortion = kmeans_distortion(cluster, kmeans.k, kmeans.distance);
        if (tmp_distortion < distortion)
            distortion = tmp_distortion;
        //printf("kmeans_distortion \n");

    }

    // In ra distortion
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
    cluster_destroy(cluster); 

}
#endif // KMEAN