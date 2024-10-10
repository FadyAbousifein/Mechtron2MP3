// CODE: include necessary library(s)
// you have to write all the functions and algorithms from scratch,
// You will submit this file, mySort.c holds the actual implementation of sorting functions
#include <stdlib.h> 
#include <math.h>
#include <stdbool.h>

void swap(int *x, int *y) {
    int temp = *x;
    *x = *y;
    *y = temp;
}

// Bubble Sort 
void bubbleSort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1])
                swap(&arr[j], &arr[j + 1]);
        } 
    }
}

// CODE: implement the algorithms for Insertion Sort, Merge Sort, Heap Sort, Counting sorting

// Insertion Sort 
void insertionSort(int arr[], int n) {
    for (int i = 0; i < n-1; i++) {
        int *minimum = &arr[i]; // set pointer to first element
        for (int j = i +1; j < n; j++) {
            if(arr[j] < *minimum) { // check wheather next element is smaller than the current minimum
                swap(&arr[j], minimum); // swap the two values 
            }
        }
    }

}

// Merge Sort
void mergeSort(int arr[], int left, int right) {
    int middle; 
    if (left < right) {
        middle  = (left + right)/2; // arr[left...middle...right]
        mergeSort(arr, left, middle); 
        mergeSort(arr, middle + 1, right); 
    }
    
    int size_subArray1 = middle - left + 1; //size of arr[left...middle]
    int size_subArray2 = right - middle;  // size of arr[middle + 1...right]

    // initiallizing both sub arrays
    int subArray1[size_subArray1];
    int subArray2[size_subArray2]; 

    // inserting the proper values into the sub arrays
    for (int i = 0; i < size_subArray1; i++) {
        subArray1[i] = arr[left + i]; 
    }
    for (int i = 0; i < size_subArray2; i++) {
        subArray2[i] = arr[middle + i + 1]; 
    }

    int i = 0; int j = 0; int k = left; 

    while (i < size_subArray1 && j < size_subArray2) {
        if (subArray1[i] <= subArray2[j]) {
            arr[k] = subArray1[i];
            i++; 
        } 
        else {
            arr[k] = subArray2[j];
            j++;
        }
        k++; 
    }

    while (i < size_subArray1) {
        arr[k] = subArray1[i];
        i++; 
        k++;
    }

    while (k < size_subArray2) {
        arr[k] = subArray2[j]; 
        j++; 
        k++;
    }
}
