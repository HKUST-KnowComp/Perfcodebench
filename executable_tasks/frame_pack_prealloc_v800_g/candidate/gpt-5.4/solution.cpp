#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

static inline uint64_t fnv1a_update(uint64_t hash, const uint8_t* data, size_t n) {
  constexpr uint64_t kPrime = 1099511628211ULL;
  for (size_t i = 0; i < n; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= kPrime;
  }
  return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  size_t total_size = 0;
  for (const std::string& frame : frames) {
    total_size += 2u + frame.size();
  }

  std::vector<uint8_t> out;
  out.resize(total_size);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* p = out.data();
    for (const std::string& frame : frames) {
      const uint16_t len = static_cast<uint16_t>(frame.size());
      p[0] = static_cast<uint8_t>(len);
      p[1] = static_cast<uint8_t>(len >> 8);
      p += 2;

      const size_t n = frame.size();
      const char* src = frame.data();
      for (size_t i = 0; i < n; ++i) {
        p[i] = static_cast<uint8_t>(src[i]);
      }
      p += n;
    }

    uint64_t h = 1469598103934665603ULL;
    h = fnv1a_update(h, out.data(), out.size());
    hash = h;
  }
  return hash;
}
