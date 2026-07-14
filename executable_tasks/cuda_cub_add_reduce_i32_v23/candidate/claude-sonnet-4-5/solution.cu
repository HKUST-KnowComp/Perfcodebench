#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void optimized_reduce_kernel(const int* input, int* output, int n) {
  __shared__ int shared[256];
  
  int tid = threadIdx.x;
  int idx = blockIdx.x * blockDim.x * 4 + threadIdx.x;
  int stride = blockDim.x * gridDim.x * 4;
  
  int sum = 0;
  for (int i = idx; i < n; i += stride) {
    sum += input[i];
    if (i + blockDim.x < n) sum += input[i + blockDim.x];
    if (i + blockDim.x * 2 < n) sum += input[i + blockDim.x * 2];
    if (i + blockDim.x * 3 < n) sum += input[i + blockDim.x * 3];
  }
  
  shared[tid] = sum;
  __syncthreads();
  
  for (int s = 128; s > 32; s >>= 1) {
    if (tid < s) shared[tid] += shared[tid + s];
    __syncthreads();
  }
  
  if (tid < 32) {
    int warp_sum = shared[tid];
    warp_sum += shared[tid + 32];
    for (int offset = 16; offset > 0; offset >>= 1) {
      warp_sum += __shfl_down_sync(0xffffffff, warp_sum, offset);
    }
    if (tid == 0) atomicAdd(output, warp_sum);
  }
}

}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    optimized_reduce_kernel<<<2048, 256>>>(input, output, n);
  }
}
