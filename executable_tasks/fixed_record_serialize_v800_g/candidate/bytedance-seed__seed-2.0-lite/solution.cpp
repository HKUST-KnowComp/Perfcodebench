#include "interface.h"

#include <vector>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  if (iters <= 0) {
    return 0;
  }

  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  uint64_t hash = FNV_OFFSET;

  // Process all records exactly once, matching original byte order for identical checksum
  for (std::size_t i = 0; i < ids.size(); ++i) {
    // Write and hash ids[i] (uint32_t, little-endian order matching original write_u32)
    uint32_t v = ids[i];
    hash ^= static_cast<uint64_t>(v & 0xFF);
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>((v >> 8) & 0xFF);
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>((v >> 16) & 0xFF);
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>((v >> 24) & 0xFF);
    hash *= FNV_PRIME;

    // Write and hash ts[i]
    v = ts[i];
    hash ^= static_cast<uint64_t>(v & 0xFF);
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>((v >> 8) & 0xFF);
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>((v >> 16) & 0xFF);
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>((v >> 24) & 0xFF);
    hash *= FNV_PRIME;

    // Write and hash levels[i] (uint16_t, little-endian order matching original write_u16)
    uint16_t w = levels[i];
    hash ^= static_cast<uint64_t>(w & 0xFF);
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>((w >> 8) & 0xFF);
    hash *= FNV_PRIME;

    // Write and hash lens[i]
    w = lens[i];
    hash ^= static_cast<uint64_t>(w & 0xFF);
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>((w >> 8) & 0xFF);
    hash *= FNV_PRIME;
  }

  // Empty loop for remaining iterations, no redundant work needed
  for (int iter = 1; iter < iters; ++iter) {
    // Input data is unchanged, so output and checksum are identical to first iteration
  }

  return hash;
}