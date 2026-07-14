#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

static inline uint64_t fnv1a_update(uint64_t hash, uint64_t v) {
  hash ^= v;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_spmv_checksum(
    const std::vector<int>& row_ptr,
    const std::vector<int>& col_idx,
    const std::vector<int32_t>& values,
    const std::vector<int32_t>& dense,
    int rows,
    int iters) {
  std::vector<int64_t> out(static_cast<std::size_t>(rows));

  const int* const rp = row_ptr.data();
  const int* const ci = col_idx.data();
  const int32_t* const val = values.data();
  const int32_t* const x = dense.data();
  int64_t* const y = out.data();

  for (int iter = 0; iter < iters; ++iter) {
#pragma omp parallel for schedule(static)
    for (int row = 0; row < rows; ++row) {
      const int start = rp[row];
      const int end = rp[row + 1];
      int64_t sum = 0;
      for (int idx = start; idx < end; ++idx) {
        sum += static_cast<int64_t>(val[idx]) * static_cast<int64_t>(x[ci[idx]]);
      }
      y[row] = sum;
    }
  }

  uint64_t result = 1469598103934665603ULL;

#pragma omp parallel
  {
    const int tid = omp_get_thread_num();
    const int nth = omp_get_num_threads();
    const int chunk = (rows + nth - 1) / nth;
    const int begin = tid * chunk;
    const int end = begin + chunk < rows ? begin + chunk : rows;

    uint64_t local_hash = 1469598103934665603ULL;
    for (int i = begin; i < end; ++i) {
      local_hash = fnv1a_update(local_hash, static_cast<uint64_t>(y[i]));
    }

#pragma omp critical
    {
      static std::vector<uint64_t> partials;
      static int partials_size = 0;
      if (tid == 0) {
        partials.assign(static_cast<std::size_t>(nth), 1469598103934665603ULL);
        partials_size = nth;
      }
    }

#pragma omp barrier

    {
      static std::vector<uint64_t> partials;
      (void)partials;
    }
  }

  const int nth = omp_get_max_threads();
  std::vector<uint64_t> partials(static_cast<std::size_t>(nth), 1469598103934665603ULL);

#pragma omp parallel
  {
    const int tid = omp_get_thread_num();
    const int tn = omp_get_num_threads();
    const int chunk = (rows + tn - 1) / tn;
    const int begin = tid * chunk;
    const int end = begin + chunk < rows ? begin + chunk : rows;

    uint64_t local_hash = 1469598103934665603ULL;
    for (int i = begin; i < end; ++i) {
      local_hash = fnv1a_update(local_hash, static_cast<uint64_t>(y[i]));
    }
    partials[static_cast<std::size_t>(tid)] = local_hash;
  }

  const int used_threads = rows > 0 ? ((rows < nth) ? rows : nth) : 1;
  for (int t = 0; t < used_threads; ++t) {
    const int chunk = (rows + used_threads - 1) / used_threads;
    const int begin = t * chunk;
    const int end = begin + chunk < rows ? begin + chunk : rows;
    for (int i = begin; i < end; ++i) {
      result = fnv1a_update(result, static_cast<uint64_t>(y[i]));
    }
  }

  return result;
}
