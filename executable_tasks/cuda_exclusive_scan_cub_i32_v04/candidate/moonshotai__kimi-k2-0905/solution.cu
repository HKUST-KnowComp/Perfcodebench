#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceScan::ExclusiveSum(nullptr,  // no custom temp storage
                                    0,        // size_t& bytes temp storage -> ignored when null
                                    input,
                                    output,
                                    n);
  }
}