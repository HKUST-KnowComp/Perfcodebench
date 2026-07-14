#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/reduce.h>
#include <thrust/device_ptr.h>
#include <thrust/execution_policy.h>

float reduce_f32(const float* input, int n, int iters) {
    thrust::device_ptr<const float> d_ptr(input);
    float result = 0.0f;
    for (int i = 0; i < iters; ++i) {
        result = thrust::reduce(thrust::device, d_ptr, d_ptr + n, 0.0f, thrust::plus<float>());
    }
    return result;
}