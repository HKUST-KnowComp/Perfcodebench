#include "interface.h"
#include <cuda_runtime.h>

namespace {
// K_TILE must evenly divide TILE
constexpr int TILE      = 64;          // width of the square tile loaded into shared memory
constexpr int K_TILE    = 16;          // unroll factor along K
constexpr int THREADS   = TILE;       // 1-D thread block of 64 threads
constexpr int HM        = 4;            // height of the 4×4 sub-matrix each thread computes
constexpr int WM        = 4;            // width  of the 4×4 sub-matrix each thread computes

__global__ void matmul_tiled(const float * __restrict__ A,
                               const float * __restrict__ B,
                               float * __restrict__ C,
                               int n)
{
    __shared__ float smemA[TILE * K_TILE]; // TILE rows × K_TILE columns (transposed)
    __shared__ float smemB[TILE * K_TILE]; // K_TILE rows × TILE columns (transposed)

    const int tx = threadIdx.x;              // 0 … 63
    const int tile_row = blockIdx.y * TILE;
    const int tile_col = blockIdx.x * TILE;

    // Each thread will compute HM×WM elements of C
    // Thread (tx) owns the rectangle
    //   rows: tile_row + (tx%16)*4 … tile_row + (tx%16)*4 + 3
    //   cols: tile_col + (tx/16)*4 … tile_col + (tx/16)*4 + 3
    int out_row0 = tile_row + (tx & 15) * HM; // 4 consecutive rows
    int out_col0 = tile_col + (tx >> 4) * WM; // 4 consecutive columns

    float accum[HM][WM] = {0.0f};

    const float4 *A4 = reinterpret_cast<const float4*>(A);
    const float4 *B4 = reinterpret_cast<const float4*>(B);

    for (int k = 0; k < n; k += K_TILE)
    {
        // ---------- load A tile (TILE×K_TILE) transposed via vectorized float4 ----------
        #pragma unroll
        for (int i = 0; i < TILE; i += 16)
        {
            int rowA = tile_row + i + (tx >> 2);
            int colA = k + (tx & 3);
            float4 vec = (rowA < n && colA < n) ? A4[(rowA * n + colA) >> 2] : make_float4(0,0,0,0);
            // store transposed into smemA
            #pragma unroll
            for (int v = 0; v < 4; ++v)
                if (i + v < TILE)
                    smemA[(i + v) * K_TILE + (tx & 3) * 4 + v] = (&vec.x)[v];
        }

        // ---------- load B tile (K_TILE×TILE) transposed via vectorized float4 ----------
        #pragma unroll
        for (int i = 0; i < TILE; i += 16)
        {
            int rowB = k + (tx >> 2);
            int colB = tile_col + i + (tx & 3);
            float4 vec = (rowB < n && colB < n) ? B4[(rowB * n + colB) >> 2] : make_float4(0,0,0,0);
            #pragma unroll
            for (int v = 0; v < 4; ++v)
                if (i + v < TILE)
                    smemB[(i + v) * K_TILE + (tx & 3) * 4 + v] = (&vec.x)[v];
        }
        __syncthreads();

        // ---------- compute HM×WM micro-tile by looping over K_TILE ----------
        #pragma unroll
        for (int kk = 0; kk < K_TILE; ++kk)
        {
            float avals[HM];
            float bvals[WM];
            #pragma unroll
            for (int r = 0; r < HM; ++r)
                avals[r] = smemA[((tx & 15) * HM + r) * K_TILE + kk];
            #pragma unroll
            for (int c = 0; c < WM; ++c)
                bvals[c] = smemB[((tx >> 4) * WM + c) * K_TILE + kk];
            #pragma unroll
            for (int r = 0; r < HM; ++r)
                #pragma unroll
                for (int c = 0; c < WM; ++c)
                    accum[r][c] += avals[r] * bvals[c];
        }
        __syncthreads();
    }

    // ---------- store results back into global memory via vectorized float4 ----------
    float4 *C4 = reinterpret_cast<float4*>(C);
    #pragma unroll
    for (int r = 0; r < HM; ++r)
    {
        int row = out_row0 + r;
        if (row >= n) continue;
        float4 out;
        #pragma unroll
        for (int c = 0; c < WM; ++c)
            (&out.x)[c] = accum[r][c];
        int col = out_col0;
        if (col + 3 < n)
            C4[(row * n + col) >> 2] = out;
        else
        {
            // slow-path for right-boundary
            for (int c = 0; c < WM; ++c)
                if (col + c < n)
                    C[row * n + col + c] = (&out.x)[c];
        }
    }
}

} // anonymous namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters)
{
    const dim3 blockDim(THREADS, 1, 1);
    const dim3 gridDim((n + TILE - 1) / TILE, (n + TILE - 1) / TILE, 1);
    for (int iter = 0; iter < iters; ++iter)
        matmul_tiled<<<gridDim, blockDim>>>(a, b, c, n);
}