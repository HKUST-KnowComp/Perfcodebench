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
    
    int thread_total = thread_sum;
    int warp_scan = warp_inclusive_scan(thread_total, lane);
    int thread_offset = warp_scan - thread_total;
    
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
    
    int block_offset = warp_sums[warp_id] + thread_offset;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = base + i;
        if (idx < n) {
            output[idx] = local[i] + block_offset;
        }
    }
}

__global__ void block_scan_phase1(const int* __restrict__ input, int* __restrict__ output, 
                                   int* __restrict__ block_sums, int n) {
    __shared__ int shared[BLOCK_SIZE];
    __shared__ int warp_sums[8];
    
    int tid = threadIdx.x;
    int bid = blockIdx.x;
    int lane = tid & 31;
    int warp_id = tid >> 5;
    
    int local[ITEMS_PER_THREAD];
    int base = bid * ITEMS_PER_BLOCK + tid * ITEMS_PER_THREAD;
    
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
    
    int thread_total = thread_sum;
    int warp_scan = warp_inclusive_scan(thread_total, lane);
    int thread_offset = warp_scan - thread_total;
    
    if (lane == 31) {
        warp_sums[warp_id] = warp_scan;
    }
    __syncthreads();
    
    if (warp_id == 0 && lane < 8) {
        int ws = warp_sums[lane];
        int scanned = warp_inclusive_scan(ws, lane);
        warp_sums[lane] = scanned - ws;
    }
    __syncthreads();
    
    int block_offset = warp_sums[warp_id] + thread_offset;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = base + i;
        if (idx < n) {
            output[idx] = local[i] + block_offset;
        }
    }
    
    if (tid == BLOCK_SIZE - 1) {
        block_sums[bid] = block_offset + thread_total;
    }
}

__global__ void scan_block_sums(int* block_sums, int num_blocks) {
    __shared__ int shared[1024];
    
    int tid = threadIdx.x;
    int lane = tid & 31;
    int warp_id = tid >> 5;
    
    int val = (tid < num_blocks) ? block_sums[tid] : 0;
    
    int warp_scan = warp_inclusive_scan(val, lane);
    
    if (lane == 31) {
        shared[warp_id] = warp_scan;
    }
    __syncthreads();
    
    if (warp_id == 0) {
        int ws = (lane < 32) ? shared[lane] : 0;
        int scanned = warp_inclusive_scan(ws, lane);
        shared[lane] = scanned;
    }
    __syncthreads();
    
    int warp_offset = (warp_id > 0) ? shared[warp_id - 1] : 0;
    int result = warp_scan - val + warp_offset;
    
    if (tid < num_blocks) {
        block_sums[tid] = result;
    }
}

__global__ void add_block_offsets(int* __restrict__ output, const int* __restrict__ block_sums, int n) {
    int bid = blockIdx.x;
    int tid = threadIdx.x;
    int base = bid * ITEMS_PER_BLOCK + tid * ITEMS_PER_THREAD;
    
    int offset = block_sums[bid];
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = base + i;
        if (idx < n) {
            output[idx] += offset;
        }
    }
}

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;
    
    int num_blocks = (n + ITEMS_PER_BLOCK - 1) / ITEMS_PER_BLOCK;
    
    int* d_block_sums = nullptr;
    if (num_blocks > 1) {
        cudaMalloc(&d_block_sums, num_blocks * sizeof(int));
    }
    
    for (int iter = 0; iter < iters; ++iter) {
        if (num_blocks == 1) {
            single_block_scan<<<1, BLOCK_SIZE>>>(input, output, n);
        } else {
            block_scan_phase1<<<num_blocks, BLOCK_SIZE>>>(input, output, d_block_sums, n);
            scan_block_sums<<<1, 1024>>>(d_block_sums, num_blocks);
            add_block_offsets<<<num_blocks, BLOCK_SIZE>>>(output, d_block_sums, n);
        }
    }
    
    if (d_block_sums) {
        cudaFree(d_block_sums);
    }
}