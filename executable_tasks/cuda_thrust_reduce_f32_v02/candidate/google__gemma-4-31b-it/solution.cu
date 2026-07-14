#include "interface.h"
#include <thrust/reduce.h>
#include <thrust/device_ptr.h>

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0) return 0.0f;

    // Wrap the raw pointer in a thrust device pointer
    thrust::device_ptr<const float> dev_ptr(input);
    float result = 0.0f;

    for (int iter = 0; iter < iters; ++iter) {
        // thrust::reduce is highly optimized for float reduction on CUDA
        result = thrust::reduce(dev_ptr, dev_ptr + n, 0.0f, thrust::plus<float>());
    }

    return result;
}