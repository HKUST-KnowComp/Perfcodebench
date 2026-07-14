#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

namespace {
__global__ void reduce_kernel(const float* __restrict__ in,
                              float* __restrict__ out,
                              int num_items)
{
    using BlockReduce = cub::BlockReduce<float, 256>;
    __shared__ typename BlockReduce::TempStorage temp;

    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    float partial = 0.0f;
    for (int i = tid; i < num_items; i += blockDim.x * gridDim.x)
        partial += in[i];

    partial = BlockReduce(temp).Sum(partial);

    if (threadIdx.x == 0)
        atomicAdd(out, partial);
}

__global__ void final_reduce_kernel(float* partial, int blocks)
{
    using BlockReduce = cub::BlockReduce<float, 256>;
    __shared__ typename BlockReduce::TempStorage temp;

    float sum = 0.0f;
    for (int i = threadIdx.x; i < blocks; i += blockDim.x)
        sum += partial[i];

    sum = BlockReduce(temp).Sum(sum);
    if (threadIdx.x == 0)
        partial[0] = sum;
}

int div_up(int a, int b) { return (a + b - 1) / b; }

} // anonymous

float reduce_f32(const float* input, int n, int iters)
{
    float* d_out = nullptr;
    const int blocks = 1024;
    const int threads = 256;
    cudaMalloc(&d_out, (blocks + 1) * sizeof(float));

    float result = 0.0f;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(d_out, 0, sizeof(float));          // clear accumulator
        reduce_kernel<<<blocks, threads>>>(input, d_out, n);
        final_reduce_kernel<<<1, threads>>>(d_out, blocks);
    }
    cudaMemcpy(&result, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    return result;
}