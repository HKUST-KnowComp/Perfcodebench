#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int BINS = 256;
constexpr int BLOCK = 256;
constexpr int GRID  = 1024;

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input,
                                        unsigned int*        __restrict__ blockHists,
                                        int n)
{
    __shared__ unsigned int hist[256 + 32];   // 256 bins + 32 padding to avoid bank conflicts
    for (int i = threadIdx.x; i < 256; i += blockDim.x) hist[i] = 0;
    __syncthreads();

    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    for (int idx = tid; idx < n; idx += gridDim.x * blockDim.x) {
        unsigned char val = input[idx];
        atomicAdd(&hist[val], 1U);
    }
    __syncthreads();

    // write this block’s partial histogram to global memory
    unsigned int* out = blockHists + blockIdx.x * 256;
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        out[i] = hist[i];
    }
}

__global__ void reduce_blocks_kernel(const unsigned int* __restrict__ blockHists,
                                     unsigned int*       __restrict__ globalHist,
                                     int numBlocks)
{
    int bin = threadIdx.x + blockIdx.x * blockDim.x;
    if (bin >= 256) return;

    unsigned int sum = 0;
    for (int b = 0; b < numBlocks; ++b) {
        sum += blockHists[b * 256 + bin];
    }
    globalHist[bin] = sum;
}

} // anonymous namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters)
{
    static unsigned int* dBlockHists = nullptr;
    static size_t        blockBytes  = 0;

    const int numBlocks = GRID;
    size_t needed = numBlocks * 256 * sizeof(unsigned int);
    if (dBlockHists == nullptr || blockBytes < needed) {
        cudaFree(dBlockHists);
        cudaMalloc(&dBlockHists, needed);
        blockBytes = needed;
    }

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));

        histogram_shared_kernel<<<numBlocks, BLOCK>>>(input, dBlockHists, n);

        reduce_blocks_kernel<<<1, 256>>>(dBlockHists, output, numBlocks);
    }
}