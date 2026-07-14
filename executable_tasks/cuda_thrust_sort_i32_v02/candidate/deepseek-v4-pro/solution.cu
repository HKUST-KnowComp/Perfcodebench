#include "interface.h"
#include <thrust/sort.h>
#include <thrust/device_ptr.h>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    for (int iter = 0; iter < iters; ++iter) {
        // copy input to output on the device
        cudaMemcpy(output, input, n * sizeof(int), cudaMemcpyDeviceToDevice);
        // sort the output array using Thrust
        thrust::sort(thrust::device, output, output + n);
    }
}
