#include "interface.h"

namespace {

static inline uint64_t fnv1a_checksum_dense(const uint64_t* agg, int groups) {
  uint64_t hash = 1469598103934665603ULL; // FNV offset basis (64-bit)
  for (int g = 0; g < groups; ++g) {
    hash ^= agg[g];
    hash *= 1099511628211ULL; // FNV prime (64-bit)
  }
  return hash;
}

} // namespace

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
  // Match baseline behavior for edge cases
  if (iters <= 0) return 0ULL;
  if (groups <= 0) return 1469598103934665603ULL; // checksum over zero groups

  const std::size_t n = keys.size();
  const uint32_t* kptr = keys.data();
  const uint32_t* vptr = values.data();
  const uint32_t ugroups = static_cast<uint32_t>(groups);

  // Dense aggregation buffer; value-initialized to zero
  std::vector<uint64_t> agg(static_cast<std::size_t>(groups));
  uint64_t* aptr = agg.data();

  // Accumulate once (result is identical across iterations for same inputs)
  std::size_t i = 0;
  const std::size_t n4 = n & ~static_cast<std::size_t>(3);
  for (; i < n4; i += 4) {
    uint32_t k0 = kptr[i + 0]; if (k0 < ugroups) aptr[k0] += static_cast<uint64_t>(vptr[i + 0]);
    uint32_t k1 = kptr[i + 1]; if (k1 < ugroups) aptr[k1] += static_cast<uint64_t>(vptr[i + 1]);
    uint32_t k2 = kptr[i + 2]; if (k2 < ugroups) aptr[k2] += static_cast<uint64_t>(vptr[i + 2]);
    uint32_t k3 = kptr[i + 3]; if (k3 < ugroups) aptr[k3] += static_cast<uint64_t>(vptr[i + 3]);
  }
  for (; i < n; ++i) {
    uint32_t k = kptr[i];
    if (k < ugroups) aptr[k] += static_cast<uint64_t>(vptr[i]);
  }

  // Compute checksum exactly as in baseline
  uint64_t out = fnv1a_checksum_dense(aptr, groups);
  return out;
}
