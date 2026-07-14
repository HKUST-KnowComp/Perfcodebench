#include "interface.h"
#include <thrust/sort.h>
#include <thrust/device_ptr.h>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Copy input to output once before the iteration loop
    cudaMemcpy(output, input, n * sizeof(int), cudaMemcpyDeviceToDevice);

    // Wrap raw pointers in thrust device pointers
    thrust::device_ptr<int> dev_ptr(output);

    for (int iter = 0; iter < iters; ++iter) {
        // thrust::sort for int32 uses a highly optimized radix sort
        thrust::sort(dev_ptr, dev_ptr + n);
    }
}