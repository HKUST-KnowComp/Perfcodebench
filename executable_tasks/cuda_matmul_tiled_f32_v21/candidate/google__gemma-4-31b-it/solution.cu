#include "interface.h"
#include <cuda_runtime.h>

namespace {

#define TILE_SIZE 32
#define BLOCK_SIZE 16

__global__ void matmul_optimized_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    // Each thread computes a 2x2 block of C to increase register reuse
    const int row_start = (blockIdx.y * BLOCK_SIZE) + threadIdx.y;
    const int col_start = (blockIdx.x * BLOCK_SIZE) + threadIdx.x;

    __shared__ float s_a[BLOCK_SIZE][TILE_SIZE];
    __shared__ float s_b[TILE_SIZE][BLOCK_SIZE];

    float sum00 = 0.0f, sum01 = 0.0f, sum10 = 0.0f, sum11 = 0.0f;

    for (int tile = 0; tile < (n + TILE_SIZE - 1) / TILE_SIZE; ++tile) {
        // Load tiles into shared memory
        // Each thread loads 2 elements for A and 2 for B to fill the TILE_SIZE x BLOCK_SIZE buffers
        for (int i = 0; i < 2; ++i) {
            int load_row = row_start + i;
            int load_col = tile * TILE_SIZE + threadIdx.x;
            if (load_row < n && load_col < n) {
                s_a[threadIdx.y + (i * BLOCK_SIZE)][load_col] = a[load_row * n + load_col];
            } else {
                s_a[threadIdx.y + (i * BLOCK_SIZE)][tile * TILE_SIZE + threadIdx.x] = 0.0f;
            }

            int load_row_b = tile * TILE_SIZE + threadIdx.y + (i * BLOCK_SIZE);
            int load_col_b = col_start + i;
            if (load_row_b < n && load_col_b < n) {
                s_b[load_row_b][threadIdx.x + (i * BLOCK_SIZE)] = b[load_row_b * n + load_col_b];
            } else {
                s_b[tile * TILE_SIZE + threadIdx.y + (i * BLOCK_SIZE)][threadIdx.x + (i * BLOCK_SIZE)] = 0.0f;
            }
        }

        __syncthreads();

        // Compute partial products
        #pragma unroll
        for (int k = 0; k < TILE_SIZE; ++k) {
            float a0 = s_a[threadIdx.y][k];
            float a1 = s_a[threadIdx.y + BLOCK_SIZE][k];
            float b0 = s_b[k][threadIdx.x];
            float b1 = s_b[k][threadIdx.x + BLOCK_SIZE];

            sum00 += a0 * b0;
            sum01 += a0 * b1;
            sum10 += a1 * b0;
            sum11 += a1 * b1;
        }

        __syncthreads();
    }

    // Write results back to global memory
    if (row_start < n && col_start < n) 
        c[row_start * n + col_start] = sum00;
    if (row_start < n && col_start + 1 < n) 
        c[row_start * n + col_start + 1] = sum01;
    if (row_start + 1 < n && col_start < n) 
        c[(row_start + 1) * n + col_start] = sum10;
    if (row_start + 1 < n && col_start + 1 < n) 
        c[(row_start + 1) * n + col_start + 1] = sum11;
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    // Grid and block dimensions based on the 2x2 output per thread strategy
    // Each block handles 32x32 output elements
    const dim3 block(BLOCK_SIZE, BLOCK_SIZE);
    const dim3 grid((n + 31) / 32, (n + 31) / 32);

    for (int iter = 0; iter < iters; ++iter) {
        matmul_optimized_kernel<<<grid, block>>>(a, b, c, n);
    }
    cudaDeviceSynchronize();
}