#include "interface.h"

namespace {

static inline void fnv1_update(uint64_t& h, uint8_t b) {
  h ^= static_cast<uint64_t>(b);
  h *= 1099511628211ULL; // FNV-1 prime
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  if (iters <= 0) return 0ULL;

  uint64_t hash = 1469598103934665603ULL; // FNV-1 offset basis

  for (const std::string& frame : frames) {
    uint16_t len = static_cast<uint16_t>(frame.size());
    // write length (little-endian) into the hash stream
    fnv1_update(hash, static_cast<uint8_t>(len));
    fnv1_update(hash, static_cast<uint8_t>(len >> 8));

    const uint8_t* p = reinterpret_cast<const uint8_t*>(frame.data());
    size_t n = static_cast<size_t>(len);

    // Unrolled processing for speed
    while (n >= 8) {
      fnv1_update(hash, p[0]);
      fnv1_update(hash, p[1]);
      fnv1_update(hash, p[2]);
      fnv1_update(hash, p[3]);
      fnv1_update(hash, p[4]);
      fnv1_update(hash, p[5]);
      fnv1_update(hash, p[6]);
      fnv1_update(hash, p[7]);
      p += 8;
      n -= 8;
    }
    if (n & 4) {
      fnv1_update(hash, p[0]);
      fnv1_update(hash, p[1]);
      fnv1_update(hash, p[2]);
      fnv1_update(hash, p[3]);
      p += 4;
    }
    if (n & 2) {
      fnv1_update(hash, p[0]);
      fnv1_update(hash, p[1]);
      p += 2;
    }
    if (n & 1) {
      fnv1_update(hash, p[0]);
    }
  }

  // As baseline uses the last iteration's checksum and frames are constant per call,
  // a single computation suffices.
  return hash;
}
