#include "interface.h"
#include <cuda_runtime.h>

namespace {

template <int blockSize>
__global__ void reduce_kernel(const int* input, int* output, int n) {
    extern __shared__ int smem[];
    int tid = threadIdx.x;
    int sum = 0;
    // grid-stride loop for coalesced global memory access
    for (int i = blockIdx.x * blockSize + tid; i < n; i += gridDim.x * blockSize) {
        sum += input[i];
    }

    // warp shuffle reduction
    int warp_id = tid / warpSize;
    int lane = tid & (warpSize - 1);
    #pragma unroll
    for (int offset = warpSize / 2; offset > 0; offset >>= 1) {
        sum += __shfl_xor_sync(0xffffffff, sum, offset);
    }
    if (lane == 0) {
        smem[warp_id] = sum;
    }
    __syncthreads();

    // reduce warps: only the first warp does this
    if (warp_id == 0) {
        int warp_sum = 0;
        if (tid < blockSize / warpSize) {
            warp_sum = smem[tid];
        }
        #pragma unroll
        for (int offset = warpSize / 2; offset > 0; offset >>= 1) {
            warp_sum += __shfl_xor_sync(0xffffffff, warp_sum, offset);
        }
        if (lane == 0) {
            atomicAdd(output, warp_sum);
        }
    }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    constexpr int kBlock = 256;
    const int threads = kBlock;
    // Compute grid size: enough to saturate typical GPU but cap to avoid overhead
    int grid = (n + threads - 1) / threads;
    if (grid > 256) grid = 256;
    // Each block needs shared memory for one int per warp
    const int smem_size = (kBlock / 32) * sizeof(int);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<kBlock><<<grid, threads, smem_size>>>(input, output, n);
    }
}