#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/sort.h>
#include <thrust/copy.h>
#include <thrust/device_vector.h>
#include <thrust/execution_policy.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n == 0 || iters == 0) {
        return;
    }

    thrust::device_vector<int> temp(input, input + n);

    for (int iter = 0; iter < iters; ++iter) {
        if (input != output) {
            thrust::copy(thrust::device, temp.begin(), temp.end(), output);
        }
        thrust::sort(thrust::device, output, output + n);
    }
}