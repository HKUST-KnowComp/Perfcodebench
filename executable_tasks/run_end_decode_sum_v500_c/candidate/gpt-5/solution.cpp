#include "interface.h"

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  (void)iters;

  const std::size_t n = values.size();
  const uint32_t* re = run_ends.data();
  const uint32_t* val = values.data();

  uint64_t sum = 0;
  uint32_t prev = 0;

  std::size_t i = 0;
  for (; i + 4 <= n; i += 4) {
    uint32_t len0 = re[i] - prev;
    sum += static_cast<uint64_t>(len0) * static_cast<uint64_t>(val[i]);
    prev = re[i];

    uint32_t len1 = re[i + 1] - prev;
    sum += static_cast<uint64_t>(len1) * static_cast<uint64_t>(val[i + 1]);
    prev = re[i + 1];

    uint32_t len2 = re[i + 2] - prev;
    sum += static_cast<uint64_t>(len2) * static_cast<uint64_t>(val[i + 2]);
    prev = re[i + 2];

    uint32_t len3 = re[i + 3] - prev;
    sum += static_cast<uint64_t>(len3) * static_cast<uint64_t>(val[i + 3]);
    prev = re[i + 3];
  }

  for (; i < n; ++i) {
    uint32_t len = re[i] - prev;
    sum += static_cast<uint64_t>(len) * static_cast<uint64_t>(val[i]);
    prev = re[i];
  }

  return sum;
}
