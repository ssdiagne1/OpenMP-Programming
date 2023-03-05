#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <chrono>

using namespace std;
using seconds = chrono::seconds;
using check_time = std::chrono::high_resolution_clock;

#ifdef __cplusplus
extern "C" {
#endif
  void generatePrefixSumData (int* arr, size_t n);
  void checkPrefixSumResult (int* arr, size_t n);
#ifdef __cplusplus
}
#endif


int main (int argc, char* argv[]) {

  //Starting the clock time
  auto initiated = check_time::now();

  
  if (argc < 3) {
    std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  
  int n = atoi(argv[1]);

  int * arr = new int [n];
  int * pr = new int [n+1];
  generatePrefixSumData (arr, n);
  int nbthreads = atoi(argv[2]);
  int chunkSize = n/nbthreads;
 
  
  int * intermediateArr = new int [nbthreads];
  int * arrayIndex = new int [nbthreads];

 
  intermediateArr[0] = 0;
  arrayIndex[0] = 0;
  
  omp_set_num_threads(nbthreads);


#pragma omp parallel
  {
    int fd = open (argv[0], O_RDONLY);
    if (fd != -1) {
      close (fd);
    }
    else {
      std::cerr<<"something is amiss"<<std::endl;
    }

   pr[0] = 0;

  #pragma omp for schedule(static)
  for(int j = 0; j < nbthreads; j++)
   {
    for(int i= j*chunkSize;i<(j+1)*chunkSize; i++)
     {
      if(i== (j * chunkSize))
     {
        pr[i+1] = 0 + arr[i];
      }
      else
      {
        pr[i+1] = pr[i] + arr[i];
      }
      intermediateArr[j+ 1] = pr[i+1];  
     }
      arrayIndex[j +1] = ((j +1) * chunkSize)+1;
  }
}

    if(nbthreads * chunkSize < n)
    {
        for(int i = chunkSize * nbthreads; i < n; i++)
      {
          pr[i+1] = pr[i] + arr[i];
        }
      }


  int * left_sum = new int [nbthreads];
left_sum[0] = intermediateArr[0];
  for(int i = 1; i<nbthreads; i++)
  {
    left_sum[i] = left_sum[i-1] + intermediateArr[i];
  }


#pragma omp parallel for schedule(dynamic)
  for(int i =0;i<nbthreads; i++)
  {
    for(int j=arrayIndex[i]; j < arrayIndex[i+1]; j++)
  {
    pr[j] = pr[j] + left_sum[i];
    }


  }


      if(chunkSize * nbthreads < n)
     {
        for(int j = nbthreads * chunkSize; j < n; j++)
       {
          pr[j+1] = pr[j+1] + left_sum[nbthreads-1];
        }
      }

  checkPrefixSumResult(pr, n);

  delete[] arr;

  //stopping the clock 
  auto end = check_time::now();
  auto time_elapsed = end - initiated; // calculating the time required
  auto secs = std::chrono::duration_cast<std::chrono::duration<float>>(time_elapsed);

  std::cerr<< secs.count();

  return 0;
}








































































































































































































































































































































































