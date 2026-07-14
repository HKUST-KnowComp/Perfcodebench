#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters) {
  // Match baseline behavior for non-positive iters
  if (iters <= 0) {
    return 0ULL;
  }

  // If domain is zero, the hash remains the offset basis when an iteration occurs
  if (domain == 0u) {
    return 1469598103934665603ULL;
  }

  // Accumulate once since each iteration is identical in the baseline
  const size_t d = static_cast<size_t>(domain);
  std::vector<uint64_t> accum(d, 0ULL);

  const uint32_t* idx_ptr = indices.data();
  const uint32_t* val_ptr = values.data();
  const size_t n = indices.size();

  bool any_in_domain = false;
  for (size_t i = 0; i < n; ++i) {
    uint32_t idx = idx_ptr[i];
    if (idx < domain) {
      accum[static_cast<size_t>(idx)] += static_cast<uint64_t>(val_ptr[i]);
      any_in_domain = true;
    }
  }

  uint64_t hash = 1469598103934665603ULL;
  if (!any_in_domain) {
    // Faster path: all values are zero in-domain, only multiply by prime 'domain' times
    // (equivalent to mixing zero repeatedly)
    for (size_t i = 0; i < d; ++i) {
      hash *= 1099511628211ULL;
    }
  } else {
    const uint64_t* a = accum.data();
    for (size_t i = 0; i < d; ++i) {
      hash = mix(hash, a[i]);
    }
  }

  return hash;
}
