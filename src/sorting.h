
#ifndef SORTING_H
#define SORTING_H

void BubbleSort(int array[], int n);
void SelectionSort(int array[], int n);
void InsertionSort(int array[], int n);

void merge(int arr[], int left, int mid, int right);
void mergeSort(int arr[], int left, int right);

int partition(int arr[], int low, int high);
void quickSort(int arr[], int low, int high);

#endif
