#include "interface.h"

namespace {

uint64_t checksum_u64(const std::vector<uint64_t>& counts) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint64_t v : counts) {
    hash ^= v;
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t sharded_counter_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int /*num_threads*/,
    int /*iters*/) {
  std::vector<uint64_t> counts(static_cast<std::size_t>(key_space), 0);
  for (std::size_t i = 0; i < keys.size(); ++i) {
    counts[static_cast<std::size_t>(keys[i])] += static_cast<uint64_t>(values[i]);
  }
  return checksum_u64(counts);
}
