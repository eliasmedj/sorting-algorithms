#include <stdio.h>
#include <stdlib.h>

#include "../src/sorting.h"
#include "../src/utils.h"
#include "../src/benchmark.h"

typedef struct
{
    const char *name;
    SortFunction function;
} TestAlgorithm;

void mergeWrapper(int arr[], int n)
{
    mergeSort(arr, 0, n - 1);
}

void quickWrapper(int arr[], int n)
{
    quickSort(arr, 0, n - 1);
}

static int runTestCase(const TestAlgorithm *algorithm,
                       const int *input,
                       const int *expected,
                       int n,
                       const char *case_name)
{
    int *copy = copyArray(input, n);
    if (copy == NULL)
    {
        printf("[ERROR] Memory allocation failed.\n");
        return 0;
    }

    algorithm->function(copy, n);
    int passed = arraysEqual(copy, expected, n);

    printf("[%s] %-15s | %s\n",
           passed ? "PASS" : "FAIL",
           algorithm->name,
           case_name);

    free(copy);
    return passed;
}

int main(void)
{
    const int random_input[] = {9, 1, 8, 2, 7, 3, 6, 4, 5};
    const int random_expected[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    const int sorted_input[] = {1, 2, 3, 4, 5};
    const int sorted_expected[] = {1, 2, 3, 4, 5};

    const int reverse_input[] = {5, 4, 3, 2, 1};
    const int reverse_expected[] = {1, 2, 3, 4, 5};

    const int duplicate_input[] = {4, 2, 4, 1, 2, 1, 4};
    const int duplicate_expected[] = {1, 1, 2, 2, 4, 4, 4};

    TestAlgorithm algorithms[] =
    {
        {"Bubble Sort", BubbleSort},
        {"Selection Sort", SelectionSort},
        {"Insertion Sort", InsertionSort},
        {"Merge Sort", mergeWrapper},
        {"Quick Sort", quickWrapper}
    };

    int passed = 0;
    int total = 0;
    const int algorithm_count = (int)(sizeof(algorithms) / sizeof(algorithms[0]));

    for (int i = 0; i < algorithm_count; i++)
    {
        passed += runTestCase(&algorithms[i], random_input, random_expected,
                              9, "Random data");
        total++;
        passed += runTestCase(&algorithms[i], sorted_input, sorted_expected,
                              5, "Already sorted");
        total++;
        passed += runTestCase(&algorithms[i], reverse_input, reverse_expected,
                              5, "Reverse sorted");
        total++;
        passed += runTestCase(&algorithms[i], duplicate_input, duplicate_expected,
                              7, "Duplicate values");
        total++;
    }

    printf("\nResult: %d/%d tests passed.\n", passed, total);
    return passed == total ? 0 : 1;
}
