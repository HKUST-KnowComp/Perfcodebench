#include "interface.h"
#include <thrust/sort.h>
#include <thrust/device_ptr.h>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
  int* d_input;
  int* d_output;
  cudaMalloc((void**)&d_input, n * sizeof(int));
  cudaMalloc((void**)&d_output, n * sizeof(int));
  cudaMemcpy(d_input, input, n * sizeof(int), cudaMemcpyHostToDevice);
  for (int iter = 0; iter < iters; ++iter) {
    thrust::device_ptr<int> d_output_ptr(d_output);
    thrust::copy(thrust::device_pointer_cast(d_input), thrust::device_pointer_cast(d_input) + n, d_output_ptr);
    thrust::sort(d_output_ptr, d_output_ptr + n);
  }
  cudaMemcpy(output, d_output, n * sizeof(int), cudaMemcpyDeviceToHost);
  cudaFree(d_input);
  cudaFree(d_output);
}