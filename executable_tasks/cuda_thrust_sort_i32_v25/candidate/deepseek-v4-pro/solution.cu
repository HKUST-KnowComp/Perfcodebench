#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/sort.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    thrust::device_ptr<const int> in_ptr(input);
    thrust::device_ptr<int> out_ptr(output);
    for (int iter = 0; iter < iters; ++iter) {
        if (input != output) {
            cudaMemcpy(output, input, n * sizeof(int), cudaMemcpyDeviceToDevice);
        }
        thrust::sort(out_ptr, out_ptr + n);
    }
}
