#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sorting.h"
#include "utils.h"
#include "benchmark.h"

#define REPETITIONS 5
#define LARGE_ARRAY_SIZE 100000
#define CSV_FILE "results/results.csv"
#define COMPARISON_SIZE_COUNT 3

static const int comparison_sizes[COMPARISON_SIZE_COUNT] = {1000, 5000, 10000};

void mergeSortWrapper(int arr[], int n)
{
    mergeSort(arr, 0, n - 1);
}

void quickSortWrapper(int arr[], int n)
{
    quickSort(arr, 0, n - 1);
}

typedef struct
{
    const char *name;
    SortFunction function;
} Algorithm;

static Algorithm algorithms[] =
{
    {"Bubble Sort", BubbleSort},
    {"Selection Sort", SelectionSort},
    {"Insertion Sort", InsertionSort},
    {"Merge Sort", mergeSortWrapper},
    {"Quick Sort", quickSortWrapper}
};

static void initializeCSV(void)
{
    const char *header =
        "Input_Type,Algorithm,Array_Size,Repetitions,Average_Time_ms,"
        "Peak_Additional_Private_Memory_KB,Status";

    FILE *file = fopen(CSV_FILE, "r");
    if (file != NULL)
    {
        char first_line[512];
        if (fgets(first_line, sizeof(first_line), file) != NULL)
        {
            first_line[strcspn(first_line, "\r\n")] = '\0';
            fclose(file);
            if (strcmp(first_line, header) == 0)
                return;
        }
        else
        {
            fclose(file);
        }
    }

    file = fopen(CSV_FILE, "w");
    if (file == NULL)
    {
        printf("Cannot create CSV file. Check the results folder.\n");
        return;
    }

    fprintf(file, "%s\n", header);
    fclose(file);
}

static void saveAndPrintResult(
    const char *input_type,
    int algorithm_index,
    int n,
    const int original[])
{
    BenchmarkResult result = benchmarkSort(
        algorithms[algorithm_index].function,
        original,
        n,
        REPETITIONS);

    if (!result.valid)
    {
        printf("%-18s %10s %14s %s\n",
               algorithms[algorithm_index].name, "-", "-", "FAIL");
        exportResultCSV(CSV_FILE, input_type, algorithms[algorithm_index].name,
                        n, REPETITIONS, result);
        return;
    }

    printf("%-18s %10.4f %14.2f %s\n",
           algorithms[algorithm_index].name,
           result.time_ms,
           result.memory_kb,
           "PASS");

    if (!exportResultCSV(CSV_FILE, input_type,
                         algorithms[algorithm_index].name,
                         n, REPETITIONS, result))
        printf("Could not write benchmark result to CSV.\n");
}

static void runSingleAlgorithm(int choice, int n)
{
    int *arr = malloc((size_t)n * sizeof(int));
    if (arr == NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }

    fillRandomArray(arr, n, 42);

    printf("\nOriginal array:\n");
    if (n <= 50)
        printArray(arr, n);
    else
        printf("Array contains %d elements.\n", n);

    BenchmarkResult result = benchmarkSort(
        algorithms[choice - 1].function, arr, n, REPETITIONS);

    if (!result.valid)
    {
        printf("Sorting benchmark failed.\n");
        free(arr);
        return;
    }

    algorithms[choice - 1].function(arr, n);

    printf("\nSorted array:\n");
    if (n <= 50)
        printArray(arr, n);
    else
        printf("Array sorted successfully: %s\n", isSorted(arr, n) ? "YES" : "NO");

    printf("\nAverage Execution Time: %.6f ms\n", result.time_ms);
    printf("Peak Additional Private Memory: %.2f KB\n", result.memory_kb);

    if (!exportResultCSV(CSV_FILE, "Random", algorithms[choice - 1].name,
                         n, REPETITIONS, result))
        printf("Could not write benchmark result to CSV.\n");

    free(arr);
}

static void compareAllAlgorithms(void)
{
    printf("\n========== PERFORMANCE COMPARISON =========\n");
    printf("Each algorithm sorts its own copy of the same random input.\n");
    printf("Repetitions per algorithm: %d\n", REPETITIONS);

    for (int size_index = 0; size_index < COMPARISON_SIZE_COUNT; size_index++)
    {
        int n = comparison_sizes[size_index];
        int *original = malloc((size_t)n * sizeof(int));

        if (original == NULL)
        {
            printf("Could not allocate array of size %d.\n", n);
            continue;
        }

        fillRandomArray(original, n, 42);

        printf("\nArray Size: %d\n", n);
        printf("%-18s %10s %14s %s\n",
               "Algorithm", "Time (ms)", "Memory (KB)", "Status");
        printf("---------------------------------------------------------------\n");

        for (int algorithm_index = 0; algorithm_index < 5; algorithm_index++)
            saveAndPrintResult("Random", algorithm_index, n, original);

        free(original);
    }

    printf("\nComparison results saved in %s\n", CSV_FILE);
}

static void runLargeArrayTest(void)
{
    const int n = LARGE_ARRAY_SIZE;
    int *original = malloc((size_t)n * sizeof(int));

    if (original == NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }

    fillRandomArray(original, n, 42);

    printf("\n========== BIG ARRAY TEST =========\n");
    printf("Array Size: %d\n", n);
    printf("Repetitions: %d\n", REPETITIONS);
    printf("Input Type: Random, fixed seed 42\n\n");
    printf("%-18s %10s %14s %s\n",
           "Algorithm", "Time (ms)", "Memory (KB)", "Status");
    printf("---------------------------------------------------------------\n");

    for (int algorithm_index = 3; algorithm_index < 5; algorithm_index++)
        saveAndPrintResult("Random", algorithm_index, n, original);

    free(original);
}

static void clearInputLine(void)
{
    int character;
    while ((character = getchar()) != '\n' && character != EOF)
        ;
}

int main(void)
{
    int choice;
    int n;

    initializeCSV();

    while (1)
    {
        printf("\n========== SORTING ALGORITHMS ==========\n");
        printf("1. Bubble Sort\n");
        printf("2. Selection Sort\n");
        printf("3. Insertion Sort\n");
        printf("4. Merge Sort\n");
        printf("5. Quick Sort\n");
        printf("6. Compare All Algorithms (1000, 5000, 10000)\n");
        printf("7. Big Array Test (100000 elements)\n");
        printf("0. Exit\n");
        printf("\nYour choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input. Enter a number.\n");
            clearInputLine();
            continue;
        }

        if (choice == 0)
        {
            printf("Goodbye!\n");
            break;
        }

        if (choice == 6)
        {
            compareAllAlgorithms();
            continue;
        }

        if (choice == 7)
        {
            runLargeArrayTest();
            continue;
        }

        if (choice < 1 || choice > 5)
        {
            printf("Invalid choice.\n");
            clearInputLine();
            continue;
        }

        printf("Enter array size: ");
        if (scanf("%d", &n) != 1 || n <= 0)
        {
            printf("Invalid array size. Enter a positive integer.\n");
            clearInputLine();
            continue;
        }

        runSingleAlgorithm(choice, n);
    }

    return 0;
}
