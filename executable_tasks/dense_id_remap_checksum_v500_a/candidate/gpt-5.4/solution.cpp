#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  const std::size_t n = keys.size();
  if (n == 0 || probes.empty() || iters <= 0) {
    return 0;
  }

  uint32_t max_key = 0;
  for (std::size_t i = 0; i < n; ++i) {
    if (keys[i] > max_key) max_key = keys[i];
  }

  std::vector<uint32_t> dense(static_cast<std::size_t>(max_key) + 1);
  for (std::size_t i = 0; i < n; ++i) {
    dense[keys[i]] = values[i];
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;

    const uint32_t* p = probes.data();
    const uint32_t* const end = p + probes.size();
    for (; p != end; ++p) {
      hash = mix(hash, dense[*p]);
    }
  }
  return hash;
}
