#include "interface.h"

namespace {
constexpr uint64_t kMultiplier = 1099511628211ULL;
constexpr uint64_t kInitialHash = 1469598103934665603ULL;
} // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  if (iters <= 0) return 0;

  const std::size_t n = values.size();
  const uint8_t* f = flags.data();
  const uint32_t* v = values.data();
  uint64_t h = kInitialHash;

  // true values first (stable order)
  for (std::size_t i = 0; i < n; ++i) {
    if (f[i]) {
      h ^= v[i];
      h *= kMultiplier;
    }
  }

  // false values next (stable order)
  for (std::size_t i = 0; i < n; ++i) {
    if (!f[i]) {
      h ^= v[i];
      h *= kMultiplier;
    }
  }

  return h;
}