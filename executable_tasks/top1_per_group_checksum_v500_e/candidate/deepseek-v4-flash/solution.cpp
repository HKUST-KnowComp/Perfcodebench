#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  // Allocate persistent arrays for per-group maximums and version stamps.
  std::vector<uint32_t> best(group_count, 0);
  std::vector<uint64_t> version(group_count, 0);
  uint64_t global_version = 0;

  const std::size_t n = groups.size();
  const uint32_t* gptr = groups.data();
  const uint32_t* vptr = values.data();

  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    ++global_version;
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t g = gptr[i];
      if (version[g] != global_version) {
        version[g] = global_version;
        best[g] = vptr[i];
      } else {
        uint32_t val = vptr[i];
        if (val > best[g]) best[g] = val;
      }
    }

    uint64_t hash = 1469598103934665603ULL;
    uint32_t* bptr = best.data();
    uint64_t* verptr = version.data();
    for (uint32_t g = 0; g < group_count; ++g) {
      uint64_t val = (verptr[g] == global_version) ? bptr[g] : 0;
      hash ^= val;
      hash *= 1099511628211ULL;
    }
    final_hash = hash;
  }
  return final_hash;
}
