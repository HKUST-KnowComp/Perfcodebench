#include "interface.h"
#include <thrust/device_ptr.h>
#include <thrust/reduce.h>
#include <cuda_runtime.h>

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0) return 0.0f;

    thrust::device_ptr<const float> dev_ptr(input);
    float result = 0.0f;

    // Perform the reduction using Thrust's optimized implementation.
    // Thrust automatically selects the best reduction strategy (e.g., block-wide reduction with shared memory).
    for (int i = 0; i < iters; ++i) {
        result = thrust::reduce(dev_ptr, dev_ptr + n, 0.0f, thrust::plus<float>());
    }

    return result;
}