#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/reduce.h>
#include <thrust/device_ptr.h>

float reduce_f32(const float* input, int n, int iters) {
    float sum = 0.0f;
    thrust::device_ptr<const float> d_ptr(input);
    for (int i = 0; i < iters; ++i) {
        sum = thrust::reduce(d_ptr, d_ptr + n, 0.0f, thrust::plus<float>());
    }
    return sum;
}