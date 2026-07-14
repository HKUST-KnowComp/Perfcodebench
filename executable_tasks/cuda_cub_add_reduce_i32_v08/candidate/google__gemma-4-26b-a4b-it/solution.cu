#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

namespace {
// Pre-allocate workspace if needed, but for a general interface, 
// we use CUB's device-level reduction which handles its own internal management.
}

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // CUB DeviceReduce::Sum is the gold standard for performance and correctness.
    // It uses a highly optimized multi-stage reduction algorithm.
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum(output, input, n);
    }
}
