#include "interface.h"
#include <thrust/device_ptr.h>
#include <thrust/sort.h>
#include <thrust/copy.h>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;
    
    // Create thrust device pointers for the input and output buffers
    thrust::device_ptr<const int> d_input(input);
    thrust::device_ptr<int> d_output(output);

    for (int iter = 0; iter < iters; ++iter) {
        // Copy input to output buffer to preserve original input for subsequent iterations
        // and to perform the sort in-place on the output buffer.
        thrust::copy(d_input, d_input + n, d_output);
        
        // Use thrust::sort which dispatches to highly optimized radix sort for integers
        thrust::sort(d_output, d_output + n);
    }
    
    // Ensure all operations complete before returning
    cudaDeviceSynchronize();
}