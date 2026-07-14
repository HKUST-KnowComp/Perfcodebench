#include "interface.h"

#include <vector>
#include <cstddef>

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
  const size_t n = values.size();
  if (n == 0) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;
    }
    return hash;
  }

  // Count total true flags once
  size_t true_count = 0;
  const uint8_t* __restrict fp = flags.data();
  for (size_t i = 0; i < n; ++i) {
    true_count += fp[i] ? 1 : 0;
  }

  // Pre-allocate output buffer once
  std::vector<uint32_t> out(n);
  uint32_t* __restrict outp = out.data();
  const uint32_t* __restrict vp = values.data();

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Single pass: place elements directly using running indices
    size_t left_idx = 0;
    size_t right_idx = true_count;

    for (size_t i = 0; i < n; ++i) {
      uint32_t v = vp[i];
      if (fp[i]) {
        outp[left_idx++] = v;
      } else {
        outp[right_idx++] = v;
      }
    }

    // Compute hash with unrolling for ILP
    hash = 1469598103934665603ULL;
    size_t i = 0;
    size_t n4 = n & ~size_t(3);
    
    for (; i < n4; i += 4) {
      hash = mix(hash, outp[i]);
      hash = mix(hash, outp[i + 1]);
      hash = mix(hash, outp[i + 2]);
      hash = mix(hash, outp[i + 3]);
    }
    for (; i < n; ++i) {
      hash = mix(hash, outp[i]);
    }
  }

  return hash;
}