#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  void* d_temp_storage = nullptr;
  size_t temp_storage_bytes = 0;

  // Determine required temporary storage size
  cub::DeviceScan::ExclusiveSum(
      d_temp_storage,
      temp_storage_bytes,
      input,
      output,
      n,
      cub::Sum()
  );

  // Allocate temporary storage
  cudaMalloc(&d_temp_storage, temp_storage_bytes);

  // Perform scan iters times
  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceScan::ExclusiveSum(
        d_temp_storage,
        temp_storage_bytes,
        input,
        output,
        n,
        cub::Sum()
    );
  }

  cudaFree(d_temp_storage);
}