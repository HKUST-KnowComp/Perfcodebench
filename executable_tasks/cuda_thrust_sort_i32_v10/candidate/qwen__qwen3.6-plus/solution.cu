#include "interface.h"
#include <thrust/sort.h>
#include <thrust/execution_policy.h>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    for (int i = 0; i < iters; ++i) {
        cudaMemcpy(output, input, n * sizeof(int), cudaMemcpyDeviceToDevice);
        thrust::sort(thrust::device, output, output + n);
    }
}
