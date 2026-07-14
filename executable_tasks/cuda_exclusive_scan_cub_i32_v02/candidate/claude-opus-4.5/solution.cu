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
    int thread_sum = 0;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = block_offset + tid + i * BLOCK_SIZE;
        vals[i] = (idx < n) ? input[idx] : 0;
        thread_sum += vals[i];
    }
    
    int thread_exclusive = warp_exclusive_scan(thread_sum, lane);
    
    if (lane == 31) {
        warp_totals[warp_id] = thread_exclusive + thread_sum;
    }
    __syncthreads();
    
    int warp_offset = 0;
    if (tid < 8) {
        int wt = warp_totals[tid];
        int scanned = warp_exclusive_scan(wt, tid);
        warp_totals[tid] = scanned;
        if (tid == 7) {
            block_sums[blockIdx.x] = scanned + wt;
        }
    }
    __syncthreads();
    
    warp_offset = warp_totals[warp_id];
    int base = warp_offset + thread_exclusive;
    
    int running = base;
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = block_offset + tid + i * BLOCK_SIZE;
        if (idx < n) {
            output[idx] = running;
        }
        running += vals[i];
    }
}

__global__ void scan_block_sums_kernel(int* block_sums, int num_blocks) {
    __shared__ int temp[1024];
    
    int tid = threadIdx.x;
    int val = (tid < num_blocks) ? block_sums[tid] : 0;
    
    int lane = tid & 31;
    int warp_id = tid >> 5;
    
    int scanned = warp_inclusive_scan(val, lane);
    
    if (lane == 31) {
        temp[warp_id] = scanned;
    }
    __syncthreads();
    
    if (tid < 32) {
        int warp_sum = (tid < ((num_blocks + 31) >> 5)) ? temp[tid] : 0;
        temp[tid] = warp_exclusive_scan(warp_sum, tid);
    }
    __syncthreads();
    
    int warp_offset = temp[warp_id];
    int exclusive = (lane == 0) ? warp_offset : (__shfl_up_sync(0xffffffff, scanned, 1) + warp_offset);
    
    if (tid < num_blocks) {
        block_sums[tid] = exclusive;
    }
}

__global__ void add_block_sums_kernel(int* __restrict__ output, const int* __restrict__ block_sums, int n) {
    int block_offset = blockIdx.x * ITEMS_PER_BLOCK;
    int tid = threadIdx.x;
    int add_val = block_sums[blockIdx.x];
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = block_offset + tid + i * BLOCK_SIZE;
        if (idx < n) {
            output[idx] += add_val;
        }
    }
}

__global__ void small_scan_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    __shared__ int temp[1024];
    
    int tid = threadIdx.x;
    int val = (tid < n) ? input[tid] : 0;
    
    int lane = tid & 31;
    int warp_id = tid >> 5;
    
    int scanned = warp_inclusive_scan(val, lane);
    
    if (lane == 31) {
        temp[warp_id] = scanned;
    }
    __syncthreads();
    
    if (tid < 32) {
        int warp_sum = (tid < 32) ? temp[tid] : 0;
        temp[tid] = warp_exclusive_scan(warp_sum, tid);
    }
    __syncthreads();
    
    int warp_offset = temp[warp_id];
    int exclusive = (lane == 0) ? warp_offset : (__shfl_up_sync(0xffffffff, scanned, 1) + warp_offset);
    
    if (tid < n) {
        output[tid] = exclusive;
    }
}

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;
    
    if (n <= 1024) {
        for (int iter = 0; iter < iters; ++iter) {
            small_scan_kernel<<<1, 1024>>>(input, output, n);
        }
        return;
    }
    
    int num_blocks = (n + ITEMS_PER_BLOCK - 1) / ITEMS_PER_BLOCK;
    
    int* block_sums = nullptr;
    cudaMalloc(&block_sums, num_blocks * sizeof(int));
    
    for (int iter = 0; iter < iters; ++iter) {
        block_scan_kernel<<<num_blocks, BLOCK_SIZE>>>(input, output, block_sums, n);
        
        if (num_blocks > 1) {
            scan_block_sums_kernel<<<1, 1024>>>(block_sums, num_blocks);
            add_block_sums_kernel<<<num_blocks, BLOCK_SIZE>>>(output, block_sums, n);
        }
    }
    
    cudaFree(block_sums);
}