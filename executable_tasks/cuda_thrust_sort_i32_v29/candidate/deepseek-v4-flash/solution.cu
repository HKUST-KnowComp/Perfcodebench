#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/sort.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    // Copy input to output once (device to device)
    cudaMemcpy(output, input, n * sizeof(int), cudaMemcpyDeviceToDevice);
    thrust::device_ptr<int> dev_out(output);
    for (int i = 0; i < iters; ++i) {
        thrust::sort(dev_out, dev_out + n);
    }
}
