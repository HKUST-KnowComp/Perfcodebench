#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/sort.h>
#include <thrust/device_ptr.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    // Copy input to output once
    cudaMemcpy(output, input, n * sizeof(int), cudaMemcpyDeviceToDevice);

    // Use Thrust's parallel radix sort on the device pointer
    thrust::device_ptr<int> d_output = thrust::device_pointer_cast(output);
    for (int i = 0; i < iters; ++i) {
        thrust::sort(d_output, d_output + n);
    }
}
