#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/sort.h>
#include <thrust/device_ptr.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    for (int i = 0; i < iters; ++i) {
        // Copy input to output (device to device)
        cudaMemcpyAsync(output, input, n * sizeof(int), cudaMemcpyDeviceToDevice);
        // Sort output using Thrust radix sort
        thrust::sort(thrust::device_pointer_cast(output),
                     thrust::device_pointer_cast(output) + n);
    }
}
