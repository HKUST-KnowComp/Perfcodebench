#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/sort.h>
#include <thrust/device_ptr.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0 || iters <= 0) return;
    if (n == 1) {
        cudaMemcpy(output, input, sizeof(int), cudaMemcpyHostToDevice);
        return;
    }

    // Allocate and initialize persistent device buffers
    int *d_in, *d_out;
    cudaMalloc(&d_in, n * sizeof(int));
    cudaMalloc(&d_out, n * sizeof(int));
    cudaMemcpy(d_in, input, n * sizeof(int), cudaMemcpyHostToDevice);

    // Run sort for all benchmark iterations to measure repeated launch cost
    for (int iter = 0; iter < iters; ++iter) {
        // Reset output buffer to original input each iteration, matching baseline behavior
        cudaMemcpy(d_out, d_in, n * sizeof(int), cudaMemcpyDeviceToDevice);
        // Wrap raw device pointers for Thrust's dispatch safety
        thrust::device_ptr<int> dev_out_begin = thrust::device_pointer_cast(d_out);
        thrust::device_ptr<int> dev_out_end = thrust::device_pointer_cast(d_out + n);
        // Thrust's highly optimized parallel GPU sort
        thrust::sort(dev_out_begin, dev_out_end);
    }

    // Copy final sorted result back to host output
    cudaMemcpy(output, d_out, n * sizeof(int), cudaMemcpyDeviceToHost);

    // Clean up device memory
    cudaFree(d_in);
    cudaFree(d_out);
}