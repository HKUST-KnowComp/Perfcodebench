#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void block_reduce_kernel(const float* input, float* block_sums, int n) {
    extern __shared__ float smem[];
    const int tid = threadIdx.x;
    const int bid = blockIdx.x;
    const int block_size = blockDim.x;

    // Grid-stride loop over elements assigned to this block
    float sum = 0.0f;
    for (int i = bid * block_size + tid; i < n; i += gridDim.x * block_size) {
        sum += input[i];
    }

    // Warp-level reduction within each warp (warp size 32)
    const int warp_id = tid / 32;
    const int lane = tid % 32;
    for (int offset = 16; offset > 0; offset >>= 1) {
        sum += __shfl_down_sync(0xffffffff, sum, offset);
    }

    // Write warp sum to shared memory
    if (lane == 0) {
        smem[warp_id] = sum;
    }
    __syncthreads();

    // Final reduction of warp sums (assuming block_size <= 1024, so at most 32 warps)
    if (warp_id == 0) {
        float warp_sum = (tid < (block_size + 31) / 32) ? smem[tid] : 0.0f;
        for (int offset = 16; offset > 0; offset >>= 1) {
            warp_sum += __shfl_down_sync(0xffffffff, warp_sum, offset);
        }
        if (tid == 0) {
            block_sums[bid] = warp_sum;
        }
    }
}

__global__ void final_reduce_kernel(const float* block_sums, float* result, int num_blocks) {
    extern __shared__ float smem[];
    const int tid = threadIdx.x;

    // Load block sums into shared memory
    float sum = 0.0f;
    for (int i = tid; i < num_blocks; i += blockDim.x) {
        sum += block_sums[i];
    }
    smem[tid] = sum;
    __syncthreads();

    // Reduction in shared memory (power-of-two sizes handled via padding)
    for (int stride = blockDim.x / 2; stride > 0; stride >>= 1) {
        if (tid < stride) {
            smem[tid] += smem[tid + stride];
        }
        __syncthreads();
    }

    if (tid == 0) {
        *result = smem[0];
    }
}

} // anonymous namespace

float reduce_f32(const float* input, int n, int iters) {
    const int block_size = 256;
    const int max_blocks = (n + block_size - 1) / block_size;
    // Limit grid size to a reasonable number (e.g., 1024 blocks) to avoid excessive kernel overhead.
    const int grid_size = min(max_blocks, 1024);

    // Device memory for block partial sums and final result
    float* d_block_sums = nullptr;
    float* d_result = nullptr;
    cudaMalloc(&d_block_sums, grid_size * sizeof(float));
    cudaMalloc(&d_result, sizeof(float));

    // Shared memory size for block reduction: one float per warp (max 32 warps for block_size=256)
    const size_t smem_bytes = ((block_size + 31) / 32) * sizeof(float);

    for (int iter = 0; iter < iters; ++iter) {
        // First kernel: reduce input to block sums
        block_reduce_kernel<<<grid_size, block_size, smem_bytes>>>(input, d_block_sums, n);

        // Second kernel: reduce block sums to a single value
        // Use up to 256 threads for final reduction
        int final_block_size = min(256, grid_size);
        // Ensure final_block_size is a power of two for the shared memory reduction loop
        int final_pow2 = 1;
        while (final_pow2 < final_block_size) final_pow2 <<= 1;
        final_block_size = final_pow2;
        const size_t final_smem = final_block_size * sizeof(float);
        final_reduce_kernel<<<1, final_block_size, final_smem>>>(d_block_sums, d_result, grid_size);
    }

    float host_result;
    cudaMemcpy(&host_result, d_result, sizeof(float), cudaMemcpyDeviceToHost);

    cudaFree(d_block_sums);
    cudaFree(d_result);

    return host_result;
}
