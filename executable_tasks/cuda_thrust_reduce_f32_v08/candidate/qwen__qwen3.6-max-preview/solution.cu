#include "interface.h"
#include <thrust/reduce.h>
#include <thrust/device_ptr.h>

float reduce_f32(const float* input, int n, int iters) {
    float result = 0.0f;
    auto d_in = thrust::device_pointer_cast(input);
    for (int i = 0; i < iters; ++i) {
        result = thrust::reduce(d_in, d_in + n, 0.0f);
    }
    return result;
}