#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/sort.h>
void sort_vec_i32(const int* input, int* output, int n, int iters) {
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemcpy(output, input, n * sizeof(int), cudaMemcpyDeviceToDevice);
    thrust::sort(thrust::device_pointer_cast(output), thrust::device_pointer_cast(output) + n);
  }
}
