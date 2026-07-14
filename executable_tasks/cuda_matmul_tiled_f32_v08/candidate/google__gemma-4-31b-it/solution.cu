#include "interface.h"
#include <cuda_runtime.h>

namespace {

// TILE_SIZE is the dimension of the shared memory tile
const int TILE_SIZE = 32;
// BLOCK_SIZE is the number of threads per dimension
const int BLOCK_SIZE = 16;

__global__ void matmul_tiled_kernel(const float* __restrict__ a, const float* __restrict__ b, float* __restrict__ c, int n) {
    // Shared memory for tiles of A and B
    __shared__ float s_a[TILE_SIZE][TILE_SIZE];
    __shared__ float s_b[TILE_SIZE][TILE_SIZE];

    int row = blockIdx.y * BLOCK_SIZE + threadIdx.y;
    int col = blockIdx.x * BLOCK_SIZE + threadIdx.x;
    
    float acc = 0.0f;

    // Loop over tiles
    for (int t = 0; t < (n + TILE_SIZE - 1) / TILE_SIZE; ++t) {
        // Collaborative load of tiles into shared memory
        // Each thread in the 16x16 block loads 4 elements to fill the 32x32 tile
        #pragma unroll
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                int load_row = row + i * BLOCK_SIZE;
                int load_col = col + j * BLOCK_SIZE;
                
                if (load_row < n && (t * TILE_SIZE + load_col) < n) {
                    s_a[threadIdx.y + i * BLOCK_SIZE][threadIdx.x + j * BLOCK_SIZE] = a[load_row * n + t * TILE_SIZE + load_col];
                } else {
                    s_a[threadIdx.y + i * BLOCK_SIZE][threadIdx.x + j * BLOCK_SIZE] = 0.0f;
                }

                if (load_row < n && (t * TILE_SIZE + load_col) < n) {
                    s_b[threadIdx.y + i * BLOCK_SIZE][threadIdx.x + j * BLOCK_SIZE] = b[(t * TILE_SIZE + load_row) * n + load_col];
                } else {
                    s_b[threadIdx.y + i * BLOCK_SIZE][threadIdx.x + j * BLOCK_SIZE] = 0.0f;
                }
            }
        }
        __syncthreads();

        // Compute partial product for the tile
        #pragma unroll
        for (int k = 0; k < TILE_SIZE; ++k) {
            acc += s_a[threadIdx.y][k] * s_b[k][threadIdx.x];
        }
        __syncthreads();
    }

    if (row < n && col < n) {
        c[row * n + col] = acc;
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    const dim3 block(BLOCK_SIZE, BLOCK_SIZE);
    const dim3 grid((n + BLOCK_SIZE - 1) / BLOCK_SIZE, (n + BLOCK_SIZE - 1) / BLOCK_SIZE);

    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
    cudaDeviceSynchronize();
}
