#include "interface.h"
#include <thrust/reduce.h>
#include <thrust/execution_policy.h>

float reduce_f32(const float* input, int n, int iters) {
    (void)iters;
    return thrust::reduce(thrust::device, input, input + n);
}
