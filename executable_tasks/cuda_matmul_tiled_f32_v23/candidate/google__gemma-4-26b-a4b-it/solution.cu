#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    // Using a 32x32 tile size for better occupancy and memory coalescing
    // We use 16x16 threads per block to match the original grid logic if needed, 
    // but for maximum performance, we'll use a standard 32x32 or 16x16 tiling.
    // Let's implement a 32x32 tile with 16x16 threads to maintain compatibility with the grid structure if required,
    // but it's better to define the block size explicitly.
    
    const int TILE_SIZE = 32;
    __shared__ float s_a[TILE_SIZE][TILE_SIZE];
    __shared__ float s_b[TILE_SIZE][TILE_SIZE];

    const int row = blockIdx.y * blockDim.y + threadIdx.y;
    const int col = blockIdx.x * blockDim.x + threadIdx.x;

    float sum = 0.0f;

    // Loop over tiles
    for (int t = 0; t < (n + TILE_SIZE - 1) / TILE_SIZE; ++t) {
        // Load tile from A into shared memory
        if (row < n && (t * TILE_SIZE + threadIdx.x) < n) {
            s_a[threadIdx.y][threadIdx.x] = a[row * n + t * TILE_SIZE + threadIdx.x];
        } else {
            s_a[threadIdx.y][threadIdx.x] = 0.0f;
        }

        // Load tile from B into shared memory
        if (col < n && (t * TILE_SIZE + threadIdx.y) < n) {
            s_b[threadIdx.y][threadIdx.x] = b[(t * TILE_SIZE + threadIdx.y) * n + col];
        } else {
            s_b[threadIdx.y][threadIdx.x] = 0.0f;
        }

        __syncthreads();

        // Compute partial product
        #pragma unroll
        for (int k = 0; k < TILE_SIZE; ++k) {
            sum += s_a[threadIdx.y][k] * s_b[k][threadIdx.x];
        }

        __syncthreads();
    }

    if (row < n && col < n) {
        c[row * n + col] = sum;
    }
}

// A more robust version using 16x16 threads to match the original block dim
__global__ void matmul_optimized_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    const int TILE_SIZE = 16;
    __shared__ float s_a[TILE_SIZE][TILE_SIZE];
    __shared__ float s_b[TILE_SIZE][TILE_SIZE];

    const int row = blockIdx.y * blockDim.y + threadIdx.y;
    const int col = blockIdx.x * blockDim.x + threadIdx.x;

    float sum = 0.0f;

    for (int t = 0; t < (n + TILE_SIZE - 1) / TILE_SIZE; ++t) {
        // Load A tile
        if (row < n && (t * TILE_SIZE + threadIdx.x) < n) {
            s_a[threadIdx.y][threadIdx.x] = a[row * n + t * TILE_SIZE + threadIdx.x];
        } else {
            s_a[threadIdx.y][threadIdx.x] = 0.0f;
        }

        // Load B tile
        if (col < n && (t * TILE_SIZE + threadIdx.y) < n) {
            s_b[threadIdx.y][threadIdx.x] = b[(t * TILE_SIZE + threadIdx.y) * n + col];
        } else {
            s_b[threadIdx.y][threadIdx.x] = 0.0f;
        }

        __syncthreads();

        #pragma unroll
        for (int k = 0; k < TILE_SIZE; ++k) {
            sum += s_a[threadIdx.y][k] * s_b[k][threadIdx.x];
        }

        __syncthreads();
    }

    if (row < n && col < n) {
        c[row * n + col] = sum;
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  const dim3 block(16, 16);
  const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
  
  for (int iter = 0; iter < iters; ++iter) {
    matmul_optimized_kernel<<<grid, block>>>(a, b, c, n);
  }
}
