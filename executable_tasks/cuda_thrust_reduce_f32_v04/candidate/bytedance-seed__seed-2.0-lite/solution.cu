#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/reduce.h>
#include <thrust/device_ptr.h>

float reduce_f32(const float* input, int n, int iters) {
    float actual = 0.0f;
    const auto input_start = thrust::device_pointer_cast<const float>(input);
    const auto input_end = input_start + n;
    
    for (int iter = 0; iter < iters; ++iter) {
        actual = thrust::reduce(input_start, input_end, 0.0f);
    }
    
    return actual;
}