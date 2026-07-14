#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_sum_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    // Shared memory for block-level reduction
    extern __shared__ int sdata[];

    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    int sum = 0;
    // Grid-stride loop to handle n > grid * blockDim
    while (i < n) {
        sum += input[i];
        i += stride;
    }

    sdata[tid] = sum;
    __syncthreads();

    // In-block reduction using warp shuffles
    for (int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    // Only one thread per block writes to global memory via atomicAdd
    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Tuning parameters
    const int kBlock = 256;
    // Use a smaller grid to avoid excessive atomic contention and improve cache locality
    const int kGrid = 256;
    const size_t shared_mem_size = kBlock * sizeof(int);

    for (int iter = 0; iter < iters; ++iter) {
        // Reset output to 0
        cudaMemset(output, 0, sizeof(int));
        
        // Launch reduction kernel
        reduce_sum_kernel<<<kGrid, kBlock, shared_mem_size>>>(input, output, n);
    }
}