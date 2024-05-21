#ifndef COMBI
#define COMBI
#include <stdio.h>
#include <stdlib.h>

void combinationUtil(int arr[], int data[], int start, int end, int index, int r, int** combinations, int* combinationIndex) {
    if (index == r) {
        for (int j = 0; j < r; j++) {
            combinations[*combinationIndex][j] = data[j];
        }
        (*combinationIndex)++;
        return;
    }

    for (int i = start; i <= end && end - i + 1 >= r - index; i++) {
        data[index] = arr[i];
        combinationUtil(arr, data, i + 1, end, index + 1, r, combinations, combinationIndex);
    }
}

int** generateCombinations(int arr[], int n, int k, int* numCombinations) {
    if (k > n) return NULL;

    // Tính số lượng tổ hợp
    *numCombinations = 1;
    for (int i = 1; i <= k; i++) {
        *numCombinations *= (n - i + 1);
        *numCombinations /= i;
    }

    // Tạo mảng 2 chiều để lưu các tổ hợp
    int** combinations = (int**)malloc(*numCombinations * sizeof(int*));
    for (int i = 0; i < *numCombinations; i++) {
        combinations[i] = (int*)malloc(k * sizeof(int));
    }

    // Tạo mảng tạm để lưu các phần tử trong quá trình sinh tổ hợp
    int* data = (int*)malloc(k * sizeof(int));

    int combinationIndex = 0;
    combinationUtil(arr, data, 0, n - 1, 0, k, combinations, &combinationIndex);

    free(data); // Giải phóng bộ nhớ của mảng tạm

    return combinations;
}
void Combi(int n, int k , int* **result, int* numCombinations)
{
    // Tạo mảng các số tự nhiên từ 0 đến n-1
    int* arr = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        arr[i] = i;
    }

    // Tạo các tổ hợp và lưu vào mảng 2 chiều
    *result = generateCombinations(arr, n, k, numCombinations);

}
#endif // COMBI