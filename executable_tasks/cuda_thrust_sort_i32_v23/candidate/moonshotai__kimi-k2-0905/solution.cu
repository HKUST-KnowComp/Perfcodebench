#include "interface.h"
#include <thrust/device_vector.h>
#include <thrust/sort.h>
#include <thrust/copy.h>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n == 0 || iters == 0) return;

    // Allocate device vector once
    static thread_local thrust::device_vector<int> d_vec;
    if (static_cast<int>(d_vec.size()) != n) {
        d_vec.resize(n);
    }

    // Copy input to device only once
    thrust::copy_n(thrust::device_pointer_cast(input), n, d_vec.begin());

    // Sort repeatedly in-place
    for (int iter = 0; iter < iters; ++iter) {
        thrust::sort(thrust::device, d_vec.begin(), d_vec.end());
    }

    // Copy result back to the output pointer
    thrust::copy_n(d_vec.begin(), n, thrust::device_pointer_cast(output));
}