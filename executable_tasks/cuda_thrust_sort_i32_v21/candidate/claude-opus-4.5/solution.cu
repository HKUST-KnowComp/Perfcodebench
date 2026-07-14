#include "interface.h"
#include <thrust/sort.h>
#include <thrust/device_ptr.h>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    thrust::device_ptr<const int> d_input(input);
    thrust::device_ptr<int> d_output(output);
    
    for (int iter = 0; iter < iters; ++iter) {
        // Copy input to output
        cudaMemcpyAsync(output, input, n * sizeof(int), cudaMemcpyDeviceToDevice);
        // Sort output in-place using Thrust's optimized radix sort
        thrust::sort(d_output, d_output + n);
    }
}