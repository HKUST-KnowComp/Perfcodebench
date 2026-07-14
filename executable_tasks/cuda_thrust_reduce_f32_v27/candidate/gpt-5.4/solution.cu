#include "interface.h"
#include <cuda_runtime.h>

namespace {

static inline int div_up(int a, int b) {
    return (a + b - 1) / b;
}

__inline__ __device__ float warp_reduce_sum(float v) {
    unsigned mask = 0xffffffffu;
    v += __shfl_down_sync(mask, v, 16);
    v += __shfl_down_sync(mask, v, 8);
    v += __shfl_down_sync(mask, v, 4);
    v += __shfl_down_sync(mask, v, 2);
    v += __shfl_down_sync(mask, v, 1);
    return v;
}

__global__ void reduce_stage1_kernel(const float* __restrict__ input,
                                     float* __restrict__ partial,
                                     int n) {
    __shared__ float warp_sums[32];

    const int tid = threadIdx.x;
    const int lane = tid & 31;
    const int warp = tid >> 5;
    const int warps_per_block = blockDim.x >> 5;

    const int global_thread = blockIdx.x * blockDim.x + tid;
    const int stride = blockDim.x * gridDim.x;

    float sum = 0.0f;

    int i = global_thread;
    const int stride4 = stride * 4;
    for (; i + 3 * stride < n; i += stride4) {
        sum += input[i];
        sum += input[i + stride];
        sum += input[i + 2 * stride];
        sum += input[i + 3 * stride];
    }
    for (; i < n; i += stride) {
        sum += input[i];
    }

    sum = warp_reduce_sum(sum);

    if (lane == 0) {
        warp_sums[warp] = sum;
    }
    __syncthreads();

    float block_sum = 0.0f;
    if (warp == 0) {
        block_sum = (lane < warps_per_block) ? warp_sums[lane] : 0.0f;
        block_sum = warp_reduce_sum(block_sum);
        if (lane == 0) {
            partial[blockIdx.x] = block_sum;
        }
    }
}

__global__ void reduce_final_kernel(const float* __restrict__ partial,
                                    float* __restrict__ output,
                                    int n) {
    __shared__ float warp_sums[32];

    const int tid = threadIdx.x;
    const int lane = tid & 31;
    const int warp = tid >> 5;
    const int warps_per_block = blockDim.x >> 5;

    float sum = 0.0f;
    for (int i = tid; i < n; i += blockDim.x) {
        sum += partial[i];
    }

    sum = warp_reduce_sum(sum);
    if (lane == 0) {
        warp_sums[warp] = sum;
    }
    __syncthreads();

    if (warp == 0) {
        float total = (lane < warps_per_block) ? warp_sums[lane] : 0.0f;
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

    int device = 0;
    cudaGetDevice(&device);
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, device);

    const int block_size = 256;
    int grid_size = div_up(n, block_size * 4);
    const int max_blocks = prop.multiProcessorCount * 8;
    if (grid_size > max_blocks) grid_size = max_blocks;
    if (grid_size < 1) grid_size = 1;

    float* d_partial = nullptr;
    float* d_out = nullptr;
    cudaMalloc(&d_partial, static_cast<size_t>(grid_size) * sizeof(float));
    cudaMalloc(&d_out, sizeof(float));

    for (int iter = 0; iter < iters; ++iter) {
        reduce_stage1_kernel<<<grid_size, block_size>>>(input, d_partial, n);
        reduce_final_kernel<<<1, block_size>>>(d_partial, d_out, grid_size);
    }

    float actual = 0.0f;
    cudaMemcpy(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost);

    cudaFree(d_out);
    cudaFree(d_partial);
    return actual;
}
