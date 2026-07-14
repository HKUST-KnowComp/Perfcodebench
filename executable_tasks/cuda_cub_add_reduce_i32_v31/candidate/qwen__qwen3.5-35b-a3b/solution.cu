#include "interface.h"
#include <cuda_runtime.h>
namespace {
__global__ void atomic_reduce_kernel(const int* input, int* output, int n) {
  extern __shared__ int shared[];
  int tid = threadIdx.x;
  int block_id = blockIdx.x;
  int stride = blockDim.x * gridDim.x;
  int local_sum = 0;

  for (int idx = block_id * blockDim.x + tid; idx < n; idx += stride) {
    local_sum += input[idx];
  }

  shared[tid] = local_sum;
  __syncthreads();

  for (int offset = blockDim.x / 2; offset > 0; offset >>= 1) {
    if (tid < offset) {
      shared[tid] += shared[tid + offset];
    }
    __syncthreads();
  }

  if (tid == 0) {
    atomicAdd(output, shared[0]);
  }
}
}  // namespace
void add_reduce_i32(const int* input, int* output, int n, int iters) {
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    atomic_reduce_kernel<<<1024, 256, 256 * sizeof(int)>>>(input, output, n);
  }
}