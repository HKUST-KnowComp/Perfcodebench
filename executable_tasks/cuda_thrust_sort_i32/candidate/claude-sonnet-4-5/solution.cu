#include "interface.h"
#include <thrust/device_ptr.h>
#include <thrust/sort.h>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemcpy(output, input, n * sizeof(int), cudaMemcpyDeviceToDevice);
    thrust::device_ptr<int> dev_ptr(output);
    thrust::sort(dev_ptr, dev_ptr + n);
  }
}
