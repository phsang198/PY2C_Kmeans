#ifndef WRITE
#define WRITE
#include "resource.h"

void print_vector(point_t v, FILE* output_file)
{
    int dimension = v.dimen_size;

    for (int i = 0; i < dimension; i++)
    {
        if (i == 0)
            fprintf(output_file, "(");
        if (i >= 0 && i < dimension - 1)
            fprintf(output_file, "%I64d, ", v.dimen[i]);
        if (i == dimension - 1)
            fprintf(output_file, "%I64d)", v.dimen[i]);
    }
}
#endif // WRITE