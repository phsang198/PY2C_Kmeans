#ifndef RESOURCE
#define RESOURCE
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
//
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
//_CrtDumpMemoryLeaks();

typedef struct {
    int64_t* dimen; // Mảng dữ liệu kiểu int64_t
    int64_t dimen_size;
} point_t;


point_t point_init( int64_t dimen_size)
{
    point_t tmp;
    tmp.dimen_size = dimen_size;
    tmp.dimen = (int64_t*)malloc(sizeof(int64_t) * dimen_size);
    return tmp;

}
void point_destroy(point_t* point)
{
    if (point != NULL)
    {
        if (point->dimen != NULL)
            free(point->dimen);
        point->dimen_size = 0;
        point = NULL;
    }
}

void copy_point(point_t from, point_t* to)
{
    *to = point_init(from.dimen_size);
    to->dimen_size = from.dimen_size;
    for (int64_t j = 0; j < from.dimen_size; j++)
        to->dimen[j] = from.dimen[j];
}
point_t* vector_init( int64_t vector_size)
{
    point_t* vector = (point_t*)malloc(sizeof(point_t) * vector_size);

    return vector;
}
void vector_destroy(point_t** vector, int64_t vector_size)
{
    if (*vector != NULL)
    {
        for (int i = 0; i < vector_size; i++)
            point_destroy(&(*vector)[i]);

        free(*vector);
        *vector = NULL;
    }
}
//-----------------------------------------------------------------------

typedef struct {
    point_t* vector; // Mảng dữ liệu kiểu int64_t
    point_t centroid;
    int64_t vector_size;
} cluster_t;

cluster_t* cluster_init( int64_t cluster_size)
{
    cluster_t* cluster = (cluster_t*)malloc(sizeof(cluster_t) * cluster_size);

    for (int i = 0; i < cluster_size; i++)
    {
        cluster[i].vector = NULL;
        cluster[i].vector_size = 0;
    }

    return cluster; 
}
void cluster_destroy(cluster_t* cluster)
{
    vector_destroy(&cluster->vector, cluster->vector_size); 

    point_destroy(&cluster->centroid); 

    if (cluster->vector != NULL)
        free(cluster->vector); 
    cluster->vector_size = 0;
    cluster = NULL;
}
//-----------------------------------------------------------------------
typedef struct {
    point_t* vector;        // Mảng các vector
    int64_t N;           // Số lượng vector
    int k;                // Số lượng clusters
    int distance;         // Loại khoảng cách (0: Manhattan, 1: Euclidean)
    int64_t dimension;
    bool quiet;
} kmeans_t;

// Định nghĩa hằng số LLONG_MAX với giá trị lớn nhất của kiểu int64_t
#define LLONG_MAX 9223372036854775807LL
#define MAX_VECTORS 1000


#endif // RESOURCE