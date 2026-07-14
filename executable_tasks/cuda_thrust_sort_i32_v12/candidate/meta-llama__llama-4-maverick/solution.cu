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
    cudaMemcpy(d_output, d_input, n * sizeof(int), cudaMemcpyDeviceToDevice);
    thrust::device_ptr<int> output_ptr(d_output);
    thrust::sort(output_ptr, output_ptr + n);
    cudaMemcpy(output, d_output, n * sizeof(int), cudaMemcpyDeviceToHost);
  }
  cudaFree(d_input);
  cudaFree(d_output);
}