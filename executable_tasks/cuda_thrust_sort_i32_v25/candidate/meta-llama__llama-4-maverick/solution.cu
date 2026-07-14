#include "interface.h"
#include <thrust/device_ptr.h>
#include <thrust/sort.h>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
  cudaMemcpy(output, input, n * sizeof(int), cudaMemcpyDeviceToDevice);
  thrust::device_ptr<int> output_ptr(output);
  for (int iter = 0; iter < iters; ++iter) {
    thrust::sort(output_ptr, output_ptr + n);
  }
}