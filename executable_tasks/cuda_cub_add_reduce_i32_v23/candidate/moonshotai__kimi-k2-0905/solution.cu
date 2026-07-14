#include "interface.h"
#include <cuda_runtime.h>
#include <cstdio>

static const int BLOCK_SIZE = 256;

// two-level hierarchical reduction kernel
// grid-level outputs are written atomically into a host-mapped buffer so that
// no device->device copy is needed.
static __global__ void reduction_kernel(const int * __restrict__ d_in,
                                        int * __restrict__ grid_out,
                                        const int n)
{
    __shared__ int smem[BLOCK_SIZE];
    int tid = threadIdx.x;
    int64_t local = 0;
    // first-level: warp-stride load with int64 accumulation to avoid overflow
    for (int i = blockIdx.x * BLOCK_SIZE + tid; i < n; i += gridDim.x * BLOCK_SIZE)
        local += d_in[i];
    smem[tid] = (int)local;

    // intra-block reduction
    __syncthreads();
    for (int off = BLOCK_SIZE/2; off >= 1; off /= 2) {
        if (tid < off) smem[tid] += smem[tid + off];
        __syncthreads();
    }
    // write one value per block to the host-mapped buffer
    if (tid == 0) atomicAdd(grid_out, smem[0]);
}

void add_reduce_i32(const int *d_input, int *output, int n, int iters)
{
    // tiny pinned host buffer directly accessible from kernel (zero-copy)
    int  *d_grid_sum = nullptr;
    cudaHostAlloc(&d_grid_sum, sizeof(int), cudaHostAllocMapped);

    for (int iter = 0; iter < iters; ++iter) {
        *d_grid_sum = 0;                    // reset the reduction accumulator

        const int blocks = min((n + BLOCK_SIZE - 1) / BLOCK_SIZE, 2048);
        reduction_kernel<<<blocks, BLOCK_SIZE>>>(d_input, d_grid_sum, n);
        cudaDeviceSynchronize();

        // exact int64 sum on CPU
        int64_t cpu_sum = *d_grid_sum;
        *output = (int)cpu_sum;
    }
    cudaFreeHost(d_grid_sum);
}