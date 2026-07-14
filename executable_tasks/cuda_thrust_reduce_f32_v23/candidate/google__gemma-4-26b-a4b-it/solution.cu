#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust://reduce.h>

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0) return 0.0f;

    // Wrap the raw device pointer into a thrust device pointer
    thrust::device_ptr<const float> d_ptr(const_cast<float*>(input));

    float actual = 0.0f;
    
    // Perform the reduction 'iters' times as required by the interface
    // Note: Thrust's reduce is highly optimized for parallel execution
    for (int iter = 0; iter < iters; ++iter) {
        actual = thrust::reduce(d_ptr, d_ptr + n);
    }

    return actual;
}