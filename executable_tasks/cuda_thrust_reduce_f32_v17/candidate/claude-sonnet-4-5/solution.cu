#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_SIZE = 256;
constexpr int WARP_SIZE = 32;

__inline__ __device__ float warp_reduce_sum(float val) {
    for (int offset = WARP_SIZE / 2; offset > 0; offset /= 2) {
        val += __shfl_down_sync(0xffffffff, val, offset);
    }
    return val;
}

__global__ void parallel_reduce_kernel(const float* input, float* output, int n) {
    __shared__ float shared[BLOCK_SIZE / WARP_SIZE];
    
    int tid = threadIdx.x;
    int lane = tid % WARP_SIZE;
    int warp_id = tid / WARP_SIZE;
    int grid_size = blockDim.x * gridDim.x;
    int idx = blockIdx.x * blockDim.x + tid;
    
    // Grid-stride loop for coalesced memory access
    float sum = 0.0f;
    for (int i = idx; i < n; i += grid_size) {
        sum += input[i];
    }
    
    // Warp-level reduction
    sum = warp_reduce_sum(sum);
    
    // Write warp results to shared memory
    if (lane == 0) {
        shared[warp_id] = sum;
    }
    __syncthreads();
    
    // Final reduction in first warp
    if (warp_id == 0) {
        sum = (tid < BLOCK_SIZE / WARP_SIZE) ? shared[lane] : 0.0f;
        sum = warp_reduce_sum(sum);
        
        if (tid == 0) {
            atomicAdd(output, sum);
        }
    }
}

}  // namespace

float reduce_f32(const float* input, int n, int iters) {
    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));
    
    int num_blocks = min((n + BLOCK_SIZE - 1) / BLOCK_SIZE, 1024);
    
    float actual = 0.0f;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(d_out, 0, sizeof(float));
        parallel_reduce_kernel<<<num_blocks, BLOCK_SIZE>>>(input, d_out, n);
    }
    
    cudaMemcpy(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    
    return actual;
}