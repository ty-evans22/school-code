#include <stdio.h>

void printArray(int *arr, int length) {
    for (int i = 0; i < length; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

/*
* Overwrites the final element, and shifts intermediate elements to the right, leaving a hole at the position.
* @param arr - the array of ints
* @param pos - the position in the array
* @param final - index of the final element in the array
*/
void shiftRight(int *arr, int pos, int final) {
    for (int i = final - 1; i >= pos; i--) {
        arr[i + 1] = arr[i];
    }
}

int findSortedPos(int *arr, int val, int final) {
    for (int i = 0; i < final; i++) {
        if (arr[i] > val) {
            return i;
        }
    }
}

/*
* @param arr - partially sorted array of ints
* @param pos - index of first item in array not in sorted position
* @param last - index of last element in array
*/
void insertSortedPos(int *arr, int pos, int final) {
    int v = arr[pos];
    int p = findSortedPos(arr, v, final);
    shiftRight(arr, p, pos);
    arr[p] = v;
}

void insertionSort(int *arr, int length) {
    for (int i = 1; i < length; i++) {
        insertSortedPos(arr, i, length - 1);
    }
}

void fill(int *arr, int length) {
    for (int i = 0; i < length; i++) {
        arr[i] = length - i;
    }
}

void main() {
    int arr[10] = {0};
    fill(arr, 10);
    insertionSort(arr, 10);
}5