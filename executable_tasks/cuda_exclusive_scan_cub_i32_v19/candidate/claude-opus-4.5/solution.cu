#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_SIZE = 256;
constexpr int ITEMS_PER_THREAD = 4;
constexpr int ITEMS_PER_BLOCK = BLOCK_SIZE * ITEMS_PER_THREAD;

__device__ __forceinline__ int warp_scan_exclusive(int val, int lane) {
    #pragma unroll
    for (int offset = 1; offset < 32; offset <<= 1) {
        int n = __shfl_up_sync(0xffffffff, val, offset);
        if (lane >= offset) val += n;
    }
    return val - __shfl_sync(0xffffffff, val, lane) + __shfl_up_sync(0xffffffff, val, 1) * (lane > 0);
}

__device__ __forceinline__ int warp_scan_inclusive(int val, int lane) {
    #pragma unroll
    for (int offset = 1; offset < 32; offset <<= 1) {
        int n = __shfl_up_sync(0xffffffff, val, offset);
        if (lane >= offset) val += n;
    }
    return val;
}

__global__ void scan_blocks_kernel(const int* __restrict__ input, int* __restrict__ output, 
                                    int* __restrict__ block_sums, int n) {
    __shared__ int warp_totals[8];
    
    const int tid = threadIdx.x;
    const int warp_id = tid >> 5;
    const int lane = tid & 31;
    const int block_offset = blockIdx.x * ITEMS_PER_BLOCK;
    
    int vals[ITEMS_PER_THREAD];
    int thread_sum = 0;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = block_offset + tid + i * BLOCK_SIZE;
        vals[i] = (idx < n) ? input[idx] : 0;
        thread_sum += vals[i];
    }
    
    int warp_inclusive = warp_scan_inclusive(thread_sum, lane);
    int warp_exclusive = warp_inclusive - thread_sum;
    
    if (lane == 31) {
        warp_totals[warp_id] = warp_inclusive;
    }
    __syncthreads();
    
    int block_prefix = 0;
    if (warp_id == 0 && lane < 8) {
        int wt = warp_totals[lane];
        int wt_scan = warp_scan_inclusive(wt, lane);
        warp_totals[lane] = wt_scan - wt;
    }
    __syncthreads();
    
    block_prefix = warp_totals[warp_id] + warp_exclusive;
    
    int running = block_prefix;
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = block_offset + tid + i * BLOCK_SIZE;
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
    __shared__ int block_prefix;
    
    if (threadIdx.x == 0) {
        block_prefix = block_sums[blockIdx.x];
    }
    __syncthreads();
    
    int bp = block_prefix;
    int block_offset = blockIdx.x * ITEMS_PER_BLOCK;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = block_offset + threadIdx.x + i * BLOCK_SIZE;
        if (idx < n) {
            output[idx] += bp;
        }
    }
}

__global__ void scan_block_sums_kernel(int* block_sums, int num_blocks) {
    __shared__ int warp_totals[8];
    
    const int tid = threadIdx.x;
    const int warp_id = tid >> 5;
    const int lane = tid & 31;
    
    int val = (tid < num_blocks) ? block_sums[tid] : 0;
    
    int warp_inclusive = warp_scan_inclusive(val, lane);
    int warp_exclusive = warp_inclusive - val;
    
    if (lane == 31) {
        warp_totals[warp_id] = warp_inclusive;
    }
    __syncthreads();
    
    if (warp_id == 0 && lane < 8) {
        int wt = warp_totals[lane];
        int wt_scan = warp_scan_inclusive(wt, lane);
        warp_totals[lane] = wt_scan - wt;
    }
    __syncthreads();
    
    int result = warp_totals[warp_id] + warp_exclusive;
    
    if (tid < num_blocks) {
        block_sums[tid] = result;
    }
}

__global__ void single_block_scan_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    __shared__ int warp_totals[8];
    __shared__ int sdata[ITEMS_PER_BLOCK];
    
    const int tid = threadIdx.x;
    const int warp_id = tid >> 5;
    const int lane = tid & 31;
    
    int vals[ITEMS_PER_THREAD];
    int thread_sum = 0;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = tid + i * BLOCK_SIZE;
        vals[i] = (idx < n) ? input[idx] : 0;
        thread_sum += vals[i];
    }
    
    int warp_inclusive = warp_scan_inclusive(thread_sum, lane);
    int warp_exclusive = warp_inclusive - thread_sum;
    
    if (lane == 31) {
        warp_totals[warp_id] = warp_inclusive;
    }
    __syncthreads();
    
    if (warp_id == 0 && lane < 8) {
        int wt = warp_totals[lane];
        int wt_scan = warp_scan_inclusive(wt, lane);
        warp_totals[lane] = wt_scan - wt;
    }
    __syncthreads();
    
    int block_prefix = warp_totals[warp_id] + warp_exclusive;
    
    int running = block_prefix;
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        int idx = tid + i * BLOCK_SIZE;
        if (idx < n) {
            output[idx] = running;
        }
        running += vals[i];
    }
}

static int* d_block_sums = nullptr;
static int block_sums_capacity = 0;

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;
    
    if (n <= ITEMS_PER_BLOCK) {
        for (int iter = 0; iter < iters; ++iter) {
            single_block_scan_kernel<<<1, BLOCK_SIZE>>>(input, output, n);
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
        scan_blocks_kernel<<<num_blocks, BLOCK_SIZE>>>(input, output, d_block_sums, n);
        
        if (num_blocks <= BLOCK_SIZE) {
            scan_block_sums_kernel<<<1, BLOCK_SIZE>>>(d_block_sums, num_blocks);
        } else {
            int running = 0;
            int* h_sums = new int[num_blocks];
            cudaMemcpy(h_sums, d_block_sums, num_blocks * sizeof(int), cudaMemcpyDeviceToHost);
            for (int i = 0; i < num_blocks; i++) {
                int tmp = h_sums[i];
                h_sums[i] = running;
                running += tmp;
            }
            cudaMemcpy(d_block_sums, h_sums, num_blocks * sizeof(int), cudaMemcpyHostToDevice);
            delete[] h_sums;
        }
        
        add_block_sums_kernel<<<num_blocks, BLOCK_SIZE>>>(output, d_block_sums, n);
    }
}