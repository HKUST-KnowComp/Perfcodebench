#include "interface.h"

namespace {

constexpr uint64_t kFNV64Offset = 1469598103934665603ULL;
constexpr uint64_t kFNV64Prime  = 1099511628211ULL;

inline uint64_t fnv1a_update_byte(uint64_t h, uint8_t b) {
  h ^= static_cast<uint64_t>(b);
  h *= kFNV64Prime;
  return h;
}

inline uint64_t fnv1a_update(uint64_t h, const uint8_t* p, size_t n) {
  const uint8_t* end_unrolled = p + (n & ~static_cast<size_t>(7));
  while (p < end_unrolled) {
    h ^= *p++; h *= kFNV64Prime;
    h ^= *p++; h *= kFNV64Prime;
    h ^= *p++; h *= kFNV64Prime;
    h ^= *p++; h *= kFNV64Prime;
    h ^= *p++; h *= kFNV64Prime;
    h ^= *p++; h *= kFNV64Prime;
    h ^= *p++; h *= kFNV64Prime;
    h ^= *p++; h *= kFNV64Prime;
  }
  const uint8_t* end = p + (n & static_cast<size_t>(7));
  while (p < end) {
    h ^= *p++;
    h *= kFNV64Prime;
  }
  return h;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  if (iters <= 0) return 0ULL;

  uint64_t hash = kFNV64Offset;
  for (const std::string& frame : frames) {
    uint16_t len = static_cast<uint16_t>(frame.size());
    // Little-endian length prefix
    hash = fnv1a_update_byte(hash, static_cast<uint8_t>(len & 0xFF));
    hash = fnv1a_update_byte(hash, static_cast<uint8_t>((len >> 8) & 0xFF));

    // Frame bytes
    const uint8_t* data = reinterpret_cast<const uint8_t*>(frame.data());
    hash = fnv1a_update(hash, data, frame.size());
  }

  return hash;
}
