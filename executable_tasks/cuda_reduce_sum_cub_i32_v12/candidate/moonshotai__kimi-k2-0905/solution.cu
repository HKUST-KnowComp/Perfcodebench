#include "interface.h"

#include <cuda_runtime.h>
#include <cub/cub.cuh>
#include <cstdint>

namespace {

constexpr int BLOCK_SIZE = 256;
constexpr int ITEMS_PER_THREAD = 16;

__global__ void block_reduce_kernel(const int* __restrict__ input,
                                    int* __restrict__ block_sums,
                                    int n) {
    using BlockReduce = cub::BlockReduce<int, BLOCK_SIZE>;
    __shared__ typename BlockReduce::TempStorage temp_storage;

    int sum = 0;
    int idx = (blockIdx.x * BLOCK_SIZE * ITEMS_PER_THREAD) + threadIdx.x;
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; ++i) {
        int offset = idx + i * BLOCK_SIZE;
        if (offset < n) {
            sum += input[offset];
        }
    }
    sum = BlockReduce(temp_storage).Sum(sum);
    if (threadIdx.x == 0) {
        block_sums[blockIdx.x] = sum;
    }
}

struct Buffers {
    int* dev_block_sums = nullptr;
    void* cub_temp_storage = nullptr;
    size_t cub_temp_storage_bytes = 0;
    int grid_size = 0;

    ~Buffers() {
        if (dev_block_sums) {
            cudaFree(dev_block_sums);
        }
        if (cub_temp_storage) {
            cudaFree(cub_temp_storage);
        }
    }
};

static Buffers g_bufs;

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    static bool initialized = false;
    if (!initialized) {
        g_bufs.grid_size = (n + BLOCK_SIZE * ITEMS_PER_THREAD - 1) /
                           (BLOCK_SIZE * ITEMS_PER_THREAD);
        cudaMalloc(&g_bufs.dev_block_sums, g_bufs.grid_size * sizeof(int));
        cub::DeviceReduce::Sum(nullptr, g_bufs.cub_temp_storage_bytes,
                               g_bufs.dev_block_sums, output, g_bufs.grid_size);
        cudaMalloc(&g_bufs.cub_temp_storage, g_bufs.cub_temp_storage_bytes);
        initialized = true;
    }

    for (int iter = 0; iter < iters; ++iter) {
        block_reduce_kernel<<<g_bufs.grid_size, BLOCK_SIZE>>>(
            input, g_bufs.dev_block_sums, n);
        cub::DeviceReduce::Sum(g_bufs.cub_temp_storage,
                               g_bufs.cub_temp_storage_bytes,
                               g_bufs.dev_block_sums, output, g_bufs.grid_size);
    }
}