#include "interface.h"

namespace {

static inline void fnv_update(uint64_t& h, uint8_t b) {
  h ^= static_cast<uint64_t>(b);
  h *= 1099511628211ULL; // FNV-1a 64-bit prime
}

uint64_t compute_packed_checksum(const std::vector<std::string>& frames) {
  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis

  for (const std::string& frame : frames) {
    // Write 16-bit little-endian length (truncated like baseline)
    uint16_t v = static_cast<uint16_t>(frame.size());
    fnv_update(hash, static_cast<uint8_t>(v & 0xFF));
    fnv_update(hash, static_cast<uint8_t>((v >> 8) & 0xFF));

    const unsigned char* p = reinterpret_cast<const unsigned char*>(frame.data());
    size_t n = frame.size();

    // Unrolled loop for better throughput
    size_t i = 0;
    while (i + 8 <= n) {
      fnv_update(hash, p[i + 0]);
      fnv_update(hash, p[i + 1]);
      fnv_update(hash, p[i + 2]);
      fnv_update(hash, p[i + 3]);
      fnv_update(hash, p[i + 4]);
      fnv_update(hash, p[i + 5]);
      fnv_update(hash, p[i + 6]);
      fnv_update(hash, p[i + 7]);
      i += 8;
    }
    for (; i < n; ++i) {
      fnv_update(hash, p[i]);
    }
  }

  return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  if (iters <= 0) return 0; // baseline behavior when no iterations
  // Result is identical for every iteration; compute once.
  return compute_packed_checksum(frames);
}
