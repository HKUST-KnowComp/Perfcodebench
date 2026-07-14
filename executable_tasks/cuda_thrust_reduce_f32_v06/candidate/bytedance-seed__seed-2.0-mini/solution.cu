#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/reduce.h>
#include <thrust/device_ptr.h>
#include <thrust/system/cuda/execution_policy.h>

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0) {
        return 0.0f;
    }
    float actual = 0.0f;
    const auto d_input_begin = thrust::device_pointer_cast(input);
    const auto d_input_end = d_input_begin + n;
    for (int iter = 0; iter < iters; ++iter) {
        actual = thrust::reduce(thrust::cuda::par, d_input_begin, d_input_end);
    }
    return actual;
}