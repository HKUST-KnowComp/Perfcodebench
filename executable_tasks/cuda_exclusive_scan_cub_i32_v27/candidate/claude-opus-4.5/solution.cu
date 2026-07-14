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
    return __shfl_up_sync(0xffffffff, val, 1);
}

__device__ __forceinline__ int warp_inclusive_scan(int val, int lane) {
    #pragma unroll
    for (int offset = 1; offset < 32; offset <<= 1) {
        int n = __shfl_up_sync(0xffffffff, val, offset);
        if (lane >= offset) val += n;
    }
    return val;
}

__global__ void single_block_scan(const int* __restrict__ input, int* __restrict__ output, int n) {
    __shared__ int shared[BLOCK_SIZE];
    __shared__ int warp_sums[8];
    
    int tid = threadIdx.x;
    int lane = tid & 31;
    int warp_id = tid >> 5;
    
    int local[ITEMS_PER_THREAD];
    int base = tid * ITEMS_PER_THREAD;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = base + i;
        local[i] = (idx < n) ? input[idx] : 0;
    }
    
    int thread_sum = 0;
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int tmp = local[i];
        local[i] = thread_sum;
        thread_sum += tmp;
    }
    
    int thread_prefix = warp_exclusive_scan(thread_sum, lane);
    if (lane == 0) thread_prefix = 0;
    
    int warp_total = __shfl_sync(0xffffffff, thread_prefix + thread_sum, 31);
    if (lane == 31) {
        warp_sums[warp_id] = warp_total;
    }
    __syncthreads();
    
    int block_prefix = 0;
    if (tid < 8) {
        int ws = warp_sums[tid];
        int scanned = warp_exclusive_scan(ws, tid);
        if (tid == 0) scanned = 0;
        warp_sums[tid] = scanned;
    }
    __syncthreads();
    
    block_prefix = warp_sums[warp_id];
    int final_prefix = block_prefix + thread_prefix;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = base + i;
        if (idx < n) {
            output[idx] = local[i] + final_prefix;
        }
    }
}

__global__ void block_scan_phase1(const int* __restrict__ input, int* __restrict__ output, 
                                   int* __restrict__ block_sums, int n) {
    __shared__ int warp_sums[8];
    
    int tid = threadIdx.x;
    int bid = blockIdx.x;
    int lane = tid & 31;
    int warp_id = tid >> 5;
    
    int global_base = bid * ITEMS_PER_BLOCK + tid * ITEMS_PER_THREAD;
    
    int local[ITEMS_PER_THREAD];
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = global_base + i;
        local[i] = (idx < n) ? input[idx] : 0;
    }
    
    int thread_sum = 0;
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int tmp = local[i];
        local[i] = thread_sum;
        thread_sum += tmp;
    }
    
    int thread_prefix = warp_exclusive_scan(thread_sum, lane);
    if (lane == 0) thread_prefix = 0;
    
    int warp_total = __shfl_sync(0xffffffff, thread_prefix + thread_sum, 31);
    if (lane == 31) {
        warp_sums[warp_id] = warp_total;
    }
    __syncthreads();
    
    if (tid < 8) {
        int ws = warp_sums[tid];
        int scanned = warp_exclusive_scan(ws, tid);
        if (tid == 0) scanned = 0;
        warp_sums[tid] = scanned;
    }
    __syncthreads();
    
    int block_prefix = warp_sums[warp_id];
    int final_prefix = block_prefix + thread_prefix;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = global_base + i;
        if (idx < n) {
            output[idx] = local[i] + final_prefix;
        }
    }
    
    if (tid == BLOCK_SIZE - 1) {
        block_sums[bid] = final_prefix + thread_sum;
    }
}

__global__ void scan_block_sums(int* block_sums, int num_blocks) {
    __shared__ int shared[1024];
    
    int tid = threadIdx.x;
    int val = (tid < num_blocks) ? block_sums[tid] : 0;
    
    int lane = tid & 31;
    int warp_id = tid >> 5;
    
    int scanned = warp_inclusive_scan(val, lane);
    
    if (lane == 31) {
        shared[warp_id] = scanned;
    }
    __syncthreads();
    
    if (tid < 32) {
        int warp_sum = (tid < ((num_blocks + 31) >> 5)) ? shared[tid] : 0;
        int warp_prefix = warp_inclusive_scan(warp_sum, tid);
        shared[tid] = warp_prefix;
    }
    __syncthreads();
    
    int prefix = scanned;
    if (warp_id > 0) {
        prefix += shared[warp_id - 1];
    }
    
    int exclusive = prefix - val;
    
    if (tid < num_blocks) {
        block_sums[tid] = exclusive;
    }
}

__global__ void add_block_sums(int* __restrict__ output, const int* __restrict__ block_sums, int n) {
    int bid = blockIdx.x;
    int tid = threadIdx.x;
    int block_offset = block_sums[bid];
    
    int global_base = bid * ITEMS_PER_BLOCK + tid * ITEMS_PER_THREAD;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = global_base + i;
        if (idx < n) {
            output[idx] += block_offset;
        }
    }
}

static int* d_block_sums = nullptr;
static int block_sums_capacity = 0;

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;
    
    if (n <= ITEMS_PER_BLOCK) {
        for (int iter = 0; iter < iters; ++iter) {
            single_block_scan<<<1, BLOCK_SIZE>>>(input, output, n);
        }
        return;
    }
    
    int num_blocks = (n + ITEMS_PER_BLOCK - 1) / ITEMS_PER_BLOCK;
    
    if (num_blocks > block_sums_capacity) {
        if (d_block_sums) cudaFree(d_block_sums);
        cudaMalloc(&d_block_sums, num_blocks * sizeof(int));
        block_sums_capacity = num_blocks;
    }
    
    for (int iter = 0; iter < iters; ++iter) {
        block_scan_phase1<<<num_blocks, BLOCK_SIZE>>>(input, output, d_block_sums, n);
        scan_block_sums<<<1, 1024>>>(d_block_sums, num_blocks);
        add_block_sums<<<num_blocks, BLOCK_SIZE>>>(output, d_block_sums, n);
    }
}
