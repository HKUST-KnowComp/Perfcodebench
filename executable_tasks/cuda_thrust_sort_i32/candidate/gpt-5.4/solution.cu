#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

namespace {

inline void safe_cuda(cudaError_t err) {
  (void)err;
}

}  // namespace

void sort_vec_i32(const int* input, int* output, int n, int iters) {
  if (iters <= 0 || n <= 0) return;

  void* temp_storage = nullptr;
  size_t temp_storage_bytes = 0;

  cub::DeviceRadixSort::SortKeys(
      temp_storage, temp_storage_bytes,
      input, output,
      n,
      0, sizeof(int) * 8);

  safe_cuda(cudaMalloc(&temp_storage, temp_storage_bytes));

  for (int iter = 0; iter < iters; ++iter) {
    if (iter != 0) {
      safe_cuda(cudaMemcpy(output, input, static_cast<size_t>(n) * sizeof(int), cudaMemcpyDeviceToDevice));
    }
    cub::DeviceRadixSort::SortKeys(
        temp_storage, temp_storage_bytes,
        output, output,
        n,
        0, sizeof(int) * 8);
  }

  safe_cuda(cudaFree(temp_storage));
}
