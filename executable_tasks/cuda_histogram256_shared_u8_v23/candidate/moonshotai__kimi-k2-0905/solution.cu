#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_privatised(const unsigned char* __restrict__ input,
                                     unsigned int* __restrict__ out,
                                     int n,
                                     unsigned int* __restrict__ blockBins)
{
    __shared__ unsigned int warpHist[8][32];   // 8 warps × 32 bins (bank-conflict free)

    const int tid   = threadIdx.x;
    const int warp  = tid >> 5;
    const int lane  = tid & 31;

    // zero shared memory
    for (int i = 0; i < 8; i += 2) {
        warpHist[warp][lane] = 0;
        if (warp + 8 < 8) warpHist[warp + 8][lane] = 0;
    }
    __syncthreads();

    // accumulate into privatised warp histograms
    for (int idx = blockIdx.x * blockDim.x + tid; idx < n; idx += blockDim.x * gridDim.x) {
        unsigned char val = input[idx];
        atomicAdd(&warpHist[warp][val & 31], 1U);
    }
    __syncthreads();

    // reduce 8 warp histograms into one per-block histogram in global memory
    const int bin = tid;
    if (bin < 256) {
        unsigned int sum = 0;
        for (int w = 0; w < 8; ++w)
            sum += warpHist[w][bin & 31];
        blockBins[blockIdx.x * 256 + bin] = sum;
    }
}

__global__ void reduce_blocks(const unsigned int* __restrict__ blockBins,
                              unsigned int* __restrict__ out)
{
    const int bin = threadIdx.x + blockIdx.x * blockDim.x;
    if (bin >= 256) return;

    unsigned int sum = 0;
    for (int b = 0; b < gridDim.y; ++b)
        sum += blockBins[b * 256 + bin];
    out[bin] = sum;
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters)
{
    const int block = 256;
    const int grid  = 1024;

    unsigned int* dBlockHist = nullptr;
    cudaMalloc(&dBlockHist, grid * 256 * sizeof(unsigned int));

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(dBlockHist, 0, grid * 256 * sizeof(unsigned int));

        histogram_privatised<<<grid, block>>>(input, output, n, dBlockHist);

        reduce_blocks<<<1, 256>>>(dBlockHist, output);
    }

    cudaFree(dBlockHist);
}