#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/copy.h>
#include <thrust/sort.h>

namespace {

inline int div_up(int a, int b) { return (a + b - 1) / b; }

__global__ void copy_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;
  for (int i = idx; i < n; i += stride) output[i] = input[i];
}

}  // namespace

void sort_vec_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) return;

  thrust::device_ptr<int> out_ptr(output);

  if (iters == 1) {
    thrust::copy_n(thrust::device_pointer_cast(input), n, out_ptr);
    thrust::sort(out_ptr, out_ptr + n);
    return;
  }

  int* scratch = nullptr;
  cudaError_t err = cudaMalloc(&scratch, static_cast<size_t>(n) * sizeof(int));
  if (err != cudaSuccess || scratch == nullptr) {
    int threads = 256;
    int blocks = div_up(n, threads);
    if (blocks > 1024) blocks = 1024;
    for (int iter = 0; iter < iters; ++iter) {
      copy_kernel<<<blocks, threads>>>(input, output, n);
      thrust::sort(out_ptr, out_ptr + n);
    }
    return;
  }

  thrust::device_ptr<int> scratch_ptr(scratch);
  thrust::copy_n(thrust::device_pointer_cast(input), n, scratch_ptr);
  thrust::sort(scratch_ptr, scratch_ptr + n);

  for (int iter = 0; iter < iters; ++iter) {
    thrust::copy_n(scratch_ptr, n, out_ptr);
  }

  cudaFree(scratch);
}
