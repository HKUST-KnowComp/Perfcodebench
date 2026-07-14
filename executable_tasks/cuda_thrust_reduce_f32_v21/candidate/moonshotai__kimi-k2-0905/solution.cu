#include "interface.h"
#include <cuda_runtime.h>
#include <cstdio>

namespace {

__device__ inline float warpReduceSum(float val) {
    for (int offset = warpSize/2; offset > 0; offset /= 2)
        val += __shfl_down_sync(0xffffffff, val, offset);
    return val;
}

__global__ void reduceBlockKernel(const float* __restrict__ input,
                                  float* __restrict__ blockSums,
                                  int n)
{
    const int tid = threadIdx.x;
    const int gid = blockIdx.x * blockDim.x + tid;
    const int lane = tid & 31;
    const int warpid = tid >> 5;

    extern __shared__ float shared[];

    float val = 0.0f;
    // grid-stride load
    for (int i = gid; i < n; i += blockDim.x * gridDim.x)
        val += input[i];

    // warp reduction
    val = warpReduceSum(val);

    // write warp result to shared
    if (lane == 0) shared[warpid] = val;
    __syncthreads();

    // final warp reduces shared partials
    val = (tid < (blockDim.x + warpSize - 1) / warpSize) ? shared[lane] : 0.0f;
    if (warpid == 0) val = warpReduceSum(val);

    // block result
    if (tid == 0) blockSums[blockIdx.x] = val;
}

__global__ void reduceFinalKernel(const float* __restrict__ blockSums,
                                  float* __restrict__ result,
                                  int numBlocks)
{
    const int tid = threadIdx.x;
    extern __shared__ float shared[];

    float val = 0.0f;
    if (tid < numBlocks) val = blockSums[tid];
    shared[tid] = val;
    __syncthreads();

    // power-of-two reduction in shared memory
    for (int stride = blockDim.x/2; stride > 0; stride >>= 1) {
        if (tid < stride) shared[tid] += shared[tid + stride];
        __syncthreads();
    }
    if (tid == 0) *result = shared[0];
}

} // anon namespace

float reduce_f32(const float* input, int n, int iters)
{
    const int blockSize = 256;
    const int warpsPerBlock = blockSize / 32;
    const int smem = warpsPerBlock * sizeof(float);

    int numBlocks = (n + blockSize - 1) / blockSize;
    // cap blocks to avoid too many partials
    if (numBlocks > 1024) numBlocks = 1024;

    float *dBlockSums = nullptr;
    float *dResult = nullptr;
    cudaMalloc(&dBlockSums, numBlocks * sizeof(float));
    cudaMalloc(&dResult, sizeof(float));

    for (int i = 0; i < iters; ++i) {
        reduceBlockKernel<<<numBlocks, blockSize, smem>>>(input, dBlockSums, n);
        reduceFinalKernel<<<1, numBlocks, numBlocks * sizeof(float)>>>(dBlockSums, dResult, numBlocks);
    }

    float hResult = 0.0f;
    cudaMemcpy(&hResult, dResult, sizeof(float), cudaMemcpyDeviceToHost);

    cudaFree(dBlockSums);
    cudaFree(dResult);
    return hResult;
}