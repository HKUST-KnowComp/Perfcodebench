#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/reduce.h>
#include <thrust/device_ptr.h>
#include <thrust/execution_policy.h>

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0) return 0.0f;

    // Wrap the raw pointer into a thrust device pointer
    thrust::device_ptr<const float> dev_ptr(input);
    float result = 0.0f;

    for (int iter = 0; iter < iters; ++iter) {
        // thrust::reduce is highly optimized for GPU architectures
        // It uses a combination of shared memory and warp-level primitives
        result = thrust::reduce(thrust::device, dev_ptr, dev_ptr + n, 0.0f, thrust::plus<float>());
    }

    return result;
}