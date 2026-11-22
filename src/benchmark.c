#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <psapi.h>

#include "benchmark.h"
#include "utils.h"

typedef struct
{
    volatile LONG stop;
    SIZE_T baseline_bytes;
    double peak_additional_kb;
} MemoryMonitor;

static SIZE_T getPrivateUsageBytes(void)
{
    PROCESS_MEMORY_COUNTERS_EX memory;
    memory.cb = sizeof(memory);

    if (!GetProcessMemoryInfo(
            GetCurrentProcess(),
            (PROCESS_MEMORY_COUNTERS *)&memory,
            sizeof(memory)))
        return 0;

    return (SIZE_T)memory.PrivateUsage;
}

static DWORD WINAPI monitorMemory(LPVOID parameter)
{
    MemoryMonitor *monitor = (MemoryMonitor *)parameter;

    while (InterlockedCompareExchange(&monitor->stop, 0, 0) == 0)
    {
        SIZE_T current_bytes = getPrivateUsageBytes();

        if (current_bytes > monitor->baseline_bytes)
        {
            double additional_kb =
                (double)(current_bytes - monitor->baseline_bytes) / 1024.0;

            if (additional_kb > monitor->peak_additional_kb)
                monitor->peak_additional_kb = additional_kb;
        }

        Sleep(1);
    }

    return 0;
}

BenchmarkResult benchmarkSort(
    SortFunction sort,
    const int original[],
    int n,
    int repetitions)
{
    BenchmarkResult result = {0.0, 0.0, 0};

    if (sort == NULL || original == NULL || n <= 0 || repetitions <= 0)
        return result;

    int *copy = copyArray(original, n);
    if (copy == NULL)
        return result;

    LARGE_INTEGER frequency;
    if (!QueryPerformanceFrequency(&frequency))
    {
        free(copy);
        return result;
    }

    double total_time_ms = 0.0;
    double maximum_memory_kb = 0.0;

    for (int run = 0; run < repetitions; run++)
    {
        for (int i = 0; i < n; i++)
            copy[i] = original[i];

        MemoryMonitor monitor;
        monitor.stop = 0;
        monitor.baseline_bytes = 0;
        monitor.peak_additional_kb = 0.0;

        HANDLE monitor_thread = CreateThread(
            NULL, 0, monitorMemory, &monitor, CREATE_SUSPENDED, NULL);

        if (monitor_thread == NULL)
        {
            free(copy);
            return result;
        }

        monitor.baseline_bytes = getPrivateUsageBytes();
        ResumeThread(monitor_thread);

        LARGE_INTEGER start;
        LARGE_INTEGER end;
        QueryPerformanceCounter(&start);
        sort(copy, n);
        QueryPerformanceCounter(&end);

        InterlockedExchange(&monitor.stop, 1);
        WaitForSingleObject(monitor_thread, INFINITE);
        CloseHandle(monitor_thread);

        if (!isSorted(copy, n))
        {
            free(copy);
            return result;
        }

        total_time_ms +=
            ((double)(end.QuadPart - start.QuadPart) * 1000.0) /
            (double)frequency.QuadPart;

        if (monitor.peak_additional_kb > maximum_memory_kb)
            maximum_memory_kb = monitor.peak_additional_kb;
    }

    result.time_ms = total_time_ms / repetitions;
    result.memory_kb = maximum_memory_kb;
    result.valid = 1;

    free(copy);
    return result;
}

int exportResultCSV(
    const char *filename,
    const char *input_type,
    const char *algorithm,
    int n,
    int repetitions,
    BenchmarkResult result)
{
    FILE *file = fopen(filename, "a");
    if (file == NULL)
        return 0;

    fprintf(file, "%s,%s,%d,%d,%.6f,%.2f,%s\n",
            input_type,
            algorithm,
            n,
            repetitions,
            result.time_ms,
            result.memory_kb,
            result.valid ? "PASS" : "FAIL");

    fclose(file);
    return 1;
}
