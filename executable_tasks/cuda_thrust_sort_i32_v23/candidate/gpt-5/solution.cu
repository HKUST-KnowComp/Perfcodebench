#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/sort.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
  if (n <= 1 || iters <= 0) return;
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemcpyAsync(output, input, static_cast<size_t>(n) * sizeof(int), cudaMemcpyDeviceToDevice, 0);
    thrust::device_ptr<int> out_ptr(output);
    thrust::sort(out_ptr, out_ptr + n);
  }
}
