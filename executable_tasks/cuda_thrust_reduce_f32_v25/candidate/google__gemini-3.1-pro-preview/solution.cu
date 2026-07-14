#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/reduce.h>
#include <thrust/execution_policy.h>

/**
 * Performs a reduction of a float array using NVIDIA Thrust.
 * Thrust is highly optimized for various GPU architectures, utilizing 
 * grid-stride loops, shared memory, and warp-shuffle primitives.
 */
float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0) return 0.0f;

    float result = 0.0f;

    // The timing harness measures the duration of the loop.
    // thrust::reduce is synchronous with respect to the host by default 
    // when returning a scalar, which is appropriate for this interface.
    for (int iter = 0; iter < iters; ++iter) {
        result = thrust::reduce(thrust::device, input, input + n, 0.0f, thrust::plus<float>());
    }

    return result;
}