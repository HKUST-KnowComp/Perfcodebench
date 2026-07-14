#include "interface.h"

#include <cub/cub.cuh>
#include <cuda_runtime.h>

#include <cstdint>

namespace {

void *d_temp_storage = nullptr;
size_t temp_storage_bytes = 0;

} // namespace

void exclusive_scan_i32(const int *input,
                        int       *output,
                        int        n,
                        int        iters) {
  // Determine required temporary storage size once, then reuse.
  if (d_temp_storage == nullptr) {
    cub::DeviceScan::ExclusiveSum(nullptr, temp_storage_bytes,
                                  input, output, n);
    cudaMalloc(&d_temp_storage, temp_storage_bytes);
  }

  for (int i = 0; i < iters; ++i) {
    cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes,
                                  input, output, n);
  }
}