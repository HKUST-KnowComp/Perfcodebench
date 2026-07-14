#include "interface.h"
#include <thrust/reduce.h>
#include <thrust/device_ptr.h>

float reduce_f32(const float* input, int n, int iters) {
    thrust::device_ptr<const float> d_in(input);
    float sum = 0.0f;
    for (int i = 0; i < iters; ++i) {
        sum = thrust::reduce(d_in, d_in + n, 0.0f, thrust::plus<float>());
    }
    return sum;
}