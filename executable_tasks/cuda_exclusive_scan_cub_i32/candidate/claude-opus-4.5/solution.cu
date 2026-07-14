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
    int inclusive = val;
    int exclusive = __shfl_up_sync(0xffffffff, inclusive, 1);
    return (lane == 0) ? 0 : exclusive;
}

__device__ __forceinline__ int warp_inclusive_scan(int val, int lane) {
    #pragma unroll
    for (int offset = 1; offset < 32; offset <<= 1) {
        int n = __shfl_up_sync(0xffffffff, val, offset);
        if (lane >= offset) val += n;
    }
    return val;
}

__global__ void block_scan_kernel(const int* __restrict__ input, 
                                   int* __restrict__ output,
                                   int* __restrict__ block_sums,
                                   int n) {
    __shared__ int warp_sums[BLOCK_SIZE / 32];
    __shared__ int warp_offsets[BLOCK_SIZE / 32];
    
    const int tid = threadIdx.x;
    const int warp_id = tid / 32;
    const int lane = tid % 32;
    const int block_offset = blockIdx.x * ITEMS_PER_BLOCK;
    
    int local_data[ITEMS_PER_THREAD];
    int local_sum = 0;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = block_offset + i * BLOCK_SIZE + tid;
        local_data[i] = (idx < n) ? input[idx] : 0;
        local_sum += local_data[i];
    }
    
    int thread_exclusive = warp_exclusive_scan(local_sum, lane);
    
    if (lane == 31) {
        warp_sums[warp_id] = thread_exclusive + local_sum;
    }
    __syncthreads();
    
    if (warp_id == 0 && lane < (BLOCK_SIZE / 32)) {
        int ws = warp_sums[lane];
        int warp_exc = warp_exclusive_scan(ws, lane);
        warp_offsets[lane] = warp_exc;
        if (lane == (BLOCK_SIZE / 32) - 1) {
            if (block_sums != nullptr) {
                block_sums[blockIdx.x] = warp_exc + ws;
            }
        }
    }
    __syncthreads();
    
    int base = warp_offsets[warp_id] + thread_exclusive;
    
    int running = base;
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = block_offset + i * BLOCK_SIZE + tid;
        if (idx < n) {
            output[idx] = running;
        }
        running += local_data[i];
    }
}

__global__ void add_block_offsets_kernel(int* __restrict__ output,
                                          const int* __restrict__ block_offsets,
                                          int n) {
    const int tid = threadIdx.x;
    const int block_offset_val = block_offsets[blockIdx.x];
    const int block_start = blockIdx.x * ITEMS_PER_BLOCK;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = block_start + i * BLOCK_SIZE + tid;
        if (idx < n) {
            output[idx] += block_offset_val;
        }
    }
}

__global__ void single_block_scan_kernel(int* __restrict__ data, int n) {
    __shared__ int warp_sums[32];
    __shared__ int warp_offsets[32];
    
    const int tid = threadIdx.x;
    const int warp_id = tid / 32;
    const int lane = tid % 32;
    const int num_warps = blockDim.x / 32;
    
    int val = (tid < n) ? data[tid] : 0;
    
    int exc = warp_exclusive_scan(val, lane);
    
    if (lane == 31) {
        warp_sums[warp_id] = exc + val;
    }
    __syncthreads();
    
    if (warp_id == 0 && lane < num_warps) {
        int ws = warp_sums[lane];
        warp_offsets[lane] = warp_exclusive_scan(ws, lane);
    }
    __syncthreads();
    
    if (tid < n) {
        data[tid] = exc + warp_offsets[warp_id];
    }
}

void scan_block_sums(int* block_sums, int num_blocks, int* temp_buffer) {
    if (num_blocks <= 1024) {
        int threads = ((num_blocks + 31) / 32) * 32;
        if (threads < 32) threads = 32;
        if (threads > 1024) threads = 1024;
        single_block_scan_kernel<<<1, threads>>>(block_sums, num_blocks);
    } else {
        int level2_blocks = (num_blocks + ITEMS_PER_BLOCK - 1) / ITEMS_PER_BLOCK;
        int* level2_sums = temp_buffer;
        
        block_scan_kernel<<<level2_blocks, BLOCK_SIZE>>>(block_sums, block_sums, level2_sums, num_blocks);
        
        scan_block_sums(level2_sums, level2_blocks, level2_sums + level2_blocks);
        
        add_block_offsets_kernel<<<level2_blocks, BLOCK_SIZE>>>(block_sums, level2_sums, num_blocks);
    }
}

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;
    
    int num_blocks = (n + ITEMS_PER_BLOCK - 1) / ITEMS_PER_BLOCK;
    
    int* block_sums = nullptr;
    int* temp_buffer = nullptr;
    
    if (num_blocks > 1) {
        int temp_size = num_blocks;
        int level_blocks = num_blocks;
        while (level_blocks > 1024) {
            int next_level = (level_blocks + ITEMS_PER_BLOCK - 1) / ITEMS_PER_BLOCK;
            temp_size += next_level;
            level_blocks = next_level;
        }
        cudaMalloc(&block_sums, temp_size * sizeof(int));
        temp_buffer = block_sums + num_blocks;
    }
    
    for (int iter = 0; iter < iters; ++iter) {
        if (num_blocks == 1) {
            block_scan_kernel<<<1, BLOCK_SIZE>>>(input, output, nullptr, n);
        } else {
            block_scan_kernel<<<num_blocks, BLOCK_SIZE>>>(input, output, block_sums, n);
            scan_block_sums(block_sums, num_blocks, temp_buffer);
            add_block_offsets_kernel<<<num_blocks, BLOCK_SIZE>>>(output, block_sums, n);
        }
    }
    
    if (block_sums) {
        cudaFree(block_sums);
    }
}