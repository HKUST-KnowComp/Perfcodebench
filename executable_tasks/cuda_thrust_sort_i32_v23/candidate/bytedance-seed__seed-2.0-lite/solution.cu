#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/sort.h>
#include <thrust/device_ptr.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
  for (int iter = 0; iter < iters; ++iter) {
    // Match original kernel's behavior: copy input to output first, then sort
    cudaMemcpy(output, input, n * sizeof(int), cudaMemcpyDeviceToDevice);
    // Wrap raw device pointers for Thrust to recognize device memory
    thrust::device_ptr<int> dev_output = thrust::device_pointer_cast(output);
    // Thrust's CUDA-accelerated sort, optimized for parallel GPU execution
    thrust::sort(dev_output, dev_output + n);
  }
}