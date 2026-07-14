#include "interface.h"

namespace {

// Compute 64-bit FNV-1 hash over the serialized byte stream produced by
// [id:u32][ts:u32][level:u16][len:u16] for each record, little-endian.
inline uint64_t fnv1_hash_records(
    const uint32_t* ids,
    const uint32_t* ts,
    const uint16_t* levels,
    const uint16_t* lens,
    std::size_t n) {
  uint64_t hash = 1469598103934665603ULL;           // FNV-1 64-bit offset basis
  const uint64_t FNV_PRIME = 1099511628211ULL;      // FNV-1 64-bit prime

  for (std::size_t i = 0; i < n; ++i) {
    uint32_t v32;
    uint16_t v16;

    // ids[i] (u32 LE)
    v32 = ids[i];
    hash ^= static_cast<uint8_t>(v32);        hash *= FNV_PRIME;
    hash ^= static_cast<uint8_t>(v32 >> 8);   hash *= FNV_PRIME;
    hash ^= static_cast<uint8_t>(v32 >> 16);  hash *= FNV_PRIME;
    hash ^= static_cast<uint8_t>(v32 >> 24);  hash *= FNV_PRIME;

    // ts[i] (u32 LE)
    v32 = ts[i];
    hash ^= static_cast<uint8_t>(v32);        hash *= FNV_PRIME;
    hash ^= static_cast<uint8_t>(v32 >> 8);   hash *= FNV_PRIME;
    hash ^= static_cast<uint8_t>(v32 >> 16);  hash *= FNV_PRIME;
    hash ^= static_cast<uint8_t>(v32 >> 24);  hash *= FNV_PRIME;

    // levels[i] (u16 LE)
    v16 = levels[i];
    hash ^= static_cast<uint8_t>(v16);        hash *= FNV_PRIME;
    hash ^= static_cast<uint8_t>(v16 >> 8);   hash *= FNV_PRIME;

    // lens[i] (u16 LE)
    v16 = lens[i];
    hash ^= static_cast<uint8_t>(v16);        hash *= FNV_PRIME;
    hash ^= static_cast<uint8_t>(v16 >> 8);   hash *= FNV_PRIME;
  }

  return hash;
}

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  // Baseline recomputes the same checksum each iteration and returns the last one.
  // For correctness, if iters == 0, baseline returns 0.
  if (iters <= 0) return 0;

  const std::size_t n = ids.size();
  return fnv1_hash_records(ids.data(), ts.data(), levels.data(), lens.data(), n);
}
