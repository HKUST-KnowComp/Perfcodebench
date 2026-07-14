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

__device__ __forceinline__ int warp_inclusive_scan(int val, int lane) {
    #pragma unroll
    for (int offset = 1; offset < 32; offset <<= 1) {
        int n = __shfl_up_sync(0xffffffff, val, offset);
        if (lane >= offset) val += n;
    }
    return val;
}

__global__ void block_scan_kernel(const int* __restrict__ input, int* __restrict__ output,
                                   int* __restrict__ block_sums, int n) {
    __shared__ int warp_totals[8];
    
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
    
    int warp_scan = warp_inclusive_scan(local_sum, lane);
    
    if (lane == 31) {
        warp_totals[warp_id] = warp_scan;
    }
    __syncthreads();
    
    int warp_prefix = 0;
    if (tid < 8) {
        int wt = warp_totals[tid];
        int ws = warp_exclusive_scan(wt, tid);
        warp_totals[tid] = ws;
    }
    __syncthreads();
    
    warp_prefix = warp_totals[warp_id];
    int thread_prefix = warp_scan - local_sum + warp_prefix;
    
    if (tid == BLOCK_SIZE - 1 && block_sums != nullptr) {
        block_sums[blockIdx.x] = warp_scan + warp_prefix;
    }
    
    int running = thread_prefix;
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = block_offset + tid + i * BLOCK_SIZE;
        if (idx < n) {
            output[idx] = running;
            running += vals[i];
        }
    }
}

__global__ void add_block_sums_kernel(int* __restrict__ output, const int* __restrict__ block_sums, int n) {
    const int tid = threadIdx.x;
    const int block_offset = (blockIdx.x + 1) * ITEMS_PER_BLOCK;
    
    int prefix = block_sums[blockIdx.x];
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = block_offset + tid + i * BLOCK_SIZE;
        if (idx < n) {
            output[idx] += prefix;
        }
    }
}

__global__ void scan_block_sums_kernel(int* __restrict__ block_sums, int num_blocks) {
    __shared__ int warp_totals[8];
    
    const int tid = threadIdx.x;
    const int lane = tid & 31;
    const int warp_id = tid >> 5;
    
    int val = (tid < num_blocks) ? block_sums[tid] : 0;
    
    int warp_scan = warp_inclusive_scan(val, lane);
    
    if (lane == 31) {
        warp_totals[warp_id] = warp_scan;
    }
    __syncthreads();
    
    int warp_prefix = 0;
    if (tid < 8) {
        int wt = warp_totals[tid];
        int ws = warp_exclusive_scan(wt, tid);
        warp_totals[tid] = ws;
    }
    __syncthreads();
    
    warp_prefix = warp_totals[warp_id];
    int exclusive_result = warp_scan - val + warp_prefix;
    
    if (tid < num_blocks) {
        block_sums[tid] = exclusive_result;
    }
}

__global__ void sequential_scan_kernel(const int* input, int* output, int n) {
    int running = 0;
    for (int i = 0; i < n; ++i) {
        output[i] = running;
        running += input[i];
    }
}

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;
    
    if (n <= 1024) {
        for (int iter = 0; iter < iters; ++iter) {
            sequential_scan_kernel<<<1, 1>>>(input, output, n);
        }
        return;
    }
    
    int num_blocks = (n + ITEMS_PER_BLOCK - 1) / ITEMS_PER_BLOCK;
    
    int* block_sums = nullptr;
    if (num_blocks > 1) {
        cudaMalloc(&block_sums, num_blocks * sizeof(int));
    }
    
    for (int iter = 0; iter < iters; ++iter) {
        block_scan_kernel<<<num_blocks, BLOCK_SIZE>>>(input, output, block_sums, n);
        
        if (num_blocks > 1) {
            scan_block_sums_kernel<<<1, 256>>>(block_sums, num_blocks);
            add_block_sums_kernel<<<num_blocks - 1, BLOCK_SIZE>>>(output, block_sums, n);
        }
    }
    
    if (block_sums) {
        cudaFree(block_sums);
    }
}