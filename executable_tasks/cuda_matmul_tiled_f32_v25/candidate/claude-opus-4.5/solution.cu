#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int BM = 64;   // tile rows
constexpr int BN = 64;   // tile cols
constexpr int BK = 16;   // tile depth
constexpr int TM = 8;    // thread tile rows
constexpr int TN = 8;    // thread tile cols

// Threads per block: (BM/TM) * (BN/TN) = 8*8 = 64
constexpr int BLOCK_SIZE = (BM / TM) * (BN / TN);

__global__ __launch_bounds__(BLOCK_SIZE)
void matmul_tiled_kernel(const float* __restrict__ A,
                         const float* __restrict__ B,
                         float* __restrict__ C,
                         int n) {
    // Shared memory with padding to avoid bank conflicts
    __shared__ float As[2][BK][BM + 4];
    __shared__ float Bs[2][BK][BN + 4];

    const int bx = blockIdx.x;
    const int by = blockIdx.y;
    const int tx = threadIdx.x;

    // Thread position within the thread-tile grid
    const int threadRow = tx / (BN / TN);  // 0..7
    const int threadCol = tx % (BN / TN);  // 0..7

    // Global starting positions for this block
    const int aRowStart = by * BM;
    const int bColStart = bx * BN;

    // Register tile for accumulation
    float regC[TM][TN] = {0.0f};

    // Registers for A and B fragments
    float regA[TM];
    float regB[TN];

    // Number of tiles along K dimension
    const int numTiles = (n + BK - 1) / BK;

    // Precompute load indices for A and B
    // Each thread loads multiple elements to fill shared memory
    // A tile: BM x BK = 64 x 16 = 1024 elements, 64 threads -> 16 elements each
    // B tile: BK x BN = 16 x 64 = 1024 elements, 64 threads -> 16 elements each

    const int loadARow = tx / 4;           // 0..15 (but we need 64 rows)
    const int loadACol = (tx % 4) * 4;     // 0, 4, 8, 12

    const int loadBRow = tx / 16;          // 0..3 (but we need 16 rows)
    const int loadBCol = (tx % 16) * 4;    // 0, 4, 8, ..., 60

    // Load first tile into buffer 0
    int writeBuffer = 0;
    int kStart = 0;

    // Load A tile (64 rows x 16 cols)
    // Each thread loads 4 elements per iteration, need 4 iterations for rows
    #pragma unroll
    for (int i = 0; i < 4; ++i) {
        int aRow = aRowStart + loadARow + i * 16;
        int aCol = kStart + loadACol;
        if (aRow < n && aCol + 3 < n) {
            float4 tmp = *reinterpret_cast<const float4*>(&A[aRow * n + aCol]);
            As[writeBuffer][loadACol + 0][loadARow + i * 16] = tmp.x;
            As[writeBuffer][loadACol + 1][loadARow + i * 16] = tmp.y;
            As[writeBuffer][loadACol + 2][loadARow + i * 16] = tmp.z;
            As[writeBuffer][loadACol + 3][loadARow + i * 16] = tmp.w;
        } else {
            #pragma unroll
            for (int j = 0; j < 4; ++j) {
                int r = aRow;
                int c = aCol + j;
                As[writeBuffer][loadACol + j][loadARow + i * 16] = 
                    (r < n && c < n) ? A[r * n + c] : 0.0f;
            }
        }
    }

    // Load B tile (16 rows x 64 cols)
    // Each thread loads 4 elements per iteration, need 4 iterations for rows
    #pragma unroll
    for (int i = 0; i < 4; ++i) {
        int bRow = kStart + loadBRow + i * 4;
        int bCol = bColStart + loadBCol;
        if (bRow < n && bCol + 3 < n) {
            float4 tmp = *reinterpret_cast<const float4*>(&B[bRow * n + bCol]);
            Bs[writeBuffer][loadBRow + i * 4][loadBCol + 0] = tmp.x;
            Bs[writeBuffer][loadBRow + i * 4][loadBCol + 1] = tmp.y;
            Bs[writeBuffer][loadBRow + i * 4][loadBCol + 2] = tmp.z;
            Bs[writeBuffer][loadBRow + i * 4][loadBCol + 3] = tmp.w;
        } else {
            #pragma unroll
            for (int j = 0; j < 4; ++j) {
                int r = bRow;
                int c = bCol + j;
                Bs[writeBuffer][loadBRow + i * 4][loadBCol + j] = 
                    (r < n && c < n) ? B[r * n + c] : 0.0f;
            }
        }
    }

    __syncthreads();

    // Main loop over tiles
    for (int tile = 0; tile < numTiles; ++tile) {
        int readBuffer = writeBuffer;
        writeBuffer = 1 - writeBuffer;

        // Prefetch next tile if not last
        if (tile + 1 < numTiles) {
            kStart = (tile + 1) * BK;

            #pragma unroll
            for (int i = 0; i < 4; ++i) {
                int aRow = aRowStart + loadARow + i * 16;
                int aCol = kStart + loadACol;
                if (aRow < n && aCol + 3 < n) {
                    float4 tmp = *reinterpret_cast<const float4*>(&A[aRow * n + aCol]);
                    As[writeBuffer][loadACol + 0][loadARow + i * 16] = tmp.x;
                    As[writeBuffer][loadACol + 1][loadARow + i * 16] = tmp.y;
                    As[writeBuffer][loadACol + 2][loadARow + i * 16] = tmp.z;
                    As[writeBuffer][loadACol + 3][loadARow + i * 16] = tmp.w;
                } else {
                    #pragma unroll
                    for (int j = 0; j < 4; ++j) {
                        int r = aRow;
                        int c = aCol + j;
                        As[writeBuffer][loadACol + j][loadARow + i * 16] = 
                            (r < n && c < n) ? A[r * n + c] : 0.0f;
                    }
                }
            }

            #pragma unroll
            for (int i = 0; i < 4; ++i) {
                int bRow = kStart + loadBRow + i * 4;
                int bCol = bColStart + loadBCol;
                if (bRow < n && bCol + 3 < n) {
                    float4 tmp = *reinterpret_cast<const float4*>(&B[bRow * n + bCol]);
                    Bs[writeBuffer][loadBRow + i * 4][loadBCol + 0] = tmp.x;
                    Bs[writeBuffer][loadBRow + i * 4][loadBCol + 1] = tmp.y;
                    Bs[writeBuffer][loadBRow + i * 4][loadBCol + 2] = tmp.z;
                    Bs[writeBuffer][loadBRow + i * 4][loadBCol + 3] = tmp.w;
                } else {
                    #pragma unroll
                    for (int j = 0; j < 4; ++j) {
                        int r = bRow;
                        int c = bCol + j;
                        Bs[writeBuffer][loadBRow + i * 4][loadBCol + j] = 
                            (r < n && c < n) ? B[r * n + c] : 0.0f;
                    }
                }
            }
        }

        // Compute on current tile
        #pragma unroll
        for (int k = 0; k < BK; ++k) {
            // Load A fragment
            #pragma unroll
            for (int i = 0; i < TM; ++i) {
                regA[i] = As[readBuffer][k][threadRow * TM + i];
            }
            // Load B fragment
            #pragma unroll
            for (int j = 0; j < TN; ++j) {
                regB[j] = Bs[readBuffer][k][threadCol * TN + j];
            }
            // Outer product
            #pragma unroll
            for (int i = 0; i < TM; ++i) {
                #pragma unroll
                for (int j = 0; j < TN; ++j) {
                    regC[i][j] += regA[i] * regB[j];
                }
            }
        }

        __syncthreads();
    }

    // Write results to global memory
    const int cRowStart = aRowStart + threadRow * TM;
    const int cColStart = bColStart + threadCol * TN;

    #pragma unroll
    for (int i = 0; i < TM; ++i) {
        int cRow = cRowStart + i;
        if (cRow < n) {
            #pragma unroll
            for (int j = 0; j < TN; j += 4) {
                int cCol = cColStart + j;
                if (cCol + 3 < n) {
                    float4 out;
                    out.x = regC[i][j + 0];
                    out.y = regC[i][j + 1];
                    out.z = regC[i][j + 2];
                    out.w = regC[i][j + 3];
                    *reinterpret_cast<float4*>(&C[cRow * n + cCol]) = out;
                } else {
                    #pragma unroll
                    for (int jj = 0; jj < 4; ++jj) {
                        if (cCol + jj < n) {
                            C[cRow * n + cCol + jj] = regC[i][j + jj];
                        }
                    }
                }
            }
        }
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    dim3 block(BLOCK_SIZE);
    dim3 grid((n + BN - 1) / BN, (n + BM - 1) / BM);
    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}
