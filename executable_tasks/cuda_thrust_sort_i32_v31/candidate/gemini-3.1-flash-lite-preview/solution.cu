#include "interface.h"
#include <thrust/device_ptr.h>
#include <thrust/sort.h>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;
    
    // Copy input to output buffer once to prepare for sorting
    cudaMemcpy(output, input, n * sizeof(int), cudaMemcpyDeviceToDevice);
    
    // Create thrust device pointers for the output array
    thrust::device_ptr<int> dev_ptr(output);
    
    for (int iter = 0; iter < iters; ++iter) {
        // If not the first iteration, reset data to original input state
        if (iter > 0) {
            cudaMemcpy(output, input, n * sizeof(int), cudaMemcpyDeviceToDevice);
        }
        
        // Thrust sort uses a high-performance radix sort for integers
        thrust::sort(dev_ptr, dev_ptr + n);
    }
}