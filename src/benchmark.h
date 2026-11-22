#ifndef BENCHMARK_H
#define BENCHMARK_H

typedef void (*SortFunction)(int[], int);

typedef struct
{
    double time_ms;
    double memory_kb;
    int valid;
} BenchmarkResult;

BenchmarkResult benchmarkSort(
    SortFunction sort,
    const int original[],
    int n,
    int repetitions
);

int exportResultCSV(
    const char *filename,
    const char *input_type,
    const char *algorithm,
    int n,
    int repetitions,
    BenchmarkResult result
);

#endif
