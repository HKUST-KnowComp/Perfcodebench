#include "interface.h"
#include <cuda_runtime.h>

namespace {

__device__ __forceinline__ int warp_reduce_sum(int val) {
  #pragma unroll
  for (int offset = 16; offset > 0; offset >>= 1)
    val += __shfl_down_sync(0xffffffff, val, offset);
  return val;
}

__global__ void reduce_kernel(const int* input, int* output, int n) {
  int tid = threadIdx.x;
  int bid = blockIdx.x;
  int block_size = blockDim.x;
  int grid_size = gridDim.x * block_size;
  int idx = bid * block_size + tid;

  int sum = 0;
  while (idx < n) {
    sum += input[idx];
    idx += grid_size;
  }

  // Warp-level reduction
  sum = warp_reduce_sum(sum);

  // Write warp results to shared memory
  __shared__ int warp_sum[32];  // enough for up to 1024 threads (32 warps)
  if (tid % 32 == 0) {
    warp_sum[tid / 32] = sum;
  }
  __syncthreads();

  // Final block reduction by thread 0
  if (tid == 0) {
    int num_warps = (block_size + 31) / 32;
    int block_sum = 0;
    for (int i = 0; i < num_warps; ++i) {
      block_sum += warp_sum[i];
    }
    atomicAdd(output, block_sum);
  }
}

}  // anonymous namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  if (n == 0) {
    // sum is zero; ensure output is zero (already zero from previous iter if any)
    for (int iter = 0; iter < iters; ++iter) {
      cudaMemset(output, 0, sizeof(int));
    }
    return;
  }

  const int block_size = 256;
  const int grid_size = min(1024, (n + block_size - 1) / block_size);

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    reduce_kernel<<<grid_size, block_size>>>(input, output, n);
  }
}