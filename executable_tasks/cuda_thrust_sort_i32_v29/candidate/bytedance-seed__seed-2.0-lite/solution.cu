#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/sort.h>
#include <thrust/device_ptr.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    for (int iter = 0; iter < iters; ++iter) {
        // Copy original input to output to start with fresh data each iteration, matching baseline behavior
        cudaMemcpy(output, input, n * sizeof(int), cudaMemcpyDeviceToDevice);
        // Create type-safe device pointer for Thrust operations
        thrust::device_ptr<int> d_output = thrust::device_pointer_cast(output);
        // Thrust's optimized parallel GPU sort produces identical ascending order to the baseline insertion sort
        thrust::sort(d_output, d_output + n);
    }
}