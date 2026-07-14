#include "interface.h"

#include <vector>
#include <cstdint>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  const std::size_t n = values.size();
  if (n == 0) {
    uint64_t hash = 1469598103934665603ULL;
    return hash;
  }

  // Pre-allocate output buffer once
  std::vector<uint32_t> out(n);

  // Pre-compute prefix sums for stable partition indices
  // left_idx[i] = number of flags[j]==1 for j < i (index in left partition)
  // right_idx[i] = number of flags[j]==0 for j < i (offset in right partition)
  std::vector<uint32_t> left_idx(n);
  std::vector<uint32_t> right_idx(n);

  uint32_t left_count = 0;
  uint32_t right_count = 0;
  for (std::size_t i = 0; i < n; ++i) {
    left_idx[i] = left_count;
    right_idx[i] = right_count;
    if (flags[i]) {
      ++left_count;
    } else {
      ++right_count;
    }
  }

  // Total left elements determines where right partition starts
  const uint32_t left_total = left_count;

  const uint8_t* __restrict f_ptr = flags.data();
  const uint32_t* __restrict v_ptr = values.data();
  const uint32_t* __restrict li_ptr = left_idx.data();
  const uint32_t* __restrict ri_ptr = right_idx.data();

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint32_t* __restrict o_ptr = out.data();

    // Scatter values to their stable partition positions
    for (std::size_t i = 0; i < n; ++i) {
      if (f_ptr[i]) {
        o_ptr[li_ptr[i]] = v_ptr[i];
      } else {
        o_ptr[left_total + ri_ptr[i]] = v_ptr[i];
      }
    }

    // Compute hash
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < n; ++i) {
      hash = mix(hash, o_ptr[i]);
    }
  }

  return hash;
}