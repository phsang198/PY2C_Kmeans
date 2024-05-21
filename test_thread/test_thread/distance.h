#ifndef DISTANCE
#define DISTANCE
#include "resource.h"
// Tính bình phương khoảng cách Euclidean giữa hai vector
int64_t euclidean_distance_squared(point_t a, point_t b) {
    int64_t sum = 0;
    for (int64_t i = 0; i < a.dimen_size; i++)
    {
        int64_t diff = a.dimen[i] - b.dimen[i];
        sum += diff * diff;
    }
    return sum;
}

int64_t _ABC(int64_t value)
{
    if (value < 0) return -value;
    return value; 
}
// Tính bình phương khoảng cách Manhattan giữa hai vector
int64_t manhattan_distance_squared(point_t a, point_t b)
{
    /*int32_t test32 = a.dimen[0];
    int64_t test64 = a.dimen[0];*/ 

   /* printf("a : %d %d , b : %d %d \n", a.dimen[0],
        a.dimen[1],
        b.dimen[0], b.dimen[1]);*/

    int64_t sum = 0;
    for (int64_t i = 0; i < a.dimen_size; i++)
    {
        int64_t diff = _ABC(a.dimen[i] - b.dimen[i]);
        sum += diff;
    }
    return sum * sum;
}

// Tính bình phương khoảng cách dựa trên loại khoảng cách được chọn (Manhattan hoặc Euclidean)
int64_t dist_squared(int type_distance, point_t a, point_t b)
{
    if (type_distance == 0) {
        return manhattan_distance_squared(a, b); // Sử dụng khoảng cách Manhattan
    }
    else {
        return euclidean_distance_squared(a, b); // Sử dụng khoảng cách Euclidean
    }
}

#endif // DISTANCE