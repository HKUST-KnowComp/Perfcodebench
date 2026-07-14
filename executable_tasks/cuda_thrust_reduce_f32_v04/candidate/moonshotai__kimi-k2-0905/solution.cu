#include "interface.h"
#include <cuda_runtime.h>
#include <cfloat>

__global__ void reduce_f32_kernel(const float* __restrict__ input,
                                  float* __restrict__ out,
                                  int n)
{
    const int tid   = threadIdx.x;
    const int gtid  = blockIdx.x * blockDim.x + tid;

    // Each thread loads data
    float val = 0.0f;
    for (int i = gtid; i < n; i += gridDim.x * blockDim.x)
        val += input[i];

    // Warp-level reduction using shfl_down_sync
    for (int offset = warpSize / 2; offset > 0; offset /= 2)
        val += __shfl_down_sync(0xFFFFFFFF, val, offset);

    // First lane of each warp writes partial sum to shared memory
    __shared__ float warpSums[32];
    const int laneId = tid & 31;
    const int warpId = tid >> 5;
    if (laneId == 0)
        warpSums[warpId] = val;
    __syncthreads();

    // Final reduction inside block by first warp
    if (tid < 32)
    {
        val = (tid < (blockDim.x >> 5)) ? warpSums[tid] : 0.0f;
        for (int offset = warpSize / 2; offset > 0; offset /= 2)
            val += __shfl_down_sync(0xFFFFFFFF, val, offset);

        // Atomically add block result to global accumulator
        if (tid == 0)
            atomicAdd(out, val);
    }
}

float reduce_f32(const float* input, int n, int iters)
{
    static float* d_out = nullptr;
    if (d_out == nullptr)
        cudaMalloc(&d_out, sizeof(float));

    const int threads = 1024;
    const int blocks  = 128;

    float result = 0.0f;
    for (int i = 0; i < iters; ++i)
    {
        cudaMemsetAsync(d_out, 0, sizeof(float));
        reduce_f32_kernel<<<blocks, threads>>>(input, d_out, n);
    }
    cudaMemcpy(&result, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    return result;
}