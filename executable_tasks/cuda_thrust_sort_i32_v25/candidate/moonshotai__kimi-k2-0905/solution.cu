#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_vector.h>
#include <thrust/sort.h>
#include <thrust/copy.h>

extern "C" void sort_vec_i32(const int* input,
                              int*       output,
                              int        n,
                              int        iters)
{
    const size_t nbytes = n * sizeof(int);

    // Allocate device buffers once; reuse across sweeps
    static int* d_in  = nullptr;
    static int* d_out = nullptr;
    static int  cap = 0;

    if (n > cap) {
        cudaFree(d_in);
        cudaFree(d_out);
        cudaMalloc(&d_in,  nbytes);
        cudaMalloc(&d_out, nbytes);
        cap = n;
    }
    cudaMemcpy(d_in, input, nbytes, cudaMemcpyHostToDevice);

    // Multiple warm-up/timed launches w/out host syncs
    for (int i = 0; i < iters; ++i) {
        cudaMemcpyAsync(d_in, input, nbytes, cudaMemcpyHostToDevice);
        thrust::device_ptr<int> dv(d_in);
        thrust::sort(dv, dv + n);          // radix sort on the GPU
        cudaMemcpyAsync(output, d_in, nbytes, cudaMemcpyDeviceToHost);
    }
    cudaDeviceSynchronize();
}