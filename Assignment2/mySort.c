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
        middle = left + (right -left)/2; // arr[left...middle...right]
        mergeSort(arr, left, middle); 
        mergeSort(arr, middle + 1, right); 
    }
    else {
        return; 
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

    int i = 0; int j = 0; int k = left; // indexes for subarray1, 2, and arr respectivley 

    // merge two subarrays in sorted order, continue until all elements in one subarry is fully traversed
    while (i < size_subArray1 && j < size_subArray2) {
        if (subArray1[i] <= subArray2[j]) {
            swap(&arr[k], &subArray1[i]); 
            //arr[k] = subArray1[i];
            i++; 
        } 
        else {
            swap(&arr[k], &subArray2[j]);
            // arr[k] = subArray2[j];
            j++;
        }
        k++; 
    }

    // remaining elments are then just copied into the original array
    while (i < size_subArray1) {
        swap(&arr[k], &subArray1[i]); 
        i++; 
        k++;
    }

    while (j < size_subArray2) {
        swap(&arr[k], &subArray2[j]);
        j++; 
        k++;
    }
}

// Heap Sort 

void heapify(int arr[], int n, int i) {
    // alters the heap ensuring the parent contains the maximum/largest number 
    // while the children are always smaller 
    int maximum = i; 
    int left = 2*i + 1; 
    int right = 2*i +2; 

    if (left < n && arr[left] > arr[maximum]) maximum = left; 
    if (right < n && arr[right] > arr[maximum]) maximum = right; 
    if (maximum != i) {
        swap(&arr[i], &arr[maximum]);
        heapify(arr,n,maximum); 
    }
} 

void heapSort(int arr[], int n) {
    // construct the max heap 
    for (int i = n/2 -1; i >=0; i--) {
        heapify(arr,n,i); 
    }

    // actual sorting of the heap 
    for (int i = n - 1; i >= 0; i--) {
        swap(&arr[0], &arr[i]); 

        heapify(arr,i,0); // sorting will ruin the max heap therefore call heapify again
    }
}

// Counting Sort

void countingSort(int arr[], int n) {
	
    // initialize max and min variables
    int maximum = 0;
	int minimum = 0;

    // actually find max and min elements
	for (int i = 0; i < n; i++) {
		if (arr[i] > maximum) {
            maximum = arr[i];
        }

		if (arr[i] < minimum) {
            minimum = arr[i];
        }
	}

	
	int countArray[abs(minimum) + maximum + 1]; // abs(min) + max + 1 ensures count contains all values from min - max
	
    // set all elements to 0
    for (int i = 0; i < abs(minimum) + maximum + 1; i++) {
		countArray[i] = 0;
	}

    // determine the number of times a number appears in arr
	for (int i = 0; i < n; i++) {
		countArray[arr[i]+abs(minimum)]++;
	}

    // replace the number of each element in the countArray into arr
	int k = 0;
	for (int i = minimum; i <= maximum; i++) {
		for (int j = 0; j < countArray[i+abs(minimum)]; j++) {
			arr[k] = i;
			k++;
		}
	}
}
