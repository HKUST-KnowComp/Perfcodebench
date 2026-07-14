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
    
    int vals[ITEMS_PER_THREAD];
    int local_sum = 0;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = tid + i * BLOCK_SIZE;
        vals[i] = (idx < n) ? input[idx] : 0;
        local_sum += vals[i];
    }
    
    int warp_scan = warp_inclusive_scan(local_sum, lane);
    
    if (lane == 31) {
        warp_sums[warp_id] = warp_scan;
    }
    __syncthreads();
    
    if (warp_id == 0 && lane < 8) {
        int ws = warp_sums[lane];
        int scanned = warp_exclusive_scan(ws, lane);
        warp_sums[lane] = (lane == 0) ? 0 : scanned;
    }
    __syncthreads();
    
    int thread_offset = warp_sums[warp_id] + warp_scan - local_sum;
    
    int running = thread_offset;
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = tid + i * BLOCK_SIZE;
        if (idx < n) {
            output[idx] = running;
            running += vals[i];
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
    int base = bid * ITEMS_PER_BLOCK;
    
    int vals[ITEMS_PER_THREAD];
    int local_sum = 0;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = base + tid + i * BLOCK_SIZE;
        vals[i] = (idx < n) ? input[idx] : 0;
        local_sum += vals[i];
    }
    
    int warp_scan = warp_inclusive_scan(local_sum, lane);
    
    if (lane == 31) {
        warp_sums[warp_id] = warp_scan;
    }
    __syncthreads();
    
    if (warp_id == 0 && lane < 8) {
        int ws = warp_sums[lane];
        int scanned = warp_exclusive_scan(ws, lane);
        warp_sums[lane] = (lane == 0) ? 0 : scanned;
    }
    __syncthreads();
    
    int thread_offset = warp_sums[warp_id] + warp_scan - local_sum;
    
    int running = thread_offset;
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = base + tid + i * BLOCK_SIZE;
        if (idx < n) {
            output[idx] = running;
            running += vals[i];
        }
    }
    
    if (tid == BLOCK_SIZE - 1) {
        block_sums[bid] = running;
    }
}

__global__ void scan_block_sums(int* block_sums, int num_blocks) {
    __shared__ int warp_sums[8];
    
    int tid = threadIdx.x;
    int lane = tid & 31;
    int warp_id = tid >> 5;
    
    int vals[ITEMS_PER_THREAD];
    int local_sum = 0;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = tid + i * BLOCK_SIZE;
        vals[i] = (idx < num_blocks) ? block_sums[idx] : 0;
        local_sum += vals[i];
    }
    
    int warp_scan = warp_inclusive_scan(local_sum, lane);
    
    if (lane == 31) {
        warp_sums[warp_id] = warp_scan;
    }
    __syncthreads();
    
    if (warp_id == 0 && lane < 8) {
        int ws = warp_sums[lane];
        int scanned = warp_exclusive_scan(ws, lane);
        warp_sums[lane] = (lane == 0) ? 0 : scanned;
    }
    __syncthreads();
    
    int thread_offset = warp_sums[warp_id] + warp_scan - local_sum;
    
    int running = thread_offset;
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = tid + i * BLOCK_SIZE;
        if (idx < num_blocks) {
            block_sums[idx] = running;
            running += vals[i];
        }
    }
}

__global__ void add_block_offsets(int* output, const int* __restrict__ block_sums, int n) {
    int bid = blockIdx.x;
    int tid = threadIdx.x;
    int base = bid * ITEMS_PER_BLOCK;
    int offset = block_sums[bid];
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = base + tid + i * BLOCK_SIZE;
        if (idx < n) {
            output[idx] += offset;
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
        scan_block_sums<<<1, BLOCK_SIZE>>>(d_block_sums, num_blocks);
        add_block_offsets<<<num_blocks, BLOCK_SIZE>>>(output, d_block_sums, n);
    }
}