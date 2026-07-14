#include "interface.h"
#include <cuda_runtime.h>

namespace {

static inline int ceil_div(int a, int b) { return (a + b - 1) / b; }

__global__ void reduce_stage1_kernel(const float* __restrict__ input,
                                     float* __restrict__ partial,
                                     int n) {
    __shared__ float smem[256];

    const int tid = threadIdx.x;
    const int block_threads = blockDim.x;
    const int grid_stride = block_threads * 2 * gridDim.x;
    int idx = blockIdx.x * (block_threads * 2) + tid;

    float sum = 0.0f;
    while (idx < n) {
        sum += input[idx];
        const int idx2 = idx + block_threads;
        if (idx2 < n) sum += input[idx2];
        idx += grid_stride;
    }

    smem[tid] = sum;
    __syncthreads();

    if (block_threads >= 256) {
        if (tid < 128) smem[tid] += smem[tid + 128];
        __syncthreads();
    }
    if (block_threads >= 128) {
        if (tid < 64) smem[tid] += smem[tid + 64];
        __syncthreads();
    }

    if (tid < 32) {
        volatile float* vsmem = smem;
        if (block_threads >= 64) vsmem[tid] += vsmem[tid + 32];
        if (block_threads >= 32) vsmem[tid] += vsmem[tid + 16];
        if (block_threads >= 16) vsmem[tid] += vsmem[tid + 8];
        if (block_threads >= 8) vsmem[tid] += vsmem[tid + 4];
        if (block_threads >= 4) vsmem[tid] += vsmem[tid + 2];
        if (block_threads >= 2) vsmem[tid] += vsmem[tid + 1];
    }

    if (tid == 0) partial[blockIdx.x] = smem[0];
}

__global__ void reduce_stage2_kernel(const float* __restrict__ partial,
                                     float* __restrict__ output,
                                     int n) {
    __shared__ float smem[256];

    const int tid = threadIdx.x;
    float sum = 0.0f;

    for (int i = tid; i < n; i += blockDim.x) sum += partial[i];

    smem[tid] = sum;
    __syncthreads();

    if (blockDim.x >= 256) {
        if (tid < 128) smem[tid] += smem[tid + 128];
        __syncthreads();
    }
    if (blockDim.x >= 128) {
        if (tid < 64) smem[tid] += smem[tid + 64];
        __syncthreads();
    }

    if (tid < 32) {
        volatile float* vsmem = smem;
        if (blockDim.x >= 64) vsmem[tid] += vsmem[tid + 32];
        if (blockDim.x >= 32) vsmem[tid] += vsmem[tid + 16];
        if (blockDim.x >= 16) vsmem[tid] += vsmem[tid + 8];
        if (blockDim.x >= 8) vsmem[tid] += vsmem[tid + 4];
        if (blockDim.x >= 4) vsmem[tid] += vsmem[tid + 2];
        if (blockDim.x >= 2) vsmem[tid] += vsmem[tid + 1];
    }

    if (tid == 0) *output = smem[0];
}

}  // namespace

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0 || iters <= 0) return 0.0f;

    constexpr int threads = 256;

    int device = 0;
    cudaGetDevice(&device);
    cudaDeviceProp prop{};
    cudaGetDeviceProperties(&prop, device);

    int blocks = prop.multiProcessorCount * 4;
    int max_blocks = ceil_div(n, threads * 2);
    if (blocks > max_blocks) blocks = max_blocks;
    if (blocks < 1) blocks = 1;

    float* d_out = nullptr;
    float* d_partial = nullptr;
    cudaMalloc(&d_out, sizeof(float));
    cudaMalloc(&d_partial, static_cast<size_t>(blocks) * sizeof(float));

    for (int iter = 0; iter < iters; ++iter) {
        reduce_stage1_kernel<<<blocks, threads>>>(input, d_partial, n);
        reduce_stage2_kernel<<<1, threads>>>(d_partial, d_out, blocks);
    }

    float actual = 0.0f;
    cudaMemcpy(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost);

    cudaFree(d_partial);
    cudaFree(d_out);
    return actual;
}
