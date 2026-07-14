#include "interface.h"
#include <thrust/device_ptr.h>
#include <thrust/sort.h>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
  int* d_output;
  cudaMalloc((void**)&d_output, n * sizeof(int));
  cudaMemcpy(d_output, input, n * sizeof(int), cudaMemcpyHostToDevice);
  for (int iter = 0; iter < iters; ++iter) {
    thrust::device_ptr<int> d_ptr = thrust::device_pointer_cast(d_output);
    thrust::sort(d_ptr, d_ptr + n);
  }
  cudaMemcpy(output, d_output, n * sizeof(int), cudaMemcpyDeviceToHost);
  cudaFree(d_output);
}