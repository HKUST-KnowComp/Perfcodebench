#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_SIZE = 256;
constexpr int ITEMS_PER_THREAD = 8;
constexpr int ITEMS_PER_BLOCK = BLOCK_SIZE * ITEMS_PER_THREAD;

__device__ __forceinline__ int warp_exclusive_scan(int val, int lane) {
    #pragma unroll
    for (int offset = 1; offset < 32; offset <<= 1) {
        int n = __shfl_up_sync(0xffffffff, val, offset);
        if (lane >= offset) val += n;
    }
    int result = __shfl_up_sync(0xffffffff, val, 1);
    return (lane == 0) ? 0 : result;
}

__device__ __forceinline__ int warp_reduce(int val) {
    #pragma unroll
    for (int offset = 16; offset > 0; offset >>= 1) {
        val += __shfl_down_sync(0xffffffff, val, offset);
    }
    return val;
}

__global__ void block_scan_kernel(const int* __restrict__ input, int* __restrict__ output,
                                   int* __restrict__ block_sums, int n) {
    __shared__ int shared[BLOCK_SIZE];
    __shared__ int warp_sums[8];
    
    const int tid = threadIdx.x;
    const int block_offset = blockIdx.x * ITEMS_PER_BLOCK;
    const int lane = tid & 31;
    const int warp_id = tid >> 5;
    
    int vals[ITEMS_PER_THREAD];
    int local_sum = 0;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = block_offset + tid + i * BLOCK_SIZE;
        vals[i] = (idx < n) ? input[idx] : 0;
        local_sum += vals[i];
    }
    
    int thread_exclusive = warp_exclusive_scan(local_sum, lane);
    
    if (lane == 31) {
        warp_sums[warp_id] = thread_exclusive + local_sum;
    }
    __syncthreads();
    
    if (tid < 8) {
        int ws = warp_sums[tid];
        int warp_exclusive = warp_exclusive_scan(ws, tid);
        warp_sums[tid] = warp_exclusive;
    }
    __syncthreads();
    
    int block_exclusive = thread_exclusive + warp_sums[warp_id];
    
    int running = block_exclusive;
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = block_offset + tid + i * BLOCK_SIZE;
        if (idx < n) {
            output[idx] = running;
        }
        running += vals[i];
    }
    
    if (tid == BLOCK_SIZE - 1) {
        block_sums[blockIdx.x] = running;
    }
}

__global__ void scan_block_sums_kernel(int* block_sums, int num_blocks) {
    __shared__ int shared[1024];
    
    int tid = threadIdx.x;
    int val = (tid < num_blocks) ? block_sums[tid] : 0;
    
    int lane = tid & 31;
    int warp_id = tid >> 5;
    
    int warp_scan = warp_exclusive_scan(val, lane);
    int warp_total = __shfl_sync(0xffffffff, warp_scan + val, 31);
    
    __shared__ int warp_totals[32];
    if (lane == 31) {
        warp_totals[warp_id] = warp_total;
    }
    __syncthreads();
    
    if (tid < 32) {
        int wt = (tid < ((num_blocks + 31) >> 5)) ? warp_totals[tid] : 0;
        int wt_scan = warp_exclusive_scan(wt, tid);
        warp_totals[tid] = wt_scan;
    }
    __syncthreads();
    
    int result = warp_scan + warp_totals[warp_id];
    
    if (tid < num_blocks) {
        block_sums[tid] = result;
    }
}

__global__ void add_block_sums_kernel(int* __restrict__ output, const int* __restrict__ block_sums, int n) {
    const int block_offset = blockIdx.x * ITEMS_PER_BLOCK;
    const int tid = threadIdx.x;
    const int block_sum = block_sums[blockIdx.x];
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = block_offset + tid + i * BLOCK_SIZE;
        if (idx < n) {
            output[idx] += block_sum;
        }
    }
}

int* d_block_sums = nullptr;
int block_sums_capacity = 0;

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;
    
    int num_blocks = (n + ITEMS_PER_BLOCK - 1) / ITEMS_PER_BLOCK;
    
    if (num_blocks > block_sums_capacity) {
        if (d_block_sums) cudaFree(d_block_sums);
        cudaMalloc(&d_block_sums, num_blocks * sizeof(int));
        block_sums_capacity = num_blocks;
    }
    
    for (int iter = 0; iter < iters; ++iter) {
        block_scan_kernel<<<num_blocks, BLOCK_SIZE>>>(input, output, d_block_sums, n);
        
        if (num_blocks > 1) {
            scan_block_sums_kernel<<<1, 1024>>>(d_block_sums, num_blocks);
            add_block_sums_kernel<<<num_blocks, BLOCK_SIZE>>>(output, d_block_sums, n);
        }
    }
}
