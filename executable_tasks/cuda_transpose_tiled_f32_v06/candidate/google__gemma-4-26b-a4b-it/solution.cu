#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
// Padding to avoid bank conflicts in shared memory
constexpr int kTileDimPadded = kTileDim + 1;

__global__ void transpose_tiled_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[kTileDimPadded][kTileDimPadded];

    // Calculate global indices
    int x = blockIdx.x * kTileDim + threadIdx.x;
    int y = blockIdx.y * kTileDim + threadIdx.y;

    // Load from input into shared memory (coalesced read)
    if (x < width && y < height) {
        tile[threadIdx.y][threadIdx.x] = input[y * width + x];
    } else {
        tile[threadIdx.y][threadIdx.x] = 0.0f;
    }

    __syncthreads();

    // Calculate new indices for transposed output
    // The output coordinates are (y, x) relative to the original matrix
    // In the transposed matrix, the new x is the old y, and new y is the old x
    int out_x = blockIdx.y * kTileDim + threadIdx.x;
    int out_y = blockIdx.x * kTileDim + threadIdx.y;

    // Write from shared memory to output (coalesced write)
    // We read from tile[threadIdx.x][threadIdx.y] to ensure the write to output is coalesced
    if (out_x < height && out_y < width) {
        output[out_y * height + out_x] = tile[threadIdx.x][threadIdx.y];
    }
}

// A more efficient version using a single thread to handle multiple elements
__global__ void transpose_optimized_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[kTileDim][kTileDimPadded];

    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int col = blockIdx.x * kTileDim + tx;
    int row = blockIdx.y * kTileDim + ty;

    if (col < width && row < height) {
        tile[ty][tx] = input[row * width + col];
    }

    __syncthreads();

    // Transpose indices: blockIdx.x and blockIdx.y are swapped
    int new_col = blockIdx.y * kTileDim + tx;
    int new_row = blockIdx.x * kTileDim + ty;

    if (new_col < height && new_row < width) {
        output[new_row * height + new_col] = tile[tx][ty];
    }
}

// Final high-performance kernel using 16x16 threads to cover 32x32 tiles
__global__ void transpose_final_kernel(const float* __restrict__ input, float* __restrict__ output, int width, int height) {
    __shared__ float tile[kTileDim][kTileDimPadded];

    int tx = threadIdx.x;
    int ty = threadIdx.y;
    
    // Each thread handles 2 elements to fill a 32x32 tile with 16x16 threads
    int x = blockIdx.x * kTileDim + tx + (ty >= 16 ? 16 : 0);
    int y = blockIdx.y * kTileDim + ty;

    // Load first element
    if (x < width && y < height) {
        tile[ty][tx] = input[y * width + x];
        // Load second element if within bounds
        if (x + 16 < width && y < height) {
            tile[ty][tx + 16] = input[y * width + (x + 16)];
        } else {
            tile[ty][tx + 16] = 0.0f;
        }
    } else {
        tile[ty][tx] = 0.0f;
        tile[ty][tx + 16] = 0.0f;
    }

    __syncthreads();

    // Transpose write
    int out_x = blockIdx.y * kTileDim + tx + (ty >= 16 ? 16 : 0);
    int out_y = blockIdx.x * kTileDim + ty;

    if (out_x < height && out_y < width) {
        output[out_y * height + out_x] = tile[tx][ty];
        if (out_x + 16 < height && out_y < width) {
            output[out_y * height + (out_x + 16)] = tile[tx + 16][ty];
        }
    }
}

} // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
    // Using a standard 32x32 tile with 32x8 threads for simplicity and high occupancy
    // but optimized for coalesced access.
    dim3 block(32, 8);
    dim3 grid((width + 31) / 32, (height + 31) / 32);

    // Re-implementing a robust tiled kernel that is highly efficient
    auto kernel = [] __global__ (const float* in, float* out, int w, int h) {
        __shared__ float smem[32][33];
        int tx = threadIdx.x;
        int ty = threadIdx.y;
        int x = blockIdx.x * 32 + tx;
        int y = blockIdx.y * 32 + ty;

        // Load into shared memory
        // Each thread in 32x8 block loads 4 elements to fill 32x32
        for (int i = 0; i < 4; ++i) {
            int cur_x = x + (i / 2) * 16;
            int cur_y = y + (i % 2) * 4;
            if (cur_x < w && cur_y < h) {
                smem[ty + (i % 2) * 4][tx + (i / 2) * 16] = in[cur_y * w + cur_x];
            } else {
                smem[ty + (i % 2) * 4][tx + (i / 2) * 16] = 0.0f;
            }
        }
        __syncthreads();

        // Write out
        int out_x = blockIdx.y * 32 + tx;
        int out_y = blockIdx.x * 32 + ty;
        for (int i = 0; i < 4; ++i) {
            int cur_out_x = out_x + (i / 2) * 16;
            int cur_out_y = out_y + (i % 2) * 4;
            if (cur_out_x < h && cur_out_y < w) {
                out[cur_out_y * h + cur_out_x] = smem[tx + (i / 2) * 16][ty + (i % 2) * 4];
            }
        }
    };

    // Actually, the most reliable high-perf way is a simple 32x32 tile with 32x32 threads
    // but we must handle the grid/block dimensions correctly.
    // Let's use a standard 32x32 block for maximum simplicity and correctness.
    auto kernel_std = [] __global__ (const float* in, float* out, int w, int h) {
        __shared__ float smem[32][33];
        int tx = threadIdx.x;
        int ty = threadIdx.y;
        int x = blockIdx.x * 32 + tx;
        int y = blockIdx.y * 32 + ty;

        if (x < w && y < h) smem[ty][tx] = in[y * w + x];
        else smem[ty][tx] = 0.0f;

        __syncthreads();

        int ox = blockIdx.y * 32 + tx;
        int oy = blockIdx.x * 32 + ty;
        if (ox < h && oy < w) out[oy * h + ox] = smem[tx][ty];
    };

    // Re-calculating grid for 32x32 blocks
    dim3 block32(32, 32);
    dim3 grid32((width + 31) / 32, (height + 31) / 32);

    for (int iter = 0; iter < iters; ++iter) {
        kernel_std<<<grid32, block32>>>(input, output, width, height);
    }
}
```