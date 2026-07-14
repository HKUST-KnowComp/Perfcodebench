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

__device__ __forceinline__ int warp_inclusive_scan(int val, int lane) {
    #pragma unroll
    for (int offset = 1; offset < 32; offset <<= 1) {
        int n = __shfl_up_sync(0xffffffff, val, offset);
        if (lane >= offset) val += n;
    }
    return val;
}

__global__ void scan_tiles_kernel(const int* __restrict__ input, int* __restrict__ output, 
                                   int* __restrict__ block_sums, int n) {
    __shared__ int warp_sums[8];
    
    const int tid = threadIdx.x;
    const int warp_id = tid >> 5;
    const int lane = tid & 31;
    const int block_offset = blockIdx.x * TILE_SIZE;
    
    int vals[ITEMS_PER_THREAD];
    int local_sum = 0;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = block_offset + i * BLOCK_SIZE + tid;
        vals[i] = (idx < n) ? input[idx] : 0;
        local_sum += vals[i];
    }
    
    int warp_scan = warp_inclusive_scan(local_sum, lane);
    int thread_offset = warp_scan - local_sum;
    
    if (lane == 31) {
        warp_sums[warp_id] = warp_scan;
    }
    __syncthreads();
    
    int warp_offset = 0;
    if (warp_id > 0) {
        if (tid < 8) {
            int ws = warp_sums[tid];
            #pragma unroll
            for (int offset = 1; offset < 8; offset <<= 1) {
                int n_val = __shfl_up_sync(0xff, ws, offset);
                if (tid >= offset) ws += n_val;
            }
            warp_sums[tid] = ws;
        }
        __syncthreads();
        warp_offset = warp_sums[warp_id - 1];
    } else {
        if (tid < 8) {
            int ws = warp_sums[tid];
            #pragma unroll
            for (int offset = 1; offset < 8; offset <<= 1) {
                int n_val = __shfl_up_sync(0xff, ws, offset);
                if (tid >= offset) ws += n_val;
            }
            warp_sums[tid] = ws;
        }
        __syncthreads();
    }
    
    int prefix = warp_offset + thread_offset;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = block_offset + i * BLOCK_SIZE + tid;
        if (idx < n) {
            output[idx] = prefix;
            prefix += vals[i];
        }
    }
    
    if (tid == BLOCK_SIZE - 1 && block_sums != nullptr) {
        block_sums[blockIdx.x] = warp_sums[7];
    }
}

__global__ void scan_block_sums_kernel(int* block_sums, int num_blocks) {
    __shared__ int warp_sums[8];
    
    const int tid = threadIdx.x;
    const int warp_id = tid >> 5;
    const int lane = tid & 31;
    
    int val = (tid < num_blocks) ? block_sums[tid] : 0;
    
    int warp_scan = warp_inclusive_scan(val, lane);
    int exclusive = warp_scan - val;
    
    if (lane == 31) {
        warp_sums[warp_id] = warp_scan;
    }
    __syncthreads();
    
    if (tid < 8) {
        int ws = warp_sums[tid];
        #pragma unroll
        for (int offset = 1; offset < 8; offset <<= 1) {
            int n_val = __shfl_up_sync(0xff, ws, offset);
            if (tid >= offset) ws += n_val;
        }
        warp_sums[tid] = ws;
    }
    __syncthreads();
    
    int warp_offset = (warp_id > 0) ? warp_sums[warp_id - 1] : 0;
    
    if (tid < num_blocks) {
        block_sums[tid] = warp_offset + exclusive;
    }
}

__global__ void add_block_offsets_kernel(int* output, const int* __restrict__ block_sums, int n) {
    const int block_offset = blockIdx.x * TILE_SIZE;
    const int tid = threadIdx.x;
    const int offset = block_sums[blockIdx.x];
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = block_offset + i * BLOCK_SIZE + tid;
        if (idx < n) {
            output[idx] += offset;
        }
    }
}

__global__ void single_block_scan_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    __shared__ int warp_sums[8];
    __shared__ int shared_data[TILE_SIZE];
    
    const int tid = threadIdx.x;
    const int warp_id = tid >> 5;
    const int lane = tid & 31;
    
    int vals[ITEMS_PER_THREAD];
    int local_sum = 0;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = i * BLOCK_SIZE + tid;
        vals[i] = (idx < n) ? input[idx] : 0;
        local_sum += vals[i];
    }
    
    int warp_scan = warp_inclusive_scan(local_sum, lane);
    int thread_offset = warp_scan - local_sum;
    
    if (lane == 31) {
        warp_sums[warp_id] = warp_scan;
    }
    __syncthreads();
    
    if (tid < 8) {
        int ws = warp_sums[tid];
        #pragma unroll
        for (int offset = 1; offset < 8; offset <<= 1) {
            int n_val = __shfl_up_sync(0xff, ws, offset);
            if (tid >= offset) ws += n_val;
        }
        warp_sums[tid] = ws;
    }
    __syncthreads();
    
    int warp_offset = (warp_id > 0) ? warp_sums[warp_id - 1] : 0;
    int prefix = warp_offset + thread_offset;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = i * BLOCK_SIZE + tid;
        if (idx < n) {
            output[idx] = prefix;
            prefix += vals[i];
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
        if (num_blocks == 1) {
            single_block_scan_kernel<<<1, BLOCK_SIZE>>>(input, output, n);
        } else {
            scan_tiles_kernel<<<num_blocks, BLOCK_SIZE>>>(input, output, block_sums, n);
            
            if (num_blocks <= 256) {
                scan_block_sums_kernel<<<1, 256>>>(block_sums, num_blocks);
            } else {
                int* temp_output;
                cudaMalloc(&temp_output, num_blocks * sizeof(int));
                
                int level2_blocks = (num_blocks + TILE_SIZE - 1) / TILE_SIZE;
                int* level2_sums = nullptr;
                if (level2_blocks > 1) {
                    cudaMalloc(&level2_sums, level2_blocks * sizeof(int));
                }
                
                scan_tiles_kernel<<<level2_blocks, BLOCK_SIZE>>>(block_sums, temp_output, level2_sums, num_blocks);
                
                if (level2_blocks > 1) {
                    scan_block_sums_kernel<<<1, 256>>>(level2_sums, level2_blocks);
                    add_block_offsets_kernel<<<level2_blocks, BLOCK_SIZE>>>(temp_output, level2_sums, num_blocks);
                    cudaFree(level2_sums);
                }
                
                cudaMemcpy(block_sums, temp_output, num_blocks * sizeof(int), cudaMemcpyDeviceToDevice);
                cudaFree(temp_output);
            }
            
            add_block_offsets_kernel<<<num_blocks, BLOCK_SIZE>>>(output, block_sums, n);
        }
    }
    
    if (block_sums) {
        cudaFree(block_sums);
    }
}