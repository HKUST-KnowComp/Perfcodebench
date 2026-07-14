#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_SIZE = 256;
constexpr int ITEMS_PER_THREAD = 4;
constexpr int TILE_SIZE = BLOCK_SIZE * ITEMS_PER_THREAD;

__device__ __forceinline__ int warp_exclusive_scan(int val, int lane) {
    #pragma unroll
    for (int offset = 1; offset < 32; offset <<= 1) {
        int n = __shfl_up_sync(0xffffffff, val, offset);
        if (lane >= offset) val += n;
    }
    int inclusive = val;
    int exclusive = __shfl_up_sync(0xffffffff, inclusive, 1);
    return (lane == 0) ? 0 : exclusive;
}

__global__ void block_scan_kernel(const int* __restrict__ input, 
                                   int* __restrict__ output,
                                   int* __restrict__ block_sums,
                                   int n) {
    __shared__ int shared[TILE_SIZE];
    __shared__ int warp_totals[8];
    
    int tid = threadIdx.x;
    int block_offset = blockIdx.x * TILE_SIZE;
    
    int local[ITEMS_PER_THREAD];
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = block_offset + tid + i * BLOCK_SIZE;
        local[i] = (idx < n) ? input[idx] : 0;
    }
    
    int thread_sum = 0;
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        thread_sum += local[i];
    }
    
    int lane = tid & 31;
    int warp_id = tid >> 5;
    
    int warp_exclusive = warp_exclusive_scan(thread_sum, lane);
    int warp_total = __shfl_sync(0xffffffff, warp_exclusive + thread_sum, 31);
    
    if (lane == 31) {
        warp_totals[warp_id] = warp_total;
    }
    __syncthreads();
    
    int block_prefix = 0;
    if (tid < 8) {
        int wt = warp_totals[tid];
        int wt_exclusive = warp_exclusive_scan(wt, tid);
        warp_totals[tid] = wt_exclusive;
        if (tid == 7) {
            if (block_sums != nullptr) {
                block_sums[blockIdx.x] = wt_exclusive + wt;
            }
        }
    }
    __syncthreads();
    
    block_prefix = warp_totals[warp_id] + warp_exclusive;
    
    int running = block_prefix;
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        shared[tid + i * BLOCK_SIZE] = running;
        running += local[i];
    }
    __syncthreads();
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = block_offset + tid + i * BLOCK_SIZE;
        if (idx < n) {
            output[idx] = shared[tid + i * BLOCK_SIZE];
        }
    }
}

__global__ void add_block_sums_kernel(int* __restrict__ output,
                                       const int* __restrict__ block_sums,
                                       int n) {
    int block_offset = blockIdx.x * TILE_SIZE;
    int block_sum = block_sums[blockIdx.x];
    int tid = threadIdx.x;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = block_offset + tid + i * BLOCK_SIZE;
        if (idx < n) {
            output[idx] += block_sum;
        }
    }
}

void scan_recursive(const int* input, int* output, int n, 
                    int* temp_buffer, int temp_offset) {
    if (n <= 0) return;
    
    if (n <= TILE_SIZE) {
        block_scan_kernel<<<1, BLOCK_SIZE>>>(input, output, nullptr, n);
        return;
    }
    
    int num_blocks = (n + TILE_SIZE - 1) / TILE_SIZE;
    int* block_sums = temp_buffer + temp_offset;
    int* scanned_block_sums = block_sums + num_blocks;
    
    block_scan_kernel<<<num_blocks, BLOCK_SIZE>>>(input, output, block_sums, n);
    
    int next_level_blocks = (num_blocks + TILE_SIZE - 1) / TILE_SIZE;
    int next_temp_offset = temp_offset + num_blocks * 2;
    
    scan_recursive(block_sums, scanned_block_sums, num_blocks, 
                   temp_buffer, next_temp_offset);
    
    add_block_sums_kernel<<<num_blocks, BLOCK_SIZE>>>(output, scanned_block_sums, n);
}

int compute_temp_size(int n) {
    if (n <= TILE_SIZE) return 0;
    int num_blocks = (n + TILE_SIZE - 1) / TILE_SIZE;
    return num_blocks * 2 + compute_temp_size(num_blocks);
}

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;
    
    int temp_size = compute_temp_size(n);
    int* temp_buffer = nullptr;
    if (temp_size > 0) {
        cudaMalloc(&temp_buffer, temp_size * sizeof(int));
    }
    
    for (int iter = 0; iter < iters; ++iter) {
        scan_recursive(input, output, n, temp_buffer, 0);
    }
    
    if (temp_buffer != nullptr) {
        cudaFree(temp_buffer);
    }
}