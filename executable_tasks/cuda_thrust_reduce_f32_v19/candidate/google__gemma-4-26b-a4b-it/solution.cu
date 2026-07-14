#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust://reduce.h>

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0) return 0.0f;

    // Wrap the raw device pointer in a thrust device pointer
    thrust::device_ptr<const float> d_ptr(const_cast<float*>(input));

    float actual = 0.0f;
    for (int iter = 0; iter < iters; ++iter) {
        // thrust::reduce is highly optimized for various architectures
        actual = thrust::reduce(d_ptr, d_ptr + n);
    }

    return actual;
}