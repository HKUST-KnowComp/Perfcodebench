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

__global__ void reduce_stage1_kernel(const float* __restrict__ input,
                                     float* __restrict__ partial,
                                     int n) {
    float sum = 0.0f;

    const int tid = threadIdx.x;
    const int block_threads = blockDim.x;
    const int global_thread = blockIdx.x * block_threads + tid;
    const int stride = gridDim.x * block_threads;

    for (int i = global_thread; i < n; i += stride) {
        sum += input[i];
    }

    sum = warp_reduce_sum(sum);

    __shared__ float warp_sums[32];
    const int lane = tid & 31;
    const int warp_id = tid >> 5;
    const int warp_count = block_threads >> 5;

    if (lane == 0) {
        warp_sums[warp_id] = sum;
    }
    __syncthreads();

    float block_sum = 0.0f;
    if (warp_id == 0) {
        block_sum = (lane < warp_count) ? warp_sums[lane] : 0.0f;
        block_sum = warp_reduce_sum(block_sum);
        if (lane == 0) {
            partial[blockIdx.x] = block_sum;
        }
    }
}

__global__ void reduce_final_kernel(const float* __restrict__ partial,
                                    float* __restrict__ output,
                                    int n) {
    float sum = 0.0f;
    const int tid = threadIdx.x;
    const int stride = blockDim.x;

    for (int i = tid; i < n; i += stride) {
        sum += partial[i];
    }

    sum = warp_reduce_sum(sum);

    __shared__ float warp_sums[32];
    const int lane = tid & 31;
    const int warp_id = tid >> 5;
    const int warp_count = blockDim.x >> 5;

    if (lane == 0) {
        warp_sums[warp_id] = sum;
    }
    __syncthreads();

    if (warp_id == 0) {
        float block_sum = (lane < warp_count) ? warp_sums[lane] : 0.0f;
        block_sum = warp_reduce_sum(block_sum);
        if (lane == 0) {
            *output = block_sum;
        }
    }
}

}  // namespace

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0 || input == nullptr || iters <= 0) {
        return 0.0f;
    }

    constexpr int kBlockSize = 256;
    int device = 0;
    cudaGetDevice(&device);

    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, device);

    int sm_count = prop.multiProcessorCount > 0 ? prop.multiProcessorCount : 1;
    int max_blocks = sm_count * 8;
    int grid = div_up(n, kBlockSize);
    if (grid > max_blocks) grid = max_blocks;
    if (grid < 1) grid = 1;

    float* d_out = nullptr;
    float* d_partial = nullptr;
    cudaMalloc(&d_out, sizeof(float));
    cudaMalloc(&d_partial, static_cast<size_t>(grid) * sizeof(float));

    for (int iter = 0; iter < iters; ++iter) {
        reduce_stage1_kernel<<<grid, kBlockSize>>>(input, d_partial, n);
        reduce_final_kernel<<<1, kBlockSize>>>(d_partial, d_out, grid);
    }

    float actual = 0.0f;
    cudaMemcpy(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost);

    cudaFree(d_partial);
    cudaFree(d_out);
    return actual;
}
