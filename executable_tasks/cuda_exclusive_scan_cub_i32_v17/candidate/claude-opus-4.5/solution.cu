#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_SIZE = 256;
constexpr int ITEMS_PER_THREAD = 8;
constexpr int TILE_SIZE = BLOCK_SIZE * ITEMS_PER_THREAD;

__device__ __forceinline__ int warp_exclusive_scan(int val, int lane) {
    #pragma unroll
    for (int offset = 1; offset < 32; offset <<= 1) {
        int n = __shfl_up_sync(0xffffffff, val, offset);
        if (lane >= offset) val += n;
    }
    return __shfl_up_sync(0xffffffff, val, 1);
}

__global__ void scan_tiles_kernel(const int* __restrict__ input, int* __restrict__ output,
                                   int* __restrict__ block_sums, int n) {
    __shared__ int warp_totals[BLOCK_SIZE / 32];
    
    const int tid = threadIdx.x;
    const int warp_id = tid >> 5;
    const int lane = tid & 31;
    const int block_offset = blockIdx.x * TILE_SIZE;
    
    int vals[ITEMS_PER_THREAD];
    int local_sum = 0;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; ++i) {
        int idx = block_offset + i * BLOCK_SIZE + tid;
        vals[i] = (idx < n) ? input[idx] : 0;
        local_sum += vals[i];
    }
    
    int warp_scan = warp_exclusive_scan(local_sum, lane);
    int thread_prefix = (lane == 0) ? 0 : warp_scan;
    
    int warp_total = __shfl_sync(0xffffffff, local_sum, 31);
    for (int i = 0; i < lane; ++i) {
        warp_total = __shfl_sync(0xffffffff, local_sum, 31);
    }
    warp_total = local_sum;
    #pragma unroll
    for (int offset = 1; offset < 32; offset <<= 1) {
        int n_val = __shfl_up_sync(0xffffffff, warp_total, offset);
        if (lane >= offset) warp_total += n_val;
    }
    
    if (lane == 31) {
        warp_totals[warp_id] = warp_total;
    }
    __syncthreads();
    
    int warp_prefix = 0;
    if (tid < (BLOCK_SIZE / 32)) {
        int wt = warp_totals[tid];
        #pragma unroll
        for (int offset = 1; offset < (BLOCK_SIZE / 32); offset <<= 1) {
            int n_val = __shfl_up_sync(0xffffffff, wt, offset);
            if (tid >= offset) wt += n_val;
        }
        warp_totals[tid] = (tid == 0) ? 0 : __shfl_up_sync(0xffffffff, wt, 1);
    }
    __syncthreads();
    
    warp_prefix = warp_totals[warp_id];
    thread_prefix += warp_prefix;
    
    int running = thread_prefix;
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; ++i) {
        int idx = block_offset + i * BLOCK_SIZE + tid;
        if (idx < n) {
            output[idx] = running;
        }
        running += vals[i];
    }
    
    if (tid == BLOCK_SIZE - 1 && block_sums != nullptr) {
        block_sums[blockIdx.x] = running;
    }
}

__global__ void add_block_sums_kernel(int* __restrict__ output, const int* __restrict__ block_sums, int n) {
    const int block_offset = blockIdx.x * TILE_SIZE;
    const int tid = threadIdx.x;
    
    int block_prefix = 0;
    for (int b = 0; b < blockIdx.x; ++b) {
        block_prefix += block_sums[b];
    }
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; ++i) {
        int idx = block_offset + i * BLOCK_SIZE + tid;
        if (idx < n) {
            output[idx] += block_prefix;
        }
    }
}

__global__ void scan_block_sums_kernel(int* __restrict__ block_sums, int num_blocks) {
    __shared__ int temp[1024];
    
    int tid = threadIdx.x;
    
    int val = (tid < num_blocks) ? block_sums[tid] : 0;
    temp[tid] = val;
    __syncthreads();
    
    for (int offset = 1; offset < num_blocks; offset <<= 1) {
        int t = (tid >= offset) ? temp[tid - offset] : 0;
        __syncthreads();
        temp[tid] += t;
        __syncthreads();
    }
    
    if (tid < num_blocks) {
        block_sums[tid] = (tid == 0) ? 0 : temp[tid - 1];
    }
}

__global__ void add_scanned_block_sums_kernel(int* __restrict__ output, const int* __restrict__ block_sums, int n) {
    const int block_offset = blockIdx.x * TILE_SIZE;
    const int tid = threadIdx.x;
    
    int block_prefix = block_sums[blockIdx.x];
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; ++i) {
        int idx = block_offset + i * BLOCK_SIZE + tid;
        if (idx < n) {
            output[idx] += block_prefix;
        }
    }
}

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;
    
    int num_blocks = (n + TILE_SIZE - 1) / TILE_SIZE;
    
    int* block_sums = nullptr;
    if (num_blocks > 1) {
        cudaMalloc(&block_sums, num_blocks * sizeof(int));
    }
    
    for (int iter = 0; iter < iters; ++iter) {
        scan_tiles_kernel<<<num_blocks, BLOCK_SIZE>>>(input, output, block_sums, n);
        
        if (num_blocks > 1) {
            scan_block_sums_kernel<<<1, 1024>>>(block_sums, num_blocks);
            add_scanned_block_sums_kernel<<<num_blocks, BLOCK_SIZE>>>(output, block_sums, n);
        }
    }
    
    if (block_sums != nullptr) {
        cudaFree(block_sums);
    }
}