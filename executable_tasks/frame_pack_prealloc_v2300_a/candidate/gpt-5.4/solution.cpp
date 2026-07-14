#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

inline void fnv1a_byte(uint64_t& hash, uint8_t b) {
  hash ^= static_cast<uint64_t>(b);
  hash *= 1099511628211ULL;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  const std::size_t n = frames.size();

  std::vector<const char*> data_ptrs;
  std::vector<uint16_t> lens;
  data_ptrs.reserve(n);
  lens.reserve(n);

  for (const std::string& frame : frames) {
    data_ptrs.push_back(frame.data());
    lens.push_back(static_cast<uint16_t>(frame.size()));
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    for (std::size_t i = 0; i < n; ++i) {
      const uint16_t len = lens[i];
      fnv1a_byte(h, static_cast<uint8_t>(len));
      fnv1a_byte(h, static_cast<uint8_t>(len >> 8));

      const char* p = data_ptrs[i];
      for (uint16_t j = 0; j < len; ++j) {
        fnv1a_byte(h, static_cast<uint8_t>(p[j]));
      }
    }
    hash = h;
  }
  return hash;
}
