#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    __shared__ unsigned int s_hist[256][4];
    
    // Initialize shared histogram
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        s_hist[i][0] = 0;
        s_hist[i][1] = 0;
        s_hist[i][2] = 0;
        s_hist[i][3] = 0;
    }
    __syncthreads();

    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    int sub = threadIdx.x & 3;
    
    // Grid-stride loop accumulating into bank-conflict-free sub-histograms
    for (; idx < n; idx += stride) {
        atomicAdd(&s_hist[input[idx]][sub], 1U);
    }
    __syncthreads();

    // Reduce sub-histograms and write to global memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        unsigned int val = s_hist[i][0] + s_hist[i][1] + s_hist[i][2] + s_hist[i][3];
        if (val != 0) {
            atomicAdd(&output[i], val);
        }
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 256;
    const int grid = 1024;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
    }
}