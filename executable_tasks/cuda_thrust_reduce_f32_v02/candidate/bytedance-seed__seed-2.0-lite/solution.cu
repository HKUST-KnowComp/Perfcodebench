#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/reduce.h>

float reduce_f32(const float* input, int n, int iters) {
    const thrust::device_ptr<const float> dev_ptr = thrust::device_pointer_cast(input);
    float actual = 0.0f;
    
    for (int iter = 0; iter < iters; ++iter) {
        actual = thrust::reduce(dev_ptr, dev_ptr + n, 0.0f);
    }
    
    return actual;
}