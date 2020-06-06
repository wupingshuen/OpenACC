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
  // double* arr = new double[3000000];
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


static void merge(double * const arr, int l, int m, int r) 
{ 
	int i, j, k; 
	int n1 = m - l + 1; 
	int n2 = r - m; 

	/* create temp arrays */
	double * L=NULL;
	double * R=NULL;
  	if (!(L=(double*)malloc(n1*sizeof(*arr)))) {
  	}
  	if (!(R=(double*)malloc(n2*sizeof(*arr)))) {
  	}

	/* Copy data to temp arrays L[] and R[] */
	for (i = 0; i < n1; i++) 
		L[i] = arr[l + i]; 
	for (j = 0; j < n2; j++) 
		R[j] = arr[m + 1+ j]; 

	/* Merge the temp arrays back into arr[l..r]*/
	i = 0; // Initial index of first subarray 
	j = 0; // Initial index of second subarray 
	k = l; // Initial index of merged subarray 
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

	/* Copy the remaining elements of L[], if there 
	are any */
	while (i < n1) 
	{ 
		arr[k] = L[i]; 
		i++; 
		k++; 
	} 

	/* Copy the remaining elements of R[], if there 
	are any */
	while (j < n2) 
	{ 
		arr[k] = R[j]; 
		j++; 
		k++; 
	} 

	free(L);
	free(R);
} 

/* l is for left index and r is right index of the 
sub-array of arr to be sorted */
void mergeSort(double * const arr, int l, int r) 
{ 
	if (l < r) 
	{ 
		// Same as (l+r)/2, but avoids overflow for 
		// large l and h 
		int m = l+(r-l)/2; 

		// Sort first and second halves 
		mergeSort(arr, l, m); 
		mergeSort(arr, m+1, r); 

		merge(arr, l, m, r); 
	} 
} 

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
  mergeSort(A, 0, N-1);
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