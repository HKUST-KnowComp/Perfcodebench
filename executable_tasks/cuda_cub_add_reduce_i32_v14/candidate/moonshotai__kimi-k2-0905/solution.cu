#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>
#include <cstdint>

namespace {

constexpr int BLOCK = 256;
constexpr int ITEMS_PER_THREAD = 16;

__global__ void reduce_kernel(const int* __restrict__ in,
                              int64_t* __restrict__ blockSums,
                              int n) {
    typedef cub::BlockReduce<int64_t, BLOCK> BlockReduce;
    __shared__ typename BlockReduce::TempStorage temp;

    int64_t threadSum = 0;
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; ++i) {
        int idx = tid + i * gridDim.x * blockDim.x;
        if (idx < n) threadSum += static_cast<int64_t>(in[idx]);
    }
    int64_t blockSum = BlockReduce(temp).Sum(threadSum);
    if (threadIdx.x == 0) blockSums[blockIdx.x] = blockSum;
}

__global__ void final_reduce_kernel(const int64_t* __restrict__ blockSums,
                                    int numBlocks,
                                    int* __restrict__ out) {
    typedef cub::BlockReduce<int64_t, BLOCK> BlockReduce;
    __shared__ typename BlockReduce::TempStorage temp;

    int64_t threadSum = 0;
    for (int i = threadIdx.x; i < numBlocks; i += blockDim.x)
        threadSum += blockSums[i];
    int64_t blockSum = BlockReduce(temp).Sum(threadSum);
    if (threadIdx.x == 0) *out = static_cast<int32_t>(blockSum);
}

int64_t* d_tmp = nullptr;
int maxBlocks = 0;

void ensure_workspace(int n) {
    int needed = (n + BLOCK * ITEMS_PER_THREAD - 1) / (BLOCK * ITEMS_PER_THREAD);
    if (needed > maxBlocks) {
        cudaFree(d_tmp);
        maxBlocks = needed;
        cudaMalloc(&d_tmp, maxBlocks * sizeof(int64_t));
    }
}

}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    ensure_workspace(n);
    int numBlocks = (n + BLOCK * ITEMS_PER_THREAD - 1) / (BLOCK * ITEMS_PER_THREAD);
    for (int iter = 0; iter < iters; ++iter) {
        reduce_kernel<<<numBlocks, BLOCK>>>(input, d_tmp, n);
        final_reduce_kernel<<<1, BLOCK>>>(d_tmp, numBlocks, output);
    }
}