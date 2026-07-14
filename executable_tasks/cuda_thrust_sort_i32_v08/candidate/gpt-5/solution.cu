#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/sort.h>
#include <thrust/device_ptr.h>
#include <thrust/execution_policy.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) return;

  cudaStream_t stream;
  cudaStreamCreateWithFlags(&stream, cudaStreamNonBlocking);

  thrust::device_ptr<int> out_ptr = thrust::device_pointer_cast(output);

  for (int iter = 0; iter < iters; ++iter) {
    // Copy input to output on the device, then sort in-place
    cudaMemcpyAsync(output, input, static_cast<size_t>(n) * sizeof(int), cudaMemcpyDeviceToDevice, stream);
    thrust::sort(thrust::cuda::par.on(stream), out_ptr, out_ptr + n);
  }

  // Ensure all work is complete before returning
  cudaStreamSynchronize(stream);
  cudaStreamDestroy(stream);
}
