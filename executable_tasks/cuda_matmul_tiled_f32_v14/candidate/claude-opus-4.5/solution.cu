#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int BM = 64;   // tile rows
constexpr int BN = 64;   // tile cols
constexpr int BK = 16;   // tile depth
constexpr int TM = 8;    // thread tile rows
constexpr int TN = 8;    // thread tile cols

// Threads per block: (BM/TM) * (BN/TN) = 8*8 = 64
constexpr int THREADS = (BM / TM) * (BN / TN);

__global__ __launch_bounds__(THREADS)
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

    // Thread position in the 8x8 thread grid
    const int threadRow = tx / (BN / TN);  // 0..7
    const int threadCol = tx % (BN / TN);  // 0..7

    // Global starting positions for this block
    const int aRowStart = by * BM;
    const int bColStart = bx * BN;

    // Register tile for accumulation
    float regC[TM][TN] = {{0.0f}};

    // Number of tiles along K dimension
    const int numTiles = (n + BK - 1) / BK;

    // Load first tile into buffer 0
    int bufIdx = 0;
    {
        // Load A tile: BM x BK, each thread loads multiple elements
        // Total elements: BM * BK = 64 * 16 = 1024
        // Threads: 64, so each thread loads 16 elements
        #pragma unroll
        for (int i = 0; i < (BM * BK) / THREADS; ++i) {
            int idx = tx + i * THREADS;
            int loadRow = idx / BK;
            int loadCol = idx % BK;
            int globalRow = aRowStart + loadRow;
            int globalCol = loadCol;
            float val = 0.0f;
            if (globalRow < n && globalCol < n) {
                val = A[globalRow * n + globalCol];
            }
            As[0][loadCol][loadRow] = val;
        }

        // Load B tile: BK x BN
        #pragma unroll
        for (int i = 0; i < (BK * BN) / THREADS; ++i) {
            int idx = tx + i * THREADS;
            int loadRow = idx / BN;
            int loadCol = idx % BN;
            int globalRow = loadRow;
            int globalCol = bColStart + loadCol;
            float val = 0.0f;
            if (globalRow < n && globalCol < n) {
                val = B[globalRow * n + globalCol];
            }
            Bs[0][loadRow][loadCol] = val;
        }
    }
    __syncthreads();

    for (int tile = 0; tile < numTiles; ++tile) {
        int nextTile = tile + 1;
        int nextBuf = 1 - bufIdx;

        // Prefetch next tile if available
        if (nextTile < numTiles) {
            int kStart = nextTile * BK;
            #pragma unroll
            for (int i = 0; i < (BM * BK) / THREADS; ++i) {
                int idx = tx + i * THREADS;
                int loadRow = idx / BK;
                int loadCol = idx % BK;
                int globalRow = aRowStart + loadRow;
                int globalCol = kStart + loadCol;
                float val = 0.0f;
                if (globalRow < n && globalCol < n) {
                    val = A[globalRow * n + globalCol];
                }
                As[nextBuf][loadCol][loadRow] = val;
            }

            #pragma unroll
            for (int i = 0; i < (BK * BN) / THREADS; ++i) {
                int idx = tx + i * THREADS;
                int loadRow = idx / BN;
                int loadCol = idx % BN;
                int globalRow = kStart + loadRow;
                int globalCol = bColStart + loadCol;
                float val = 0.0f;
                if (globalRow < n && globalCol < n) {
                    val = B[globalRow * n + globalCol];
                }
                Bs[nextBuf][loadRow][loadCol] = val;
            }
        }

        // Compute on current tile
        #pragma unroll
        for (int k = 0; k < BK; ++k) {
            float regA[TM];
            float regB[TN];

            #pragma unroll
            for (int i = 0; i < TM; ++i) {
                regA[i] = As[bufIdx][k][threadRow * TM + i];
            }
            #pragma unroll
            for (int j = 0; j < TN; ++j) {
                regB[j] = Bs[bufIdx][k][threadCol * TN + j];
            }

            #pragma unroll
            for (int i = 0; i < TM; ++i) {
                #pragma unroll
                for (int j = 0; j < TN; ++j) {
                    regC[i][j] += regA[i] * regB[j];
                }
            }
        }

        __syncthreads();
        bufIdx = nextBuf;
    }

    // Write results to global memory
    const int cRowStart = aRowStart + threadRow * TM;
    const int cColStart = bColStart + threadCol * TN;

    #pragma unroll
    for (int i = 0; i < TM; ++i) {
        int globalRow = cRowStart + i;
        if (globalRow < n) {
            #pragma unroll
            for (int j = 0; j < TN; ++j) {
                int globalCol = cColStart + j;
                if (globalCol < n) {
                    C[globalRow * n + globalCol] = regC[i][j];
                }
            }
        }
    }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
    dim3 block(THREADS);
    dim3 grid((n + BN - 1) / BN, (n + BM - 1) / BM);

    for (int iter = 0; iter < iters; ++iter) {
        matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
    }
}
