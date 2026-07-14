#include "interface.h"
#include <cstdint>
#include <cstring>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t partition_scatter_checksum(const std::vector<uint32_t>& keys, uint32_t groups, int iters) {
  std::size_t n = keys.size();
  std::vector<uint32_t> counts(groups, 0);
  std::vector<uint32_t> offsets(groups + 1, 0);
  std::vector<uint32_t> rows(n);

  uint64_t hash = 0;
  const uint32_t* keys_data = keys.data();
  uint32_t* counts_data = counts.data();
  uint32_t* offsets_data = offsets.data();
  uint32_t* rows_data = rows.data();

  for (int iter = 0; iter < iters; ++iter) {
    std::memset(counts_data, 0, groups * sizeof(uint32_t));
    for (std::size_t i = 0; i < n; ++i) {
      counts_data[keys_data[i]]++;
    }

    offsets_data[0] = 0;
    for (uint32_t g = 0; g < groups; ++g) {
      offsets_data[g + 1] = offsets_data[g] + counts_data[g];
    }

    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i <= groups; ++i) {
      hash = mix(hash, offsets_data[i]);
    }

    for (std::size_t i = 0; i < n; ++i) {
      uint32_t g = keys_data[i];
      rows_data[offsets_data[g]++] = static_cast<uint32_t>(i);
    }

    for (std::size_t i = 0; i < n; ++i) {
      hash = mix(hash, rows_data[i]);
    }
  }
  return hash;
}
