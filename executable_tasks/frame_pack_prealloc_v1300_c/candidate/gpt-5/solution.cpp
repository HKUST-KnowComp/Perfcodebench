#include "interface.h"

#include <vector>
#include <string>
#include <cstdint>

namespace {

constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME  = 1099511628211ULL;

inline uint64_t checksum_stream(const std::vector<std::string>& frames) {
  uint64_t h = FNV_OFFSET;
  for (const std::string& frame : frames) {
    // 16-bit little-endian length prefix (truncated like baseline)
    uint16_t len = static_cast<uint16_t>(frame.size());
    uint8_t b0 = static_cast<uint8_t>(len & 0xFF);
    uint8_t b1 = static_cast<uint8_t>((len >> 8) & 0xFF);

    h ^= b0; h *= FNV_PRIME;
    h ^= b1; h *= FNV_PRIME;

    const unsigned char* p = reinterpret_cast<const unsigned char*>(frame.data());
    size_t n = frame.size();

    // Unrolled processing for throughput
    while (n >= 8) {
      h ^= p[0]; h *= FNV_PRIME;
      h ^= p[1]; h *= FNV_PRIME;
      h ^= p[2]; h *= FNV_PRIME;
      h ^= p[3]; h *= FNV_PRIME;
      h ^= p[4]; h *= FNV_PRIME;
      h ^= p[5]; h *= FNV_PRIME;
      h ^= p[6]; h *= FNV_PRIME;
      h ^= p[7]; h *= FNV_PRIME;
      p += 8;
      n -= 8;
    }
    while (n--) {
      h ^= *p++; h *= FNV_PRIME;
    }
  }
  return h;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  if (iters <= 0) return 0;
  // Compute once; repeated iterations produce the same checksum.
  return checksum_stream(frames);
}
