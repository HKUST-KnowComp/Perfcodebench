#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust://reduce.h>

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0) return 0.0f;

    thrust::device_ptr<const float> d_ptr(input);
    float actual = 0.0f;

    for (int iter = 0; iter < iters; ++iter) {
        actual = thrust::reduce(d_ptr, d_ptr + n);
    }

    return actual;
}