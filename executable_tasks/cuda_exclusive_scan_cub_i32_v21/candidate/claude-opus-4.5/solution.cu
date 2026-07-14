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

__global__ void block_scan_kernel(const int* __restrict__ input, int* __restrict__ output,
                                   int* __restrict__ block_sums, int n) {
    __shared__ int warp_totals[8];
    
    const int tid = threadIdx.x;
    const int bid = blockIdx.x;
    const int lane = tid & 31;
    const int warp_id = tid >> 5;
    const int base = bid * TILE_SIZE;
    
    int vals[ITEMS_PER_THREAD];
    int thread_sum = 0;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = base + i * BLOCK_SIZE + tid;
        vals[i] = (idx < n) ? input[idx] : 0;
        thread_sum += vals[i];
    }
    
    int warp_exc = warp_exclusive_scan(thread_sum, lane);
    int warp_total = __shfl_sync(0xffffffff, warp_exc + thread_sum, 31);
    
    if (lane == 31) {
        warp_totals[warp_id] = warp_total;
    }
    __syncthreads();
    
    int block_offset = 0;
    if (warp_id == 0 && lane < 8) {
        int wt = warp_totals[lane];
        int wt_exc = warp_exclusive_scan(wt, lane);
        warp_totals[lane] = wt_exc;
        if (lane == 7) {
            block_sums[bid] = wt_exc + wt;
        }
    }
    __syncthreads();
    
    block_offset = warp_totals[warp_id];
    int thread_offset = block_offset + warp_exc;
    
    int running = thread_offset;
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = base + i * BLOCK_SIZE + tid;
        if (idx < n) {
            output[idx] = running;
        }
        running += vals[i];
    }
}

__global__ void propagate_kernel(int* __restrict__ output, const int* __restrict__ block_sums,
                                  const int* __restrict__ block_offsets, int n) {
    const int tid = threadIdx.x;
    const int bid = blockIdx.x;
    const int base = bid * TILE_SIZE;
    
    int offset = (bid > 0) ? block_offsets[bid] : 0;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = base + i * BLOCK_SIZE + tid;
        if (idx < n && bid > 0) {
            output[idx] += offset;
        }
    }
}

__global__ void scan_block_sums_kernel(const int* __restrict__ block_sums,
                                        int* __restrict__ block_offsets, int num_blocks) {
    __shared__ int warp_totals[8];
    
    const int tid = threadIdx.x;
    const int lane = tid & 31;
    const int warp_id = tid >> 5;
    
    int val = (tid < num_blocks) ? block_sums[tid] : 0;
    
    int warp_exc = warp_exclusive_scan(val, lane);
    int warp_total = __shfl_sync(0xffffffff, warp_exc + val, 31);
    
    if (lane == 31) {
        warp_totals[warp_id] = warp_total;
    }
    __syncthreads();
    
    int block_offset = 0;
    if (warp_id == 0 && lane < 8) {
        int wt = warp_totals[lane];
        int wt_exc = warp_exclusive_scan(wt, lane);
        warp_totals[lane] = wt_exc;
    }
    __syncthreads();
    
    block_offset = warp_totals[warp_id];
    int final_exc = block_offset + warp_exc;
    
    if (tid < num_blocks) {
        block_offsets[tid] = final_exc;
    }
}

__global__ void single_block_scan_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    __shared__ int warp_totals[8];
    
    const int tid = threadIdx.x;
    const int lane = tid & 31;
    const int warp_id = tid >> 5;
    
    int vals[ITEMS_PER_THREAD];
    int thread_sum = 0;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = i * BLOCK_SIZE + tid;
        vals[i] = (idx < n) ? input[idx] : 0;
        thread_sum += vals[i];
    }
    
    int warp_exc = warp_exclusive_scan(thread_sum, lane);
    int warp_total = __shfl_sync(0xffffffff, warp_exc + thread_sum, 31);
    
    if (lane == 31) {
        warp_totals[warp_id] = warp_total;
    }
    __syncthreads();
    
    if (warp_id == 0 && lane < 8) {
        int wt = warp_totals[lane];
        int wt_exc = warp_exclusive_scan(wt, lane);
        warp_totals[lane] = wt_exc;
    }
    __syncthreads();
    
    int block_offset = warp_totals[warp_id];
    int thread_offset = block_offset + warp_exc;
    
    int running = thread_offset;
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = i * BLOCK_SIZE + tid;
        if (idx < n) {
            output[idx] = running;
        }
        running += vals[i];
    }
}

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;
    
    if (n <= TILE_SIZE) {
        for (int iter = 0; iter < iters; ++iter) {
            single_block_scan_kernel<<<1, BLOCK_SIZE>>>(input, output, n);
        }
        return;
    }
    
    int num_blocks = (n + TILE_SIZE - 1) / TILE_SIZE;
    
    int* block_sums = nullptr;
    int* block_offsets = nullptr;
    cudaMalloc(&block_sums, num_blocks * sizeof(int));
    cudaMalloc(&block_offsets, num_blocks * sizeof(int));
    
    for (int iter = 0; iter < iters; ++iter) {
        block_scan_kernel<<<num_blocks, BLOCK_SIZE>>>(input, output, block_sums, n);
        scan_block_sums_kernel<<<1, BLOCK_SIZE>>>(block_sums, block_offsets, num_blocks);
        propagate_kernel<<<num_blocks, BLOCK_SIZE>>>(output, block_sums, block_offsets, n);
    }
    
    cudaFree(block_sums);
    cudaFree(block_offsets);
}
