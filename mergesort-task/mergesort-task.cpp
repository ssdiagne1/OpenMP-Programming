#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <chrono>

using namespace std;
#ifdef __cplusplus
extern "C" {
#endif
  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (int* arr, size_t n);
#ifdef __cplusplus
}
#endif

constexpr int task_threshold = 5000;
void merge(int arr[], int tmp_arr[], int first, int middle, int last) {
  // Merge two portions of array, and put result into a temporary array tmp_arr[]
  int p1 = first;
  int p2 = middle;
  int p = first;
  while (p <= last) {
    if (p1 < middle && (p2 > last || arr[p1] < arr[p2])) {
      tmp_arr[p++] = arr[p1++];
    } else {
      tmp_arr[p++] = arr[p2++];
    }
  }

  // Copy sorted portion from the temporary array to the original array
  for (int i = first; i <= last; ++i) {
    arr[i] = tmp_arr[i];
  }
}


void mergeSort(int arr[], int tmp_arr[], int first, int last) {
  if (first < last) {
    int middle = (first + last + 1) / 2; 
    mergeSort(arr, tmp_arr, first, middle - 1);
    mergeSort(arr, tmp_arr, middle, last);
    merge(arr, tmp_arr, first, middle, last);
  }
}


void openMPSorting(int arr[], int tmp_arr[], int first, int last) {
  if (first < last) {
    int middle = (first + last + 1) / 2; 
   
    // Spliting the  list  One is [first:middle-1], and another is [middle:last].
    // the list is split a[first:last] into two halves (called sublists).
    // For better performance, only when the list got enough values
    if (last - first < task_threshold) {
      mergeSort(arr, tmp_arr, first, middle - 1);
      mergeSort(arr, tmp_arr, middle, last);
    } else {
#pragma omp task
      openMPSorting(arr, tmp_arr, first, middle - 1);
#pragma omp task
      openMPSorting(arr, tmp_arr, middle, last);
#pragma omp taskwait
    }
    merge(arr, tmp_arr, first, middle, last);
  }
}
int main (int argc, char* argv[]) {
auto start = chrono::steady_clock::now();
  
#pragma omp parallel
  {
    int fd = open (argv[0], O_RDONLY);
    if (fd != -1) {
      close (fd);
    }
    else {
      std::cerr<<"something is amiss"<<std::endl;
    }
  }
 
  if (argc < 3) { std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  int nbthreads = atoi(argv[2]);
 
  // get arr data
  int * arr = new int [n];
int * temparr = new int [n];
  generateMergeSortData (arr, n);

#pragma omp parallel
  {
    int fd = open (argv[0], O_RDONLY);
    if (fd != -1) {
      close (fd);
    }
    else {
      std::cerr<<"something is amiss"<<std::endl;
    }
   #pragma omp single
   { 
  	openMPSorting(arr, temparr,0, n - 1); 
   }
    
}
 
  checkMergeSortResult (arr, n);
 
  delete[] arr;
  // end time
  auto timeend = chrono::steady_clock::now();
  std::chrono::duration<double> elapsed = timeend - start;
  cerr<<elapsed.count();
  return 0;
}

