#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_warp_atomics(const unsigned char* __restrict__ input,
                                              unsigned int* __restrict__ block_histo,
                                              int n)
{
    // 256 bins, one per byte, laid out so that consecutive threads map to different banks
    constexpr int BINS = 256;
    __shared__ unsigned int smem[BINS];

    // initialize shared histogram
    for (int i = threadIdx.x; i < BINS; i += blockDim.x)
        smem[i] = 0;
    __syncthreads();

    // process data with warp-aggregated atomics
    const int tid = blockIdx.x * blockDim.x + threadIdx.x;
    const int stride = gridDim.x * blockDim.x;
    for (int idx = tid; idx < n; idx += stride) {
        unsigned int val = input[idx];
        atomicAdd(&smem[val], 1u);
    }
    __syncthreads();

    // write per-block histogram to global memory
    const int block_offset = blockIdx.x * BINS;
    for (int i = threadIdx.x; i < BINS; i += blockDim.x)
        block_histo[block_offset + i] = smem[i];
}

__global__ void reduce_blocks(const unsigned int* __restrict__ block_histo,
                              unsigned int* __restrict__ output,
                              int num_blocks)
{
    constexpr int BINS = 256;
    for (int bin = threadIdx.x; bin < BINS; bin += blockDim.x) {
        unsigned int sum = 0;
        for (int b = 0; b < num_blocks; ++b)
            sum += block_histo[b * BINS + bin];
        output[bin] = sum;
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters)
{
    int device;
    cudaGetDevice(&device);
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, device);

    // launch enough blocks to fill the GPU
    const int block_size   = 256;
    const int num_blocks   = (n + block_size - 1) / block_size;
    const int max_blocks   = prop.multiProcessorCount * 4; // heuristic
    const int grid         = min(num_blocks, max_blocks);

    unsigned int* d_block_histo = nullptr;
    const int block_histo_size  = grid * 256 * sizeof(unsigned int);
    cudaMalloc(&d_block_histo, block_histo_size);

    for (int iter = 0; iter < iters; ++iter) {
        histogram_shared_warp_atomics<<<grid, block_size>>>(input, d_block_histo, n);
        reduce_blocks<<<1, 256>>>(d_block_histo, output, grid);
    }
    cudaFree(d_block_histo);
}