#include "interface.h"

#include <cstring>

namespace {

// FNV-1a 64-bit offset basis and prime
constexpr uint64_t kOffsetBasis = 1469598103934665603ULL;
constexpr uint64_t kPrime = 1099511628211ULL;

}  // namespace

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  // Dense groups: assume groups[i] < group_count
  std::vector<uint32_t> best(group_count, 0);
  uint64_t hash = 0;
  const size_t n = groups.size();
  for (int iter = 0; iter < iters; ++iter) {
    // Reset per-group max
    std::fill(best.data(), best.data() + group_count, 0);

    // Compute max per group
    const uint32_t* g_ptr = groups.data();
    const uint32_t* v_ptr = values.data();
    for (size_t i = 0; i < n; ++i) {
      uint32_t g = g_ptr[i];
      uint32_t v = v_ptr[i];
      if (v > best[g]) {
        best[g] = v;
      }
    }

    // Compute FNV-1a hash over all groups
    uint64_t h = kOffsetBasis;
    const uint32_t* b_ptr = best.data();
    for (uint32_t g = 0; g < group_count; ++g) {
      h ^= static_cast<uint64_t>(b_ptr[g]);
      h *= kPrime;
    }
    hash = h;
  }
  return hash;
}
