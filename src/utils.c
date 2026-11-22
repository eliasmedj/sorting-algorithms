
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

void printArray(int arr[], int n)
{
    for (int i = 0; i < n; i++)
        printf("%d ", arr[i]);

    printf("\n");
}

void fillRandomArray(int arr[], int n, unsigned int seed)
{
    srand(seed);

    for (int i = 0; i < n; i++)
        arr[i] = rand() % 100001;
}

int* copyArray(const int arr[], int n)
{
    int *copy = malloc((size_t)n * sizeof(int));

    if (copy == NULL)
        return NULL;

    for (int i = 0; i < n; i++)
        copy[i] = arr[i];

    return copy;
}

int isSorted(const int arr[], int n)
{
    for (int i = 1; i < n; i++)
    {
        if (arr[i - 1] > arr[i])
            return 0;
    }

    return 1;
}

int arraysEqual(const int a[], const int b[], int n)
{
    for (int i = 0; i < n; i++)
    {
        if (a[i] != b[i])
            return 0;
    }

    return 1;
}
