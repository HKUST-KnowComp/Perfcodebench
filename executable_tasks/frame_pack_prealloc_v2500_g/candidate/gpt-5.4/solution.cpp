#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

static inline uint64_t fnv1a_update(uint64_t hash, uint8_t b) {
  hash ^= static_cast<uint64_t>(b);
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  std::size_t total_size = 0;
  for (const std::string& frame : frames) {
    total_size += 2u + frame.size();
  }

  std::vector<uint8_t> out;
  out.resize(total_size);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* dst = out.data();
    for (const std::string& frame : frames) {
      const uint16_t len = static_cast<uint16_t>(frame.size());
      *dst++ = static_cast<uint8_t>(len);
      *dst++ = static_cast<uint8_t>(len >> 8);

      const std::size_t n = frame.size();
      for (std::size_t i = 0; i < n; ++i) {
        *dst++ = static_cast<uint8_t>(frame[i]);
      }
    }

    uint64_t h = 1469598103934665603ULL;
    const uint8_t* p = out.data();
    const uint8_t* end = p + out.size();
    while (p != end) {
      h = fnv1a_update(h, *p++);
    }
    hash = h;
  }
  return hash;
}
