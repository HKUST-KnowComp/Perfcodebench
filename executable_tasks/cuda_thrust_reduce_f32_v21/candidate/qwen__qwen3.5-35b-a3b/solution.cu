#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/reduce.h>
#include <thrust/functional.h>

float reduce_f32(const float* input, int n, int iters) {
    float actual = 0.0f;
    for (int iter = 0; iter < iters; ++iter) {
        thrust::device_ptr<const float> d_input(input);
        actual = thrust::reduce(thrust::device, d_input, d_input + n, 0.0f, thrust::plus<float>());
    }
    return actual;
}