#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void optimized_reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    int sum = 0;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    // Use vectorized loads to maximize memory throughput
    // Ensure we handle alignment and the remainder of the division by 4
    const int4* input4 = reinterpret_cast<const int4*>(input);
    int n4 = n / 4;
    
    // Grid-stride loop with int4
    for (int i = idx; i < n4; i += stride) {
        int4 val = input4[i];
        sum += val.x + val.y + val.z + val.w;
    }

    // Handle remaining elements if n is not a multiple of 4
    for (int i = n4 * 4 + idx; i < n; i += stride) {
        sum += input[i];
    }

    // Warp-level reduction using shuffle instructions
    for (int offset = 16; offset > 0; offset /= 2) {
        sum += __shfl_down_sync(0xffffffff, sum, offset);
    }

    // Block-level reduction using shared memory
    __shared__ int shared_sums[32]; // Max 1024 threads / 32 = 32 warps
    int lane = threadIdx.x % 32;
    int wid = threadIdx.x / 32;

    if (lane == 0) {
        shared_sums[wid] = sum;
    }

    __syncthreads();

    // Final reduction within the first warp
    if (wid == 0) {
        sum = (threadIdx.x < (blockDim.x + 31) / 32) ? shared_sums[lane] : 0;
        for (int offset = 16; offset > 0; offset /= 2) {
            sum += __shfl_down_sync(0xffffffff, sum, offset);
        }
        // Only one atomicAdd per block to global memory
        if (lane == 0) {
            atomicAdd(output, sum);
        }
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    // Configuration tuned for modern GPUs
    const int threads_per_block = 256;
    // Calculate grid size to saturate the GPU without excessive overhead
    // Using a fixed large grid for grid-stride loop efficiency
    int device_id = 0;
    int num_sm = 0;
    cudaDeviceGetAttribute(&num_sm, cudaDevAttrMultiProcessorCount, device_id);
    const int grid_size = num_sm * 4; 

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        
        // Check if input is aligned for int4 (16-byte alignment)
        if (n >= 4 && (reinterpret_cast<uintptr_t>(input) % 16 == 0)) {
            optimized_reduce_kernel<<<grid_size, threads_per_block>>>(input, output, n);
        } else {
            // Fallback for unaligned or very small arrays (though unlikely in performance tasks)
            auto simple_kernel = [] __global__ (const int* in, int* out, int size) {
                int s = 0;
                for (int i = blockIdx.x * blockDim.x + threadIdx.x; i < size; i += blockDim.x * gridDim.x) {
                    s += in[i];
                }
                for (int offset = 16; offset > 0; offset /= 2) s += __shfl_down_sync(0xffffffff, s, offset);
                __shared__ int sh[32];
                int l = threadIdx.x % 32, w = threadIdx.x / 32;
                if (l == 0) sh[w] = s;
                __syncthreads();
                if (w == 0) {
                    s = (threadIdx.x < (blockDim.x + 31) / 32) ? sh[l] : 0;
                    for (int offset = 16; offset > 0; offset /= 2) s += __shfl_down_sync(0xffffffff, s, offset);
                    if (l == 0) atomicAdd(out, s);
                }
            };
            simple_kernel<<<grid_size, threads_per_block>>>(input, output, n);
        }
    }
}