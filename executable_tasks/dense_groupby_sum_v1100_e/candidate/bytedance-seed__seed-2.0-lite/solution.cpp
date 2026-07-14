#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

struct AccumEntry {
  int last_generation = -1;
  uint64_t sum = 0;
};

uint64_t compute_checksum(const std::vector<AccumEntry>& agg, int groups, int current_generation) {
  uint64_t hash = 1469598103934665603ULL;
  for (int g = 0; g < groups; ++g) {
    const uint64_t v = (agg[g].last_generation == current_generation) ? agg[g].sum : 0;
    hash ^= v;
    hash *= 1099511628211ULL;
  }
  return hash;
}

} // namespace

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
  std::vector<AccumEntry> agg(groups);
  uint64_t out = 0;
  const std::size_t num_elements = keys.size();

  for (int iter = 0; iter < iters; ++iter) {
    for (std::size_t i = 0; i < num_elements; ++i) {
      const uint32_t key = keys[i];
      AccumEntry& entry = agg[key];
      if (entry.last_generation != iter) {
        entry.last_generation = iter;
        entry.sum = 0;
      }
      entry.sum += static_cast<uint64_t>(values[i]);
    }
    out = compute_checksum(agg, groups, iter);
  }

  return out;
}