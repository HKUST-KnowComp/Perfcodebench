#include "interface.h"

#include <cstddef>
#include <cstdint>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int /*iters*/) {
  const std::size_t N = ids.size();
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < N; ++i) {
    // write id (little-endian 4 bytes)
    uint32_t v32 = ids[i];
    hash ^= static_cast<uint64_t>(v32 & 0xFFu);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>((v32 >> 8) & 0xFFu);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>((v32 >> 16) & 0xFFu);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>((v32 >> 24) & 0xFFu);
    hash *= 1099511628211ULL;

    // write ts (little-endian 4 bytes)
    v32 = ts[i];
    hash ^= static_cast<uint64_t>(v32 & 0xFFu);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>((v32 >> 8) & 0xFFu);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>((v32 >> 16) & 0xFFu);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>((v32 >> 24) & 0xFFu);
    hash *= 1099511628211ULL;

    // write level (little-endian 2 bytes)
    uint16_t v16 = levels[i];
    hash ^= static_cast<uint64_t>(v16 & 0xFFu);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>((v16 >> 8) & 0xFFu);
    hash *= 1099511628211ULL;

    // write lens (little-endian 2 bytes)
    v16 = lens[i];
    hash ^= static_cast<uint64_t>(v16 & 0xFFu);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>((v16 >> 8) & 0xFFu);
    hash *= 1099511628211ULL;
  }
  return hash;
}
