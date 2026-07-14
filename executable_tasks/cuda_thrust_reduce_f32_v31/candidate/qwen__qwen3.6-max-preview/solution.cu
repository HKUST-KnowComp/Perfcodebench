#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/reduce.h>
#include <thrust/device_ptr.h>
#include <thrust/execution_policy.h>

float reduce_f32(const float* input, int n, int iters) {
    auto d_input = thrust::device_pointer_cast(input);
    float result = 0.0f;
    for (int i = 0; i < iters; ++i) {
        result = thrust::reduce(thrust::device, d_input, d_input + n, 0.0f);
    }
    return result;
}