#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void matmul_kernel(const float* __restrict__ A,
                              const float* __restrict__ B,
                              float*       __restrict__ C,
                              int n)
{
    const int BM = 128;               // rows of tile in A & C
    const int BN = 8;                 // cols of tile in B & C
    const int BK = 8;                 // reduction dimension
    const int TM = 4;                 // rows each thread computes
    const int TN = 1;                 // cols each thread computes

    __shared__ float As[BM * BK];     // tile A (row-major)
    __shared__ float Bs[BK * BN];     // tile B (row-major)

    float accum[TM * TN] = {0.0f};

    const int tid = threadIdx.x;
    const int rowC = blockIdx.y * BM + tid / (BN / TN) * TM;
    const int colC = blockIdx.x * BN + tid % (BN / TN) * TN;

    const int strideA = gridDim.y * BM;
    const int strideB = gridDim.x * BN;

    for (int k0 = 0; k0 < n; k0 += BK)
    {
        // Load tile A into shared memory
        #pragma unroll
        for (int i = 0; i < BM; i += 128)
        {
            int r = blockIdx.y * BM + i + tid / 8;
            int c = k0 + tid % 8;
            As[(i + tid / 8) * BK + tid % 8] =
                (r < n && c < n) ? A[r * n + c] : 0.0f;
        }

        // Load tile B into shared memory
        #pragma unroll
        for (int j = 0; j < BN; j += 8)
        {
            int r = k0 + tid / 8;
            int c = blockIdx.x * BN + j + tid % 8;
            Bs[(tid / 8) * BN + j + tid % 8] =
                (r < n && c < n) ? B[r * n + c] : 0.0f;
        }
        __syncthreads();

        // Compute
        #pragma unroll
        for (int k = 0; k < BK; ++k)
        {
            #pragma unroll
            for (int m = 0; m < TM; ++m)
            {
                float a = As[(tid / 2) * TM + m + 0 * BM] * BK + k;
                #pragma unroll
                for (int n = 0; n < TN; ++n)
                {
                    float b = Bs[k * BN + tid % 2 * TN + n];
                    accum[m * TN + n] += a * b;
                }
            }
        }
        __syncthreads();
    }

    // Store C
    #pragma unroll
    for (int m = 0; m < TM; ++m)
    {
        int r = rowC + m;
        #pragma unroll
        for (int n = 0; n < TN; ++n)
        {
            int c = colC + n;
            if (r < n && c < n)
                C[r * n + c] = accum[m * TN + n];
        }
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters)
{
    const int BM = 128;
    const int BN = 8;
    dim3 block(128);
    dim3 grid((n + BN - 1) / BN, (n + BM - 1) / BM);
    for (int iter = 0; iter < iters; ++iter)
        matmul_kernel<<<grid, block>>>(a, b, c, n);
}