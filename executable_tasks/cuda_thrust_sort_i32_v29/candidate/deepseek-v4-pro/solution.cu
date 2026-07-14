#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_vector.h>
#include <thrust/sort.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    static thrust::device_vector<int> d_input;
    static thrust::device_vector<int> d_work;

    // Ensure enough capacity for the current problem size
    if (d_input.size() < n) {
        d_input.resize(n);
        d_work.resize(n);
    }

    // Copy the input data to the device once
    if (n > 0) {
        cudaMemcpy(thrust::raw_pointer_cast(d_input.data()), input, n * sizeof(int), cudaMemcpyHostToDevice);
    }

    for (int iter = 0; iter < iters; ++iter) {
        // Copy original input to the work buffer
        if (n > 0) {
            cudaMemcpy(thrust::raw_pointer_cast(d_work.data()),
                       thrust::raw_pointer_cast(d_input.data()),
                       n * sizeof(int),
                       cudaMemcpyDeviceToDevice);
        }

        // Sort the work buffer
        thrust::sort(d_work.begin(), d_work.begin() + n);

        // Copy sorted result back to the host output
        if (n > 0) {
            cudaMemcpy(output, thrust::raw_pointer_cast(d_work.data()), n * sizeof(int), cudaMemcpyDeviceToHost);
        }
    }
}
