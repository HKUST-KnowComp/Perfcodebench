#include "interface.h"
#include <cstdint>
#include <vector>
#include <cstring>

namespace {

inline uint64_t fnv1a_u64(uint64_t hash, uint64_t v) noexcept {
  const uint64_t prime = 1099511628211ULL;
  hash ^= (v & 0xFFU);        hash *= prime;
  hash ^= ((v >> 8) & 0xFFU); hash *= prime;
  hash ^= ((v >> 16) & 0xFFU); hash *= prime;
  hash ^= ((v >> 24) & 0xFFU); hash *= prime;
  hash ^= ((v >> 32) & 0xFFU); hash *= prime;
  hash ^= ((v >> 40) & 0xFFU); hash *= prime;
  hash ^= ((v >> 48) & 0xFFU); hash *= prime;
  hash ^= ((v >> 56) & 0xFFU); hash *= prime;
  return hash;
}

inline uint64_t fnv1a_u32(uint64_t hash, uint32_t v) noexcept {
  const uint64_t prime = 1099511628211ULL;
  hash ^= (v & 0xFFU);        hash *= prime;
  hash ^= ((v >> 8) & 0xFFU); hash *= prime;
  hash ^= ((v >> 16) & 0xFFU); hash *= prime;
  hash ^= ((v >> 24) & 0xFFU); hash *= prime;
  return hash;
}

inline uint64_t fnv1a_u16(uint64_t hash, uint16_t v) noexcept {
  const uint64_t prime = 1099511628211ULL;
  hash ^= (v & 0xFFU);        hash *= prime;
  hash ^= ((v >> 8) & 0xFFU); hash *= prime;
  return hash;
}

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  const std::size_t n = ids.size();
  uint64_t hash = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    std::size_t i = 0;
    // 4-way unroll
    for (; i + 3 < n; i += 4) {
      hash = fnv1a_u32(hash, ids[i]);
      hash = fnv1a_u32(hash, ts[i]);
      hash = fnv1a_u16(hash, levels[i]);
      hash = fnv1a_u16(hash, lens[i]);

      hash = fnv1a_u32(hash, ids[i+1]);
      hash = fnv1a_u32(hash, ts[i+1]);
      hash = fnv1a_u16(hash, levels[i+1]);
      hash = fnv1a_u16(hash, lens[i+1]);

      hash = fnv1a_u32(hash, ids[i+2]);
      hash = fnv1a_u32(hash, ts[i+2]);
      hash = fnv1a_u16(hash, levels[i+2]);
      hash = fnv1a_u16(hash, lens[i+2]);

      hash = fnv1a_u32(hash, ids[i+3]);
      hash = fnv1a_u32(hash, ts[i+3]);
      hash = fnv1a_u16(hash, levels[i+3]);
      hash = fnv1a_u16(hash, lens[i+3]);
    }
    // tail
    for (; i < n; ++i) {
      hash = fnv1a_u32(hash, ids[i]);
      hash = fnv1a_u32(hash, ts[i]);
      hash = fnv1a_u16(hash, levels[i]);
      hash = fnv1a_u16(hash, lens[i]);
    }
  }
  return hash;
}