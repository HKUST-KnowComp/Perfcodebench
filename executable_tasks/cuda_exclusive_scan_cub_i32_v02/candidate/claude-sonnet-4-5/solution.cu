#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_SIZE = 256;
constexpr int WARP_SIZE = 32;

__device__ __forceinline__ int warp_scan_exclusive(int val, int lane) {
  #pragma unroll
  for (int offset = 1; offset < WARP_SIZE; offset <<= 1) {
    int temp = __shfl_up_sync(0xffffffff, val, offset);
    if (lane >= offset) val += temp;
  }
  int result = __shfl_up_sync(0xffffffff, val, 1);
  return (lane == 0) ? 0 : result;
}

__global__ void block_scan_kernel(const int* input, int* output, int* block_sums, int n) {
  __shared__ int shared[BLOCK_SIZE];
  __shared__ int warp_sums[BLOCK_SIZE / WARP_SIZE];
  
  int tid = threadIdx.x;
  int gid = blockIdx.x * BLOCK_SIZE + tid;
  int lane = tid & (WARP_SIZE - 1);
  int warp_id = tid / WARP_SIZE;
  
  int val = (gid < n) ? input[gid] : 0;
  
  int warp_scan = warp_scan_exclusive(val, lane);
  
  if (lane == WARP_SIZE - 1) {
    warp_sums[warp_id] = warp_scan + val;
  }
  __syncthreads();
  
  if (warp_id == 0) {
    int warp_sum = (tid < (BLOCK_SIZE / WARP_SIZE)) ? warp_sums[tid] : 0;
    int scanned_warp_sum = warp_scan_exclusive(warp_sum, lane);
    if (tid < (BLOCK_SIZE / WARP_SIZE)) {
      warp_sums[tid] = scanned_warp_sum;
    }
  }
  __syncthreads();
  
  int block_offset = (warp_id > 0) ? warp_sums[warp_id] : 0;
  int result = warp_scan + block_offset;
  
  if (gid < n) {
    output[gid] = result;
  }
  
  if (block_sums != nullptr && tid == BLOCK_SIZE - 1) {
    block_sums[blockIdx.x] = result + val;
  }
}

__global__ void add_block_sums_kernel(int* output, const int* block_sums, int n) {
  int gid = (blockIdx.x + 1) * BLOCK_SIZE + threadIdx.x;
  if (gid < n) {
    output[gid] += block_sums[blockIdx.x];
  }
}

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0) return;
  
  int num_blocks = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;
  
  int* block_sums = nullptr;
  int* block_sums_scanned = nullptr;
  
  if (num_blocks > 1) {
    cudaMalloc(&block_sums, num_blocks * sizeof(int));
    cudaMalloc(&block_sums_scanned, num_blocks * sizeof(int));
  }
  
  for (int iter = 0; iter < iters; ++iter) {
    block_scan_kernel<<<num_blocks, BLOCK_SIZE>>>(input, output, block_sums, n);
    
    if (num_blocks > 1) {
      block_scan_kernel<<<1, BLOCK_SIZE>>>(block_sums, block_sums_scanned, nullptr, num_blocks);
      add_block_sums_kernel<<<num_blocks - 1, BLOCK_SIZE>>>(output, block_sums_scanned, n);
    }
  }
  
  if (block_sums) cudaFree(block_sums);
  if (block_sums_scanned) cudaFree(block_sums_scanned);
}