#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/sort.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    // Use thrust to perform an in-place sort on the output buffer
    for (int i = 0; i < iters; ++i) {
        // Copy input to output (device-to-device)
        cudaMemcpy(output, input, n * sizeof(int), cudaMemcpyDeviceToDevice);
        // Sort output in-place using Thrust's fast radix sort
        thrust::sort(thrust::device_pointer_cast(output),
                     thrust::device_pointer_cast(output) + n);
    }
}
