#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstring>

namespace {

// FNV-1a 64-bit constants
constexpr uint64_t FNV_OFFSET = 14695981039346656037ULL;
constexpr uint64_t FNV_PRIME  = 1099511628211ULL;

inline void store_u32_le(uint8_t* p, uint32_t v) {
  std::memcpy(p, &v, 4);
}
inline void store_u16_le(uint8_t* p, uint16_t v) {
  std::memcpy(p, &v, 2);
}

inline uint64_t update_hash(uint64_t h, const uint8_t* bytes, size_t n) {
  for (size_t i = 0; i < n; ++i) {
    h ^= static_cast<uint64_t>(bytes[i]);
    h *= FNV_PRIME;
  }
  return h;
}

} // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {

  const size_t n = ids.size();
  const size_t record_size = 12; // 4+4+2+2
  const size_t total_size = n * record_size;

  std::vector<uint8_t> buffer;
  buffer.resize(total_size);

  uint8_t* const base = buffer.data();

  uint64_t hash = FNV_OFFSET;
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* dst = base;
    for (size_t i = 0; i < n; ++i) {
      store_u32_le(dst,      ids[i]);
      store_u32_le(dst + 4,  ts[i]);
      store_u16_le(dst + 8,  levels[i]);
      store_u16_le(dst + 10, lens[i]);
      dst += record_size;
    }
    hash = update_hash(FNV_OFFSET, base, total_size);
  }
  return hash;
}