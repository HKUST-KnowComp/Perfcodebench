#include "interface.h"
#include <thrust/device_ptr.h>
#include <thrust/sort.h>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;
    
    for (int iter = 0; iter < iters; ++iter) {
        // Copy input to output buffer
        cudaMemcpy(output, input, n * sizeof(int), cudaMemcpyDeviceToDevice);
        
        // Use Thrust's highly optimized radix sort
        thrust::device_ptr<int> ptr(output);
        thrust::sort(ptr, ptr + n);
    }
}