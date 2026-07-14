#include "interface.h"
#include <thrust/device_ptr.h>
#include <thrust/reduce.h>
#include <thrust/execution_policy.h>

float reduce_f32(const float* input, int n, int iters) {
    float result = 0.0f;
    for (int iter = 0; iter < iters; ++iter) {
        result = thrust::reduce(thrust::device, input, input + n, 0.0f, thrust::plus<float>());
    }
    return result;
}