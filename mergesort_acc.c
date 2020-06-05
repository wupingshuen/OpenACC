#include <assert.h>
#include <errno.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <time.h>
#include <stdbool.h>

/* create an array of N doubles */
static int create_arr(size_t const N, double ** const arrp)
{
  double * arr=NULL;
  if (!(arr=(double*)malloc(N*sizeof(*arr)))) {
    goto cleanup;
  }

  size_t i;

  for (i=0; i<N; ++i) {
    arr[i] = rand()%10; /* fill it with random values */
  }

  *arrp = arr;

  return 0;

  cleanup:
  free(arr);

  failure:
  return -1;
}


#pragma acc routine seq
int min(int x, int y) { return (x<y)? x :y; }

/* Function to merge the two haves arr[l..m] and arr[m+1..r] of array arr[] 
*/
 #pragma acc routine vector
 void merge(double arr[], int l, int m, int r)
{
  int i, j, k;
  int n1 = m - l + 1;
  int n2 =  r - m;

  /* create temp arrays */
  int *L, *R;
   L = (int *)malloc(sizeof(int) * n1); 
   R = (int *)malloc(sizeof(int) * n2);     
  /* Copy data to temp arrays L[] and R[] */
   #pragma acc loop independent 
  for (i = 0; i < n1; i++)
      L[i] = arr[l + i];
   #pragma acc loop independent 
  for (j = 0; j < n2; j++)
      R[j] = arr[m + 1+ j];

  /* Merge the temp arrays back into arr[l..r]*/
  i = 0;
  j = 0;
  k = l;

  #pragma acc loop independent private(k) reduction(+:j) private (L[0:n1]) private (R[0:n1]) private (arr[0:n1])reduction(+:i)
        //  private(k) reduction(+:j)
  while (i < n1 && j < n2)
  {
      if (L[i] <= R[j])
      {
          arr[k] = L[i];
          i++;
      }
      else
      {
          arr[k] = R[j];
          j++;
      }
      k++;
  }

  /* Copy the remaining elements of L[], if there are any */

  #pragma acc loop seq private (arr[0:n2]) 
  while (i < n1)
  {
      arr[k] = L[i];
      i++;
      k++;
  }

  /* Copy the remaining elements of R[], if there are any */
  #pragma acc loop seq private (arr[0:n2]) 
  while (j < n2)
  {
      arr[k] = R[j];
      j++;
      k++;
  }
  free(L);
  free(R);
}

/* Iterative mergesort function to sort arr[0...n-1] */
void mergeSort(double arr[], int n)
{
int curr_size;  // For current size of subarrays to be merged
               // curr_size varies from 1 to n/2
int left_start; // For picking starting index of left subarray
               // to be merged
#pragma acc kernels //pcopy(arr[0:n1])// pcopying (R[0:n2])
      { 
 #pragma acc loop independent
  for (curr_size=1; curr_size<=n-1; curr_size = 2*curr_size)
 {
  #pragma acc loop independent
   // Pick starting point of different subarrays of current size
   for (left_start=0; left_start<n-1; left_start += 2*curr_size)
   {
       // Find ending point of left subarray. mid+1 is starting 
       // point of right
       int mid = left_start + curr_size - 1;

       int right_end = min(left_start + 2*curr_size - 1, n-1);

       // Merge Subarrays arr[left_start...mid] & arr[mid+1...right_end]
       merge(arr, left_start, mid, right_end);
   }
 }
}}




bool check_sorted(const double * const array, const size_t N){
  for(size_t i=1; i <N; ++i){
    if(array[i] < array[i-1]){
      return false;
    }
  }
  return true;
}


static void print_time(double const seconds)
{
  printf("Search Time: %0.06fs\n", seconds);
}

int main(int argc, char * argv[])
{
  size_t N;
  double * A=NULL;
  clock_t ts, te;

  if (argc != 2) {
    fprintf(stderr, "usage: N\n");
    goto failure;
  }

  N = atoi(argv[1]);


  if (create_arr(N, &A)) {
    perror("error");
    goto failure;
  }

  size_t i;

  // for (i=0; i<N; ++i) {
  //   printf("%f ", A[i]); /* fill it with random values */
  // }

  ts = clock();
  mergeSort(A, N);
  te = clock();

  printf(check_sorted(A, N) ? "True\n" : "False\n");
  print_time((double)(te-ts)/CLOCKS_PER_SEC);


  free(A);

  return EXIT_SUCCESS;

  failure:
  if(A){
    free(A);
  }
  return EXIT_FAILURE;
}