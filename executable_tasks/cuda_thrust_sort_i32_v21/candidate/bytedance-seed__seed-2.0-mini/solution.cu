#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/sort.h>
#include <thrust/device_ptr.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    for (int iter = 0; iter < iters; ++iter) {
        // Copy input data to output buffer once per iteration
        cudaMemcpy(output, input, n * sizeof(int), cudaMemcpyDeviceToDevice);
        // Sort the output buffer in-place using Thrust's parallel sort
        thrust::device_ptr<int> dev_output(output);
        thrust::sort(dev_output, dev_output + n);
    }
}