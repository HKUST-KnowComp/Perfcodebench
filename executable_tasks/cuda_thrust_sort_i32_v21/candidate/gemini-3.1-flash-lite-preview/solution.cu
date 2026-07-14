#include "interface.h"
#include <thrust/device_ptr.h>
#include <thrust/sort.h>
#include <thrust/copy.h>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;
    
    // Create device pointers for Thrust
    thrust::device_ptr<const int> d_input(input);
    thrust::device_ptr<int> d_output(output);

    for (int iter = 0; iter < iters; ++iter) {
        // Copy input to output buffer
        thrust::copy(d_input, d_input + n, d_output);
        
        // Perform high-performance radix sort
        thrust::sort(d_output, d_output + n);
        
        // Ensure completion before next iteration or return
        cudaDeviceSynchronize();
    }
}