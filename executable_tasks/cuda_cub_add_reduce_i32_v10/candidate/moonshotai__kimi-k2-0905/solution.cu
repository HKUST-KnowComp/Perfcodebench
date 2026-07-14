#include "interface.h"
#include <cuda_runtime.h>

namespace {
template<int UNROLL>
__global__ __launch_bounds__(256, 4)
void reduce_kernel(const int* __restrict__ input,
                   int*           __restrict__ block_sums,
                   int                        N)
{
    const int tid = threadIdx.x;
    const int gtid = blockIdx.x * blockDim.x + tid;

    __shared__ int smem[256];
    smem[tid] = 0;

    for (int base = gtid; base < N; base += gridDim.x * blockDim.x * UNROLL) {
        int sum = 0;
        #pragma unroll
        for (int u = 0; u < UNROLL && base + u * blockDim.x * gridDim.x < N; ++u) {
            sum += input[base + u * blockDim.x * gridDim.x];
        }
        smem[tid] += sum;
    }

    // block-level reduction
    typedef int T;
    for (int off = blockDim.x >> 1; off > 0; off >>= 1) {
        __syncthreads();
        if (tid < off) smem[tid] += smem[tid + off];
    }
    if (tid == 0) block_sums[blockIdx.x] = smem[0];
}

__global__ void final_reduce(int* __restrict__ block_sums, int* __restrict__ out) {
    __shared__ int smem[256];
    int bid = blockIdx.x;
    if (bid > 0) return;               // only block 0 does the final work
    int tid = threadIdx.x;
    int val = 0;
    for (int i = tid; i < gridDim.x; i += blockDim.x) val += block_sums[i];
    smem[tid] = val;
    __syncthreads();
    for (int off = blockDim.x >> 1; off > 0; off >>= 1) {
        __syncthreads();
        if (tid < off) smem[tid] += smem[tid + off];
    }
    if (tid == 0) *out = smem[0];
}

static const int UNROLL = 32;
static const int BLOCK  = 256;
static int* d_tmp = nullptr;
static size_t cap = 0;
}  // anon namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    int blocks = (n + BLOCK * UNROLL - 1) / (BLOCK * UNROLL);
    if (blocks > 0) blocks = (blocks < 1024) ? blocks : 1024;
    size_t tmp_bytes = blocks * sizeof(int);
    if (tmp_bytes > cap) {
        cudaFree(d_tmp);
        cudaMalloc(&d_tmp, tmp_bytes);
        cap = tmp_bytes;
    }
    for (int iter = 0; iter < iters; ++iter) {
        reduce_kernel<UNROLL><<<blocks, BLOCK>>>((const int*)input, d_tmp, n);
        final_reduce<<<1, BLOCK>>>(d_tmp, output);
    }
}