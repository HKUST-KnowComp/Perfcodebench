#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/sort.h>
#include <thrust/execution_policy.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
  if (iters <= 0) return;
  cudaStream_t stream = 0; // use default stream for timing consistency

  if (n <= 0) return;
  if (n == 1) {
    // Only copy the single element per iteration
    for (int iter = 0; iter < iters; ++iter) {
      cudaMemcpyAsync(output, input, sizeof(int), cudaMemcpyDeviceToDevice, stream);
    }
    return;
  }

  for (int iter = 0; iter < iters; ++iter) {
    // Copy input to output each iteration
    cudaMemcpyAsync(output, input, static_cast<size_t>(n) * sizeof(int), cudaMemcpyDeviceToDevice, stream);

    // Sort in place on the device using Thrust (radix sort for int)
    thrust::device_ptr<int> begin(output);
    thrust::device_ptr<int> end = begin + n;
    thrust::sort(thrust::cuda::par.on(stream), begin, end);
  }
}
