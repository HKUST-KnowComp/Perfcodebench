#include "interface.h"
#include <thrust/reduce.h>
#include <thrust/execution_policy.h>

float reduce_f32(const float* input, int n, int iters) {
    float actual = 0.0f;
    for (int iter = 0; iter < iters; ++iter) {
        actual = thrust::reduce(thrust::device, input, input + n, 0.0f);
    }
    return actual;
}
