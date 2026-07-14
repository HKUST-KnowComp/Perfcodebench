#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input,
                                        unsigned int* __restrict__ blockOut,
                                        int n)
{
    __shared__ unsigned int warpHist[256];
    const int warpId   = threadIdx.x >> 5;
    const int laneId   = threadIdx.x & 31;
    const int warpsPerBlock = blockDim.x >> 5;

    // initialise shared memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x)
        warpHist[i] = 0;
    __syncthreads();

    // each thread processes multiple elements
    for (int idx = blockIdx.x * blockDim.x + threadIdx.x;
         idx < n;
         idx += gridDim.x * blockDim.x)
    {
        unsigned char val = input[idx];
        // warp-aggregated increment: elect one lane to do the update
        unsigned mask = __activemask();
        unsigned ballot = __ballot_sync(mask, val == val); // all lanes participate
        int leader = __ffs(mask) - 1;
        if (laneId == leader)
            warpHist[val] += __popc(ballot);
    }
    __syncthreads();

    // write per-warp partial histogram to global memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x)
        blockOut[blockIdx.x * 256 + i] = warpHist[i];
}

__global__ void reduce_blocks_kernel(const unsigned int* __restrict__ blockHist,
                                     unsigned int* __restrict__ out,
                                     int numBlocks)
{
    int bin = threadIdx.x + blockIdx.x * blockDim.x;
    if (bin >= 256) return;

    unsigned int sum = 0;
    for (int b = 0; b < numBlocks; ++b)
        sum += blockHist[b * 256 + bin];
    out[bin] = sum;
}

} // anonymous namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters)
{
    const int threadsPerBlock = 256;
    const int warpsPerBlock   = threadsPerBlock / 32;
    // choose grid size to keep all SMs busy and provide enough parallelism
    int blocks = (n + threadsPerBlock - 1) / threadsPerBlock;
    const int maxBlocks = 1024;
    if (blocks > maxBlocks) blocks = maxBlocks;

    unsigned int* dBlockHist;
    cudaMalloc(&dBlockHist, blocks * 256 * sizeof(unsigned int));

    for (int iter = 0; iter < iters; ++iter)
    {
        histogram_shared_kernel<<<blocks, threadsPerBlock>>>(input, dBlockHist, n);
        reduce_blocks_kernel<<<1, 256>>>(dBlockHist, output, blocks);
    }
    cudaFree(dBlockHist);
}