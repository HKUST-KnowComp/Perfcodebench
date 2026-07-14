#include "interface.h"
#include <cuda_runtime.h>
#include <cstdint>

namespace {

// 256-bin histogram, 256 threads per block
constexpr int BINS = 256;
constexpr int BLOCK = 256;
constexpr int WARPS = BLOCK / 32;

__global__ void histo_shared_kernel(const uint8_t* __restrict__ in,
                                     unsigned int*  __restrict__ out,
                                     int n,
                                     unsigned int*  __restrict__ block_sums) {
    // thread-private counters (256 bytes per thread)
    unsigned int priv[BINS / 32];  // 8 uints per thread
#pragma unroll
    for (int i = 0; i < BINS / 32; ++i) priv[i] = 0;

    const int tid = threadIdx.x;
    const int gtid = blockIdx.x * blockDim.x + tid;

    // process data in grid-stride fashion
    for (int i = gtid; i < n; i += gridDim.x * blockDim.x) {
        uint8_t b = in[i];
        priv[b >> 5] += 1U << ((b & 31) * 2);  // 2-bit counter per bin
        // periodic flush to avoid overflow (every 64 increments per counter)
        if ((priv[b >> 5] >> ((b & 31) * 2)) & 0x3) == 0x3) {
            // overflow imminent: flush this counter
            priv[b >> 5] &= ~(0x3U << ((b & 31) * 2));
            __shared__ unsigned int smem[BINS];
            if (tid < BINS) smem[tid] = 0;
            __syncthreads();
            atomicAdd(&smem[b], 64);
            __syncthreads();
            // fold smem into priv
            for (int j = 0; j < BINS / 32; ++j) {
                int base = j * 32;
#pragma unroll
                for (int k = 0; k < 32; ++k) {
                    int bin = base + k;
                    unsigned int v = smem[bin];
                    if (v) priv[j] += v << (k * 2);
                }
            }
        }
    }

    // final reduction of private counters into shared memory histogram
    __shared__ unsigned int histo[BINS];
    if (tid < BINS) histo[tid] = 0;
    __syncthreads();

    // each thread atomics its 8 counters into smem
    int base = 0;
#pragma unroll
    for (int i = 0; i < BINS / 32; ++i) {
        unsigned int packed = priv[i];
#pragma unroll
        for (int k = 0; k < 32; ++k) {
            unsigned int cnt = (packed >> (k * 2)) & 0x3;
            if (cnt) atomicAdd(&histo[base + k], cnt);
        }
        base += 32;
    }
    __syncthreads();

    // warp-level reduction of histo into global using 256 shuffles
    const int warp_id = tid >> 5;
    const int lane    = tid & 31;
    for (int bin = warp_id; bin < BINS; bin += WARPS) {
        unsigned int val = histo[bin];
#pragma unroll
        for (int offset = 16; offset >= 1; offset /= 2)
            val += __shfl_down_sync(0xffffffff, val, offset);
        if (lane == 0) atomicAdd(block_sums + bin * gridDim.x + blockIdx.x, val);
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input,
                     unsigned int* output,
                     int n,
                     int iters) {
    constexpr int grid = 1024;  // same block count as baseline
    size_t block_sums_bytes = BINS * grid * sizeof(unsigned int);
    unsigned int* d_block_sums;
    cudaMalloc(&d_block_sums, block_sums_bytes);
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(d_block_sums, 0, block_sums_bytes);
        histo_shared_kernel<<<grid, BLOCK>>>(
            reinterpret_cast<const uint8_t*>(input),
            output,
            n,
            d_block_sums);
        // reduce per-block partials into final output
        void* args[] = { &d_block_sums, &output, &grid };
        cudaLaunchKernel(
            reinterpret_cast<void*>(&reduce_blocks<256>),
            dim3(1), dim3(256), args, 0, nullptr);
    }
    cudaFree(d_block_sums);
}

// final reduction kernel
template<int Threads>
__global__ void reduce_blocks(const unsigned int* __restrict__ block_sums,
                              unsigned int* __restrict__ out,
                              int grid) {
    const int tid = threadIdx.x;
    for (int bin = tid; bin < 256; bin += Threads) {
        unsigned int sum = 0;
        for (int b = 0; b < grid; ++b)
            sum += block_sums[bin * grid + b];
        out[bin] = sum;
    }
}