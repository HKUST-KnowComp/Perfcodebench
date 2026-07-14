#include "interface.h"
#include <cuda_runtime.h>

namespace {

static inline int div_up(int a, int b) {
    return (a + b - 1) / b;
}

__inline__ __device__ float warp_reduce_sum(float v) {
    for (int offset = 16; offset > 0; offset >>= 1) {
        v += __shfl_down_sync(0xffffffff, v, offset);
    }
    return v;
}

template <int BLOCK_SIZE>
__global__ void reduce_stage1_kernel(const float* __restrict__ input,
                                     float* __restrict__ partial,
                                     int n) {
    float sum = 0.0f;

    const int global_thread = blockIdx.x * BLOCK_SIZE + threadIdx.x;
    const int stride = gridDim.x * BLOCK_SIZE;

    for (int i = global_thread; i < n; i += stride) {
        sum += input[i];
    }

    sum = warp_reduce_sum(sum);

    __shared__ float warp_sums[BLOCK_SIZE / 32];
    const int lane = threadIdx.x & 31;
    const int warp_id = threadIdx.x >> 5;

    if (lane == 0) {
        warp_sums[warp_id] = sum;
    }
    __syncthreads();

    if (warp_id == 0) {
        float block_sum = (lane < (BLOCK_SIZE / 32)) ? warp_sums[lane] : 0.0f;
        block_sum = warp_reduce_sum(block_sum);
        if (lane == 0) {
            partial[blockIdx.x] = block_sum;
        }
    }
}

template <int BLOCK_SIZE>
__global__ void reduce_final_kernel(const float* __restrict__ partial,
                                    float* __restrict__ output,
                                    int n) {
    float sum = 0.0f;

    for (int i = threadIdx.x; i < n; i += BLOCK_SIZE) {
        sum += partial[i];
    }

    sum = warp_reduce_sum(sum);

    __shared__ float warp_sums[BLOCK_SIZE / 32];
    const int lane = threadIdx.x & 31;
    const int warp_id = threadIdx.x >> 5;

    if (lane == 0) {
        warp_sums[warp_id] = sum;
    }
    __syncthreads();

    if (warp_id == 0) {
        float total = (lane < (BLOCK_SIZE / 32)) ? warp_sums[lane] : 0.0f;
        total = warp_reduce_sum(total);
        if (lane == 0) {
            *output = total;
        }
    }
}

}  // namespace

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0 || iters <= 0) {
        return 0.0f;
    }

    constexpr int BLOCK_SIZE = 256;

    int device = 0;
    cudaGetDevice(&device);
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, device);

    int sm_count = prop.multiProcessorCount > 0 ? prop.multiProcessorCount : 1;
    int grid = sm_count * 8;
    int max_grid = div_up(n, BLOCK_SIZE);
    if (grid > max_grid) grid = max_grid;
    if (grid < 1) grid = 1;

    float* d_out = nullptr;
    float* d_partial = nullptr;

    cudaMalloc(&d_out, sizeof(float));
    cudaMalloc(&d_partial, static_cast<size_t>(grid) * sizeof(float));

    for (int iter = 0; iter < iters; ++iter) {
        reduce_stage1_kernel<BLOCK_SIZE><<<grid, BLOCK_SIZE>>>(input, d_partial, n);
        reduce_final_kernel<BLOCK_SIZE><<<1, BLOCK_SIZE>>>(d_partial, d_out, grid);
    }

    float actual = 0.0f;
    cudaMemcpy(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost);

    cudaFree(d_partial);
    cudaFree(d_out);
    return actual;
}
