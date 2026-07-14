#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

namespace {

__global__ void cast_int64_to_int32(const int64_t* src, int* dst) {
  if (threadIdx.x == 0 && blockIdx.x == 0) {
    *dst = static_cast<int>(*src);
  }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  if (iters <= 0) {
    return;
  }

  if (n <= 0) {
    for (int iter = 0; iter < iters; ++iter) {
      cudaMemset(output, 0, sizeof(int));
    }
    return;
  }

  int64_t* d_sum_buf = nullptr;
  (void)cudaMalloc(&d_sum_buf, sizeof(int64_t));

  void* d_temp_storage = nullptr;
  size_t temp_storage_bytes = 0;
  (void)cub::DeviceReduce::Sum(
      d_temp_storage,
      temp_storage_bytes,
      input,
      d_sum_buf,
      n,
      cub::Sum{},
      0
  );

  (void)cudaMalloc(&d_temp_storage, temp_storage_bytes);

  for (int iter = 0; iter < iters; ++iter) {
    (void)cub::DeviceReduce::Sum(
        d_temp_storage,
        temp_storage_bytes,
        input,
        d_sum_buf,
        n,
        cub::Sum{},
        0
    );

    cast_int64_to_int32<<<1, 1>>>(d_sum_buf, output);
  }

  (void)cudaFree(d_sum_buf);
  (void)cudaFree(d_temp_storage);
}