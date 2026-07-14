#include "interface.h"
#include <cuda_runtime.h>

namespace {

static inline int ceil_div(int a, int b) { return (a + b - 1) / b; }

__global__ void reduce_stage1_kernel(const float* __restrict__ input,
                                     float* __restrict__ partial,
                                     int n) {
    __shared__ float sdata[256];

    const unsigned int tid = threadIdx.x;
    const unsigned int block_threads = blockDim.x;
    const unsigned int grid_stride = block_threads * 2 * gridDim.x;
    unsigned int i = blockIdx.x * (block_threads * 2) + tid;

    float sum = 0.0f;
    while (i < (unsigned int)n) {
        sum += input[i];
        unsigned int j = i + block_threads;
        if (j < (unsigned int)n) sum += input[j];
        i += grid_stride;
    }

    sdata[tid] = sum;
    __syncthreads();

    if (block_threads >= 256) {
        if (tid < 128) sdata[tid] += sdata[tid + 128];
        __syncthreads();
    }
    if (block_threads >= 128) {
        if (tid < 64) sdata[tid] += sdata[tid + 64];
        __syncthreads();
    }

    if (tid < 32) {
        volatile float* vsmem = sdata;
        if (block_threads >= 64) vsmem[tid] += vsmem[tid + 32];
        if (block_threads >= 32) vsmem[tid] += vsmem[tid + 16];
        if (block_threads >= 16) vsmem[tid] += vsmem[tid + 8];
        if (block_threads >= 8) vsmem[tid] += vsmem[tid + 4];
        if (block_threads >= 4) vsmem[tid] += vsmem[tid + 2];
        if (block_threads >= 2) vsmem[tid] += vsmem[tid + 1];
    }

    if (tid == 0) partial[blockIdx.x] = sdata[0];
}

__global__ void reduce_final_kernel(const float* __restrict__ partial,
                                    float* __restrict__ output,
                                    int n) {
    __shared__ float sdata[256];

    const unsigned int tid = threadIdx.x;
    float sum = 0.0f;

    for (unsigned int i = tid; i < (unsigned int)n; i += blockDim.x) {
        sum += partial[i];
    }

    sdata[tid] = sum;
    __syncthreads();

    if (blockDim.x >= 256) {
        if (tid < 128) sdata[tid] += sdata[tid + 128];
        __syncthreads();
    }
    if (blockDim.x >= 128) {
        if (tid < 64) sdata[tid] += sdata[tid + 64];
        __syncthreads();
    }

    if (tid < 32) {
        volatile float* vsmem = sdata;
        if (blockDim.x >= 64) vsmem[tid] += vsmem[tid + 32];
        if (blockDim.x >= 32) vsmem[tid] += vsmem[tid + 16];
        if (blockDim.x >= 16) vsmem[tid] += vsmem[tid + 8];
        if (blockDim.x >= 8) vsmem[tid] += vsmem[tid + 4];
        if (blockDim.x >= 4) vsmem[tid] += vsmem[tid + 2];
        if (blockDim.x >= 2) vsmem[tid] += vsmem[tid + 1];
    }

    if (tid == 0) *output = sdata[0];
}

}  // namespace

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0 || iters <= 0) return 0.0f;

    constexpr int block_size = 256;
    int grid_size = ceil_div(n, block_size * 2);
    if (grid_size < 1) grid_size = 1;
    if (grid_size > 1024) grid_size = 1024;

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

    cudaFree(d_partial);
    cudaFree(d_out);
    return actual;
}
