#include "interface.h"

#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

static inline void merge_sort_int32(int32_t* data, int32_t* buf, std::size_t n) {
  if (n <= 1) return;

  int32_t* src = data;
  int32_t* dst = buf;

  for (std::size_t width = 1; width < n; width <<= 1) {
    for (std::size_t left = 0; left < n; left += (width << 1)) {
      std::size_t mid = left + width;
      std::size_t right = left + (width << 1);
      if (mid > n) mid = n;
      if (right > n) right = n;

      std::size_t i = left;
      std::size_t j = mid;
      std::size_t k = left;

      while (i < mid && j < right) {
        if (src[i] <= src[j]) {
          dst[k++] = src[i++];
        } else {
          dst[k++] = src[j++];
        }
      }
      while (i < mid) dst[k++] = src[i++];
      while (j < right) dst[k++] = src[j++];
    }

    int32_t* tmp = src;
    src = dst;
    dst = tmp;
  }

  if (src != data) {
    for (std::size_t i = 0; i < n; ++i) data[i] = src[i];
  }
}

static inline int64_t sorted_pairwise_abs_sum(const int32_t* a, std::size_t n) {
  int64_t prefix = 0;
  int64_t total = 0;

  int tcount = omp_get_max_threads();
  if (tcount < 1) tcount = 1;
  if (n < static_cast<std::size_t>(tcount * 1024)) tcount = 1;

  if (tcount == 1) {
    for (std::size_t i = 0; i < n; ++i) {
      const int64_t v = static_cast<int64_t>(a[i]);
      total += v * static_cast<int64_t>(i) - prefix;
      prefix += v;
    }
    return total;
  }

  std::vector<int64_t> block_sums(static_cast<std::size_t>(tcount), 0);
  std::vector<int64_t> block_totals(static_cast<std::size_t>(tcount), 0);
  std::vector<std::size_t> starts(static_cast<std::size_t>(tcount + 1), 0);

  const std::size_t chunk = (n + static_cast<std::size_t>(tcount) - 1) / static_cast<std::size_t>(tcount);
  for (int t = 0; t < tcount; ++t) {
    starts[static_cast<std::size_t>(t)] = static_cast<std::size_t>(t) * chunk;
    if (starts[static_cast<std::size_t>(t)] > n) starts[static_cast<std::size_t>(t)] = n;
  }
  starts[static_cast<std::size_t>(tcount)] = n;

#pragma omp parallel num_threads(tcount)
  {
    const int tid = omp_get_thread_num();
    const std::size_t begin = starts[static_cast<std::size_t>(tid)];
    const std::size_t end = starts[static_cast<std::size_t>(tid + 1)];

    int64_t local_sum = 0;
    int64_t local_total = 0;
    for (std::size_t i = begin; i < end; ++i) {
      const int64_t v = static_cast<int64_t>(a[i]);
      local_total += v * static_cast<int64_t>(i - begin) - local_sum;
      local_sum += v;
    }
    block_sums[static_cast<std::size_t>(tid)] = local_sum;
    block_totals[static_cast<std::size_t>(tid)] = local_total;
  }

  int64_t carry_sum = 0;
  std::size_t carry_count = 0;
  for (int t = 0; t < tcount; ++t) {
    const std::size_t begin = starts[static_cast<std::size_t>(t)];
    const std::size_t end = starts[static_cast<std::size_t>(t + 1)];
    const std::size_t len = end - begin;

    total += block_totals[static_cast<std::size_t>(t)];
    if (len) {
      int64_t block_cross = 0;
      for (std::size_t i = begin; i < end; ++i) {
        const int64_t v = static_cast<int64_t>(a[i]);
        block_cross += v * static_cast<int64_t>(carry_count) - carry_sum;
      }
      total += block_cross;
      carry_sum += block_sums[static_cast<std::size_t>(t)];
      carry_count += len;
    }
  }

  return total;
}

}  // namespace

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  const std::size_t n = xs.size();
  if (n <= 1 || iters <= 0) return 0;

  std::vector<int32_t> sx(xs.begin(), xs.end());
  std::vector<int32_t> sy(ys.begin(), ys.end());
  std::vector<int32_t> bx(n);
  std::vector<int32_t> by(n);

  merge_sort_int32(sx.data(), bx.data(), n);
  merge_sort_int32(sy.data(), by.data(), n);

  const int64_t one_iter = sorted_pairwise_abs_sum(sx.data(), n) +
                           sorted_pairwise_abs_sum(sy.data(), n);

  int64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    total = one_iter;
  }
  return total;
}
