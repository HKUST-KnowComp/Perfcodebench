#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int RADIX_BITS = 4;
constexpr int RADIX_SIZE = 1 << RADIX_BITS;
constexpr int RADIX_MASK = RADIX_SIZE - 1;
constexpr int BLOCK_SIZE = 256;
constexpr int ITEMS_PER_THREAD = 4;

__device__ __forceinline__ int extract_digit(int value, int shift) {
    return (value >> shift) & RADIX_MASK;
}

__global__ void radix_sort_kernel(int* data, int* temp, int n, int shift) {
    __shared__ int local_hist[RADIX_SIZE];
    __shared__ int global_offset[RADIX_SIZE];
    __shared__ int block_data[BLOCK_SIZE * ITEMS_PER_THREAD];
    
    int tid = threadIdx.x;
    int lane = tid & 31;
    int warp = tid >> 5;
    
    // Initialize histogram
    if (tid < RADIX_SIZE) {
        local_hist[tid] = 0;
    }
    __syncthreads();
    
    // Load data into shared memory and compute local histogram
    int local_counts[RADIX_SIZE] = {0};
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; ++i) {
        int idx = tid + i * BLOCK_SIZE;
        int value = 0;
        if (idx < n) {
            value = data[idx];
            block_data[tid + i * BLOCK_SIZE] = value;
            int digit = extract_digit(value, shift);
            local_counts[digit]++;
        }
    }
    
    // Reduce counts within warp using shuffle
    #pragma unroll
    for (int d = 0; d < RADIX_SIZE; ++d) {
        int count = local_counts[d];
        #pragma unroll
        for (int offset = 16; offset > 0; offset >>= 1) {
            count += __shfl_down_sync(0xffffffff, count, offset);
        }
        if (lane == 0) {
            atomicAdd(&local_hist[d], count);
        }
    }
    __syncthreads();
    
    // Compute prefix sum (exclusive scan)
    if (tid < RADIX_SIZE) {
        int sum = 0;
        for (int i = 0; i < tid; ++i) {
            sum += local_hist[i];
        }
        global_offset[tid] = sum;
    }
    __syncthreads();
    
    // Scatter to temp buffer
    __shared__ int scatter_offset[RADIX_SIZE];
    if (tid < RADIX_SIZE) {
        scatter_offset[tid] = global_offset[tid];
    }
    __syncthreads();
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; ++i) {
        int idx = tid + i * BLOCK_SIZE;
        if (idx < n) {
            int value = block_data[tid + i * BLOCK_SIZE];
            int digit = extract_digit(value, shift);
            int pos = atomicAdd(&scatter_offset[digit], 1);
            temp[pos] = value;
        }
    }
}

__global__ void copy_kernel(const int* src, int* dst, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int i = idx; i < n; i += stride) {
        dst[i] = src[i];
    }
}

}  // namespace

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    int* temp;
    cudaMalloc(&temp, n * sizeof(int));
    
    for (int iter = 0; iter < iters; ++iter) {
        // Copy input to output for first pass
        copy_kernel<<<(n + 255) / 256, 256>>>(input, output, n);
        
        // Radix sort passes (8 passes for 32-bit integers with 4-bit radix)
        for (int shift = 0; shift < 32; shift += RADIX_BITS) {
            if ((shift / RADIX_BITS) % 2 == 0) {
                radix_sort_kernel<<<1, BLOCK_SIZE>>>(output, temp, n, shift);
            } else {
                radix_sort_kernel<<<1, BLOCK_SIZE>>>(temp, output, n, shift);
            }
        }
    }
    
    cudaFree(temp);
}