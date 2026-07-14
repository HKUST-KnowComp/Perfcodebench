#include "interface.cuh"
#include <cuda_runtime.h>
#include <cstdio>

namespace {

// --------------- Two-stage reduction ----------------------------------
constexpr int BLOCK = 256;                 // threads per block
constexpr int ELEMS_PER_THREAD = 16;       // work per thread

template<int blockSize>
__global__ void reduceTile(const int* __restrict__ in,
                             int* __restrict__ tileOut, int N) {
    // Shared array for one block’s partial sum
    __shared__ int smem[blockSize];
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x * ELEMS_PER_THREAD + tid;

    // Each thread sums ELEMS_PER_THREAD elements
    int local = 0;
    #pragma unroll
    for (int i = 0; i < ELEMS_PER_THREAD; ++i) {
        if (idx < N) {
            local += in[idx];
            idx += blockDim.x;
        }
    }
    smem[tid] = local;
    __syncthreads();

    // Classic tree reduction in shared memory
    #pragma unroll 5
    for (int stride = blockDim.x / 2; stride > 0; stride >>= 1) {
        if (tid < stride) smem[tid] += smem[tid + stride];
        __syncthreads();
    }
    if (tid == 0) tileOut[blockIdx.x] = smem[0];
}

// Final kernel: one block finishes the global sum of all tile results
__global__ void finishSum(const int* tileSums, int* out, int numTiles) {
    __shared__ int smem[BLOCK];
    int tid = threadIdx.x;
    // load
    smem[tid] = 0;
    for (int i = tid; i < numTiles; i += blockDim.x) {
        smem[tid] += tileSums[i];
    }
    __syncthreads();
    // reduce inside one block
    for (int stride = blockDim.x >> 1; stride > 0; stride >>= 1) {
        if (tid < stride) smem[tid] += smem[tid + stride];
        __syncthreads();
    }
    if (tid == 0) *out = smem[0];
}

// host wrapper
void computeTotal(const int* d_in, int* d_out, int n) {
    const int elemsPerTile = BLOCK * ELEMS_PER_THREAD;
    const int numTiles     = (n + elemsPerTile - 1) / elemsPerTile;

    int* d_tileSums = NULL;
    cudaMalloc(&d_tileSums, sizeof(int) * numTiles);

    // clear the final result first
    cudaMemset(d_out, 0, sizeof(int));

    // 1. first pass
    reduceTile<BLOCK><<<numTiles, BLOCK>>>(d_in, d_tileSums, n);
    // 2. finish pass
    finishSum<<<1, BLOCK>>>(d_tileSums, d_out, numTiles);

    cudaFree(d_tileSums);
}

} // anonymous namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n == 0 || iters == 0) return;

    for (int i = 0; i < iters; ++i) {
        computeTotal(input, output, n);
    }
}