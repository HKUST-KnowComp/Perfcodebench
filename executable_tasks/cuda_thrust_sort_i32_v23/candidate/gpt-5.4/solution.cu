#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/copy.h>
#include <thrust/sort.h>

namespace {

inline int div_up(int a, int b) {
  return (a + b - 1) / b;
}

__global__ void copy_kernel(const int* __restrict__ input,
                            int* __restrict__ output,
                            int n) {
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (idx < n) output[idx] = input[idx];
}

}  // namespace

void sort_vec_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) return;

  // Sort in-place on the provided output buffer to avoid extra temporary
  // allocations/copies inside the timed loop.
  const int threads = 256;
  const int blocks = div_up(n, threads);

  for (int iter = 0; iter < iters; ++iter) {
    copy_kernel<<<blocks, threads>>>(input, output, n);
    thrust::device_ptr<int> out_ptr(output);
    thrust::sort(out_ptr, out_ptr + n);
  }
}
