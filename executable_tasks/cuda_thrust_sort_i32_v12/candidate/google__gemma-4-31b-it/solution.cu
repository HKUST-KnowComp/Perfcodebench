#include "interface.h"
#include <thrust/sort.h>
#include <thrust/device_ptr.h>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Copy input to output once before the iteration loop to avoid redundant copies
    // if the sort is performed in-place on the output buffer.
    cudaMemcpy(output, input, n * sizeof(int), cudaMemcpyDeviceToDevice);

    thrust::device_ptr<int> dev_ptr(output);

    for (int iter = 0; iter < iters; ++iter) {
        // thrust::sort for int32_t uses a highly optimized radix sort
        thrust::sort(dev_ptr, dev_ptr + n);
    }
}