#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int RADIX_BITS = 4;
constexpr int RADIX = 1 << RADIX_BITS;
constexpr int RADIX_MASK = RADIX - 1;
constexpr int BLOCK_SIZE = 256;
constexpr int ITEMS_PER_THREAD = 4;

__device__ __forceinline__ unsigned int extract_digit(int value, int shift) {
    unsigned int uval = value ^ 0x80000000u;
    return (uval >> shift) & RADIX_MASK;
}

__global__ void radix_sort_kernel(int* data, int* temp, int n, int shift) {
    __shared__ int local_hist[RADIX];
    __shared__ int global_offsets[RADIX];
    __shared__ int block_data[BLOCK_SIZE * ITEMS_PER_THREAD];
    
    int tid = threadIdx.x;
    int block_offset = blockIdx.x * BLOCK_SIZE * ITEMS_PER_THREAD;
    
    if (tid < RADIX) {
        local_hist[tid] = 0;
    }
    __syncthreads();
    
    int items[ITEMS_PER_THREAD];
    int digits[ITEMS_PER_THREAD];
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; ++i) {
        int idx = block_offset + tid + i * BLOCK_SIZE;
        if (idx < n) {
            items[i] = data[idx];
            digits[i] = extract_digit(items[i], shift);
            atomicAdd(&local_hist[digits[i]], 1);
        } else {
            items[i] = 0x7FFFFFFF;
            digits[i] = RADIX - 1;
        }
    }
    __syncthreads();
    
    if (tid < RADIX) {
        int sum = 0;
        for (int b = 0; b < gridDim.x; ++b) {
            int count = (b == blockIdx.x) ? local_hist[tid] : 0;
            if (b < blockIdx.x) {
                for (int d = 0; d < RADIX; ++d) {
                    if (d == tid) sum += local_hist[d];
                }
            }
        }
        global_offsets[tid] = sum;
    }
    __syncthreads();
    
    int prefix_sum = 0;
    if (tid < RADIX) {
        for (int d = 0; d < tid; ++d) {
            prefix_sum += local_hist[d];
        }
    }
    __syncthreads();
    
    __shared__ int digit_offsets[RADIX];
    if (tid < RADIX) {
        digit_offsets[tid] = global_offsets[tid] + prefix_sum;
    }
    __syncthreads();
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; ++i) {
        int idx = block_offset + tid + i * BLOCK_SIZE;
        if (idx < n) {
            int pos = atomicAdd(&digit_offsets[digits[i]], 1);
            temp[pos] = items[i];
        }
    }
}

__global__ void histogram_kernel(const int* data, int* hist, int n, int shift) {
    __shared__ int local_hist[RADIX];
    int tid = threadIdx.x;
    
    if (tid < RADIX) {
        local_hist[tid] = 0;
    }
    __syncthreads();
    
    for (int idx = blockIdx.x * blockDim.x + tid; idx < n; idx += blockDim.x * gridDim.x) {
        unsigned int digit = extract_digit(data[idx], shift);
        atomicAdd(&local_hist[digit], 1);
    }
    __syncthreads();
    
    if (tid < RADIX) {
        atomicAdd(&hist[tid], local_hist[tid]);
    }
}

__global__ void scatter_kernel(const int* input, int* output, const int* offsets, int n, int shift) {
    __shared__ int local_offsets[RADIX];
    
    if (threadIdx.x < RADIX) {
        local_offsets[threadIdx.x] = offsets[threadIdx.x];
    }
    __syncthreads();
    
    for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n; idx += blockDim.x * gridDim.x) {
        int value = input[idx];
        unsigned int digit = extract_digit(value, shift);
        int pos = atomicAdd(&local_offsets[digit], 1);
        output[pos] = value;
    }
}

}  // namespace

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;
    
    int* d_temp;
    cudaMalloc(&d_temp, n * sizeof(int));
    int* d_hist;
    cudaMalloc(&d_hist, RADIX * sizeof(int));
    int* d_offsets;
    cudaMalloc(&d_offsets, RADIX * sizeof(int));
    
    int* src = const_cast<int*>(input);
    int* dst = output;
    
    cudaMemcpy(dst, input, n * sizeof(int), cudaMemcpyDeviceToDevice);
    
    for (int iter = 0; iter < iters; ++iter) {
        if (iter > 0) {
            cudaMemcpy(dst, input, n * sizeof(int), cudaMemcpyDeviceToDevice);
        }
        
        for (int shift = 0; shift < 32; shift += RADIX_BITS) {
            cudaMemset(d_hist, 0, RADIX * sizeof(int));
            
            int num_blocks = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;
            histogram_kernel<<<num_blocks, BLOCK_SIZE>>>(dst, d_hist, n, shift);
            
            int h_hist[RADIX];
            cudaMemcpy(h_hist, d_hist, RADIX * sizeof(int), cudaMemcpyDeviceToHost);
            
            int h_offsets[RADIX];
            h_offsets[0] = 0;
            for (int i = 1; i < RADIX; ++i) {
                h_offsets[i] = h_offsets[i - 1] + h_hist[i - 1];
            }
            
            cudaMemcpy(d_offsets, h_offsets, RADIX * sizeof(int), cudaMemcpyHostToDevice);
            
            scatter_kernel<<<num_blocks, BLOCK_SIZE>>>(dst, d_temp, d_offsets, n, shift);
            
            int* tmp = dst;
            dst = d_temp;
            d_temp = tmp;
        }
        
        if (dst != output) {
            cudaMemcpy(output, dst, n * sizeof(int), cudaMemcpyDeviceToDevice);
            dst = output;
            d_temp = (dst == output) ? src : output;
        }
    }
    
    cudaFree(d_temp);
    cudaFree(d_hist);
    cudaFree(d_offsets);
}