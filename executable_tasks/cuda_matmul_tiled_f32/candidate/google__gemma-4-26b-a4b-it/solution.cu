#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    // Using a 32x32 block size for better occupancy and coalescing
    // Tile size is 16x16 to fit within shared memory limits comfortably
    const int TILE_SIZE = 16;
    __shared__ float s_a[TILE_SIZE][TILE_SIZE];
    __shared__ float s_b[TILE_SIZE][TILE_SIZE];

    const int row = blockIdx.y * blockDim.y + threadIdx.y;
    const int col = blockIdx.x * blockDim.x + threadIdx.x;

    float sum = 0.0f;

    // Loop over tiles
    for (int t = 0; t < (n + TILE_SIZE - 1) / TILE_SIZE; ++t) {
        // Load tile from A into shared memory
        if (row < n && (t * TILE_SIZE + threadIdx.x) < n) {
            s_a[threadIdx.y][threadIdx.x] = a[row * n + (t * TILE_SIZE + threadIdx.x)];
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

        // Compute partial product for this tile
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

// A more aggressive version using larger blocks and register tiling
__global__ void matmul_optimized_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    const int BM = 32; // Block Memory dimension
    const int BK = 8;  // Block K dimension
    
    // Using a 32x8 block to facilitate better memory access patterns
    // However, for simplicity and stability in a single kernel, we use a standard 32x32 block
    // with a 16x16 tile approach optimized for the hardware.
    
    const int row = blockIdx.y * blockDim.y + threadIdx.y;
    const int col = blockIdx.x * blockDim.x + threadIdx.x;

    float sum = 0.0f;
    const int tile_size = 32;

    // We use a 32x32 block and process tiles of 32
    // To keep it simple and highly performant, we use a standard tiling pattern
    // but ensure we use __restrict__ and avoid bank conflicts.
    
    // Re-implementing a robust tiled version
    // This version uses 32x32 threads and 32x32 tiles
    // Note: 32*32*4 bytes = 4KB per shared array. Two arrays = 8KB. Fits in 48KB+ L1/Shared.
    
    __shared__ float s_a[32][32];
    __shared__ float s_b[32][32];

    for (int m = 0; m < (n + 31) / 32; ++m) {
        int a_row = row;
        int a_col = m * 32 + threadIdx.x;
        int b_row = m * 32 + threadIdx.y;
        int b_col = col;

        if (a_row < n && a_col < n) s_a[threadIdx.y][threadIdx.x] = a[a_row * n + a_col];
        else s_a[threadIdx.y][threadIdx.x] = 0.0f;

        if (b_row < n && b_col < n) s_b[threadIdx.y][threadIdx.x] = b[b_row * n + b_col];
        else s_b[threadIdx.y][threadIdx.x] = 0.0f;

        __syncthreads();

        #pragma unroll
        for (int k = 0; k < 32; ++k) {
            sum += s_a[threadIdx.y][k] * s_b[k][threadIdx.x];
        }
        __syncthreads();
    }

    if (row < n && col < n) {
        c[row * n + col] = sum;
    }
}

} // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    // 32x32 threads per block is a sweet spot for many architectures
    const dim3 block(32, 32);
    const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);

    for (int iter = 0; iter < iters; ++iter) {
        matmul_optimized_kernel<<<grid, block>>>(a, b, c, n);
    }
}