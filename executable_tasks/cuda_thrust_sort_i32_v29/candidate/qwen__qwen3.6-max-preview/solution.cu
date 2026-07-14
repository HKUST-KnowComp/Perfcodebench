#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/sort.h>
#include <thrust/device_ptr.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    auto d_out = thrust::device_pointer_cast(output);
    for (int i = 0; i < iters; ++i) {
        cudaMemcpy(output, input, n * sizeof(int), cudaMemcpyDeviceToDevice);
        thrust::sort(d_out, d_out + n);
    }
}
