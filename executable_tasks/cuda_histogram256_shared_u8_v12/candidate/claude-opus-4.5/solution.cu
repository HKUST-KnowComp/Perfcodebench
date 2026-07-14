#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kBlock = 256;
constexpr int kNumSubHist = 32;

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input,
                                        unsigned int* __restrict__ output,
                                        int n) {
    __shared__ unsigned int shist[kNumSubHist * 256];

    const int tid = threadIdx.x;
    const int subhist = tid % kNumSubHist;

    // Initialize shared memory
    #pragma unroll
    for (int i = tid; i < kNumSubHist * 256; i += kBlock) {
        shist[i] = 0;
    }
    __syncthreads();

    // Process 16 bytes at a time using vectorized loads
    const int n16 = n / 16;
    const uint4* input4 = reinterpret_cast<const uint4*>(input);

    for (int idx = blockIdx.x * kBlock + tid; idx < n16; idx += gridDim.x * kBlock) {
        uint4 v = input4[idx];
        
        unsigned int w0 = v.x;
        unsigned int w1 = v.y;
        unsigned int w2 = v.z;
        unsigned int w3 = v.w;

        #pragma unroll
        for (int i = 0; i < 4; ++i) {
            unsigned char b0 = w0 & 0xFF;
            unsigned char b1 = w1 & 0xFF;
            unsigned char b2 = w2 & 0xFF;
            unsigned char b3 = w3 & 0xFF;
            
            atomicAdd(&shist[subhist + b0 * kNumSubHist], 1U);
            atomicAdd(&shist[subhist + b1 * kNumSubHist], 1U);
            atomicAdd(&shist[subhist + b2 * kNumSubHist], 1U);
            atomicAdd(&shist[subhist + b3 * kNumSubHist], 1U);
            
            w0 >>= 8;
            w1 >>= 8;
            w2 >>= 8;
            w3 >>= 8;
        }
    }

    // Handle remaining bytes
    const int remainder_start = n16 * 16;
    for (int idx = remainder_start + blockIdx.x * kBlock + tid; idx < n; idx += gridDim.x * kBlock) {
        unsigned char b = input[idx];
        atomicAdd(&shist[subhist + b * kNumSubHist], 1U);
    }

    __syncthreads();

    // Reduce sub-histograms and write to global memory
    for (int bin = tid; bin < 256; bin += kBlock) {
        unsigned int sum = 0;
        #pragma unroll
        for (int s = 0; s < kNumSubHist; ++s) {
            sum += shist[s + bin * kNumSubHist];
        }
        if (sum > 0) {
            atomicAdd(&output[bin], sum);
        }
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    const int grid = 256;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
    }
}