#include <stdbool.h>
#include <string.h>

#include "resource.h"
#include "distance.h"
#include "read.h"
#include "combination.h"

#include "mthread.h"
#include <windows.h>

int main(int argc, char* argv[])
{
    FILE* input_file = stdin;
    FILE* output_file = stdout;
    int distance = 0;
    int k = 10;
    int picking_limit = 10;
    bool quiet = false;

    // Đọc các tham số dòng lệnh
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0) {
            output_file = fopen(argv[++i], "w");
            if (!output_file) {
                perror("Error opening output file");
                return 1;
            }

            input_file = fopen(argv[++i], "rb");
            if (!input_file) {
                perror("Error opening input file");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-d") == 0) {
            distance = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-k") == 0) {
            k = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-p") == 0) {
            picking_limit = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-q") == 0) {
            quiet = true;
        }
        else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            return 1;
        }
    }

    // Đọc dữ liệu từ tệp input

    fseek(input_file, 0, SEEK_END);
    long fileSize = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);
    // Khai báo một mảng binary_data có 12 phần tử kiểu uint8_t để lưu dữ liệu nhị phân từ file đầu vào
    unsigned char* binary_data = (unsigned char*)malloc(fileSize);

    // Đọc 12 byte dữ liệu từ file đầu vào và lưu vào mảng binary_data
    fread(binary_data, 1, fileSize, input_file);
    // Khai báo một biến kmeans có kiểu kmeans_t để lưu trữ thông tin về thuật toán K-Means
    kmeans_t kmeans;
    // Gán giá trị đầu tiên của mảng binary_data vào thuộc tính dimension của biến kmeans
    int dimension = bin_to_int_digit(binary_data, 4, 0);
    // Gán giá trị các byte từ 4 đến 7 của mảng binary_data vào thuộc tính N của biến kmeans
    kmeans.N = bin_to_int_digit(binary_data, 12, 4);

    kmeans.k = k;
    kmeans.dimension = dimension;
    kmeans.quiet = quiet;

    kmeans.distance = distance;
    if (picking_limit > kmeans.N) picking_limit = kmeans.N;
    if (picking_limit < k)
        return 0;
    kmeans.vector = vector_init(kmeans.N);

    int _byte = 20;
    for (int i = 0; i < kmeans.N; i++)
    {
        kmeans.vector[i] = point_init(dimension);
        for (int j = 0; j < dimension; j++)
        {
            long long tmp = bin_to_int_digit(binary_data, _byte, _byte - 8);
            _byte += 8;
            kmeans.vector[i].dimen[j] = tmp;
            //printf("%d ",tmp);
        }
        //printf("\n");
    }

    fprintf(output_file, "initialization centroids,distortion,centroids");
    if (!quiet) {
        fprintf(output_file, ",clusters");
    }

    fprintf(output_file, "\n\n");

    int numCombinations = 0;
    int** combinations = NULL;
    //Combi(kmeans.N, kmeans.k, &combinations, &numCombinations);
    Combi(picking_limit, kmeans.k, &combinations, &numCombinations);

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);


    pthread_t threads[NUM_THREADS];

    int count = 0;

    for (int i = 0; i < numCombinations; i++)
    {
        ThreadData threadData;
        threadData.kmeans = &kmeans;
        threadData.output_file = output_file;
        threadData.mutex = &mutex;

        point_t* init_centroid = vector_init(kmeans.k);

        for (int j = 0; j < kmeans.k; j++)
        {
            int index = combinations[i][j];
            copy_point(kmeans.vector[index], &init_centroid[j]);
        }
        Sleep(1);
        /*for (int i = 0; i < kmeans.k; i++)
        {
            printf("(%d %d ), ", init_centroid[i].dimen[0], init_centroid[i].dimen[1]);
        }
        printf("\n");*/

        pthread_mutex_lock(&mutex);
        threadData.init_centroid = init_centroid;
        pthread_mutex_unlock(&mutex);

        if (pthread_create(&threads[count], NULL, threadFunction, (void*)&threadData) != 0)
        {
            return 1;
        }
        else count++;

        if (count == NUM_THREADS || i == numCombinations - 1)
        {
            for (int j = 0; j < count; j++) {
                pthread_join(threads[j], NULL);
            }
            count = 0;
        }
    }

    // Giải phóng bộ nhớ đã cấp phát cho result
    for (int i = 0; i < numCombinations; i++) {
        free(combinations[i]);
    }
    free(combinations);


    //fprintf(output_file, res);

    fclose(input_file);
    fclose(output_file);

    vector_destroy(&kmeans.vector, kmeans.N);


    pthread_mutex_destroy(&mutex);

    return 1;
}


//int main()
//{
//    // Mở tệp input và output
//    FILE* input_file = stdin;
//    FILE* output_file = stdout;
//    int distance = 0;
//    int k = 5;
//    int picking_limit = 10;
//    bool quiet = false;
//    input_file = fopen("F:\\OutSource\\PYTHON\\PY2C\\test_thread\\x64\\Debug\\example.bin", "rb");
//    output_file = fopen("F:\\OutSource\\PYTHON\\PY2C\\test_thread\\x64\\Debug\\tesdt.csv", "w");
//
//    // Đọc dữ liệu từ tệp input
//
//    fseek(input_file, 0, SEEK_END);
//    long fileSize = ftell(input_file);
//    fseek(input_file, 0, SEEK_SET);
//    // Khai báo một mảng binary_data có 12 phần tử kiểu uint8_t để lưu dữ liệu nhị phân từ file đầu vào
//    unsigned char* binary_data = (unsigned char*)malloc(fileSize);
//
//    // Đọc 12 byte dữ liệu từ file đầu vào và lưu vào mảng binary_data
//    size_t bytesRead = fread(binary_data, 1, fileSize, input_file);
//    // Khai báo một biến kmeans có kiểu kmeans_t để lưu trữ thông tin về thuật toán K-Means
//    kmeans_t kmeans;
//    // Gán giá trị đầu tiên của mảng binary_data vào thuộc tính dimension của biến kmeans
//    int dimension = bin_to_int_digit(binary_data, 4, 0);
//    // Gán giá trị các byte từ 4 đến 7 của mảng binary_data vào thuộc tính N của biến kmeans
//    kmeans.N = bin_to_int_digit(binary_data, 12, 4);
//
//    kmeans.k = k;
//    kmeans.dimension = dimension;
//    kmeans.quiet = quiet;
//
//    kmeans.distance = distance;
//    if (picking_limit > kmeans.N) picking_limit = kmeans.N;
//    if (picking_limit < k)
//        return 0;
//    kmeans.vector = vector_init(kmeans.N);
//
//    int _byte = 20;
//    for (int i = 0; i < kmeans.N; i++)
//    {
//        kmeans.vector[i] = point_init(dimension);
//        for (int j = 0; j < dimension; j++)
//        {
//            long long tmp = bin_to_int_digit(binary_data, _byte, _byte - 8);
//            _byte += 8;
//            kmeans.vector[i].dimen[j] = tmp;
//            //printf("%d ",tmp);
//        }
//        //printf("\n");
//    }
//
//    fprintf(output_file, "initialization centroids,distortion,centroids");
//    if (!quiet) {
//        fprintf(output_file, ",clusters");
//    }
//
//    fprintf(output_file, "\n\n");
//
//    int numCombinations = 0;
//    int** combinations = NULL;
//    //Combi(kmeans.N, kmeans.k, &combinations, &numCombinations);
//    Combi(picking_limit, kmeans.k, &combinations, &numCombinations);
//
//    pthread_mutex_t mutex;
//    pthread_mutex_init(&mutex, NULL);
//
//    
//    pthread_t threads[NUM_THREADS];
//
//    int count = 0; 
//
//    for (int i = 0; i < numCombinations; i++)
//    {
//        ThreadData threadData;
//        threadData.kmeans = &kmeans;
//        threadData.output_file = output_file;
//        threadData.mutex = &mutex;
//
//        point_t* init_centroid = vector_init(kmeans.k);
//
//        for (int j = 0; j < kmeans.k; j++)
//        {
//            int index = combinations[i][j];
//            copy_point(kmeans.vector[index], &init_centroid[j]);
//        }
//        for (int i = 0; i < kmeans.k; i++)
//        {
//            printf("(%d %d ), ", init_centroid[i].dimen[0],  init_centroid[i].dimen[1]);
//        }
//        printf("\n");
//
//        pthread_mutex_lock(&mutex);
//        threadData.init_centroid = init_centroid;
//        pthread_mutex_unlock(&mutex);
//
//        if (pthread_create(&threads[count], NULL, threadFunction, (void*)&threadData) != 0) 
//        {
//            return 1;
//        }
//        else count++; 
//
//        if ( count == NUM_THREADS || i == numCombinations -1 )
//        {
//            for (int j = 0; j < count; j++) {
//                pthread_join(threads[j], NULL);
//            }
//            count = 0;
//        }
//    }
//
//    // Giải phóng bộ nhớ đã cấp phát cho result
//    for (int i = 0; i < numCombinations; i++) {
//        free(combinations[i]);
//    }
//    free(combinations);
//
//
//    //fprintf(output_file, res);
//
//    fclose(input_file);
//    fclose(output_file);
//
//    vector_destroy(&kmeans.vector, kmeans.N);
//
//
//    pthread_mutex_destroy(&mutex);
//
//    return 1;
//}

