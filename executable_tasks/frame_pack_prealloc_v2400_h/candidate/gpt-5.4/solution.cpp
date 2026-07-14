#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

static inline void fnv1a_update(uint64_t& hash, uint8_t b) {
  hash ^= static_cast<uint64_t>(b);
  hash *= 1099511628211ULL;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  std::size_t total_size = 0;
  for (const std::string& frame : frames) {
    total_size += 2u + frame.size();
  }

  std::vector<uint8_t> out;
  out.resize(total_size);

  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* p = out.data();
    for (const std::string& frame : frames) {
      const uint16_t len = static_cast<uint16_t>(frame.size());
      *p++ = static_cast<uint8_t>(len);
      *p++ = static_cast<uint8_t>(len >> 8);

      const std::size_t n = frame.size();
      for (std::size_t i = 0; i < n; ++i) {
        *p++ = static_cast<uint8_t>(frame[i]);
      }
    }
  }

  uint64_t hash = 1469598103934665603ULL;
  for (uint8_t b : out) {
    fnv1a_update(hash, b);
  }
  return hash;
}
