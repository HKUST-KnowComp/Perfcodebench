#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/execution_policy.h>
#include <thrust/sort.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0 || iters <= 0) return;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemcpy(output, input, n * sizeof(int), cudaMemcpyDeviceToDevice);
        thrust::sort(thrust::device, output, output + n);
    }
    cudaDeviceSynchronize();
}
