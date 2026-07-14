#include "interface.h"
#include <cuda_runtime.h>

namespace {

static constexpr int BLOCK_THREADS = 256;
static constexpr int WARP  = 32;
template <int W> __device__ __forceinline__
int warp_scan_inclusive(int val, int* shmem, int lane) {
    shmem[size_t(threadIdx.x)] = val;
    if (W >= 2) shmem[size_t(threadIdx.x)] = val = (lane>=1)?shmem[threadIdx.x-1]+val:val;
    if (W >= 4) shmem[size_t(threadIdx.x)] = val = (lane>=2)?shmem[threadIdx.x-2]+val:val;
    if (W >= 8) shmem[size_t(threadIdx.x)] = val = (lane>=4)?shmem[threadIdx.x-4]+val:val;
    if (W >=16) shmem[size_t(threadIdx.x)] = val = (lane>=8)?shmem[threadIdx.x-8]+val:val;
    if (W >=32) shmem[size_t(threadIdx.x)] = val = (lane>=16)?shmem[threadIdx.x-16]+val:val;
    return val;
}

template <int W> __device__ __forceinline__
int warp_scan_exclusive(int val, int* shmem, int lane) {
    return warp_scan_inclusive<W>(val, shmem, lane) - val;
}

__global__ void block_scan_kernel(
        const int* __restrict__ d_in,
        int* __restrict__ d_out,
        int n,
        int* __restrict__ d_block_sums,
        int* __restrict__ d_block_offsets)
{
    constexpr int W = WARP;
    constexpr int NUM_WARPS = BLOCK_THREADS/W;
    constexpr int SHMEM_ELEMS = BLOCK_THREADS + BLOCK_THREADS/W;
    __shared__ int shmem[SHMEM_ELEMS];
    int tid = threadIdx.x;
    int lane = tid & (W-1);
    int warp_id = tid / W;

    int block_offset = blockIdx.x * BLOCK_THREADS;
    int idx = block_offset + tid;

    int val = (idx < n)? d_in[idx] : 0;

    // warp exclusive scan, then inclusive sum for this warp
    int warp_excl = warp_scan_exclusive<W>(val, shmem + blockDim.x, lane);
    int warp_incl = warp_excl + val;

    // write warp totals in shmem
    if (lane == W-1) shmem[warp_id] = warp_incl;
    __syncthreads();

    // scan warp totals
    int thread_val = 0;
    if (tid < NUM_WARPS) thread_val = shmem[tid];
    int warp_total_excl = warp_scan_exclusive<W>(thread_val, shmem + blockDim.x, lane);

    // broadcast totals to all threads
    if (tid < NUM_WARPS) shmem[tid] = warp_total_excl;
    __syncthreads();

    // distribute carry to all warp offsets
    int block_carry = shmem[warp_id];
    int excl_scan = block_carry + warp_excl;

    if (idx < n) d_out[idx] = excl_scan;

    // write inclusive block sum for carry propagation
    int block_total = shmem[NUM_WARPS-1] + shmem[SHMEM_ELEMS-1];
    if (tid == blockDim.x-1) d_block_sums[blockIdx.x] = block_total;
}

__global__ void carry_kernel(
        const int* __restrict__ d_block_sums,
        int*  __restrict__ d_block_offsets,
        int num_blocks)
{
    int tid = blockIdx.x*blockDim.x + threadIdx.x;
    if (tid==0) d_block_offsets[0] = 0;
    for(int i=tid+1; i<num_blocks; i+=gridDim.x*blockDim.x) {
        d_block_offsets[i] = d_block_sums[i-1];
    }
}

__global__ void add_carry_kernel(
        int* __restrict__ d_out,
        const int* __restrict__ d_block_offsets,
        int n)
{
    int idx = blockIdx.x*blockDim.x + threadIdx.x;
    if (idx < n) {
        int carry = d_block_offsets[blockIdx.x];
        d_out[idx] += carry;
    }
}

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    static int* d_block_sums = nullptr;
    static int* d_block_offs = nullptr;

    int total_threads = n;
    int num_blocks = (total_threads + BLOCK_THREADS - 1) / BLOCK_THREADS;

    // persistent allocations
    if (d_block_sums==nullptr) {
        cudaMalloc(&d_block_sums, sizeof(int)*num_blocks);
        cudaMalloc(&d_block_offs, sizeof(int)*num_blocks);
    }

    for (int iter = 0; iter < iters; ++iter) {
        block_scan_kernel<<<num_blocks, BLOCK_THREADS>>>(input, output, n, d_block_sums, d_block_offs);
        carry_kernel<<<128, 128>>>(d_block_sums, d_block_offs, num_blocks);
        add_carry_kernel<<<num_blocks, BLOCK_THREADS>>>(output, d_block_offs, n);
    }
    cudaDeviceSynchronize();
}

}  // namespace