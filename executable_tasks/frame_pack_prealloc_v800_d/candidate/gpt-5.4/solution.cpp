#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

namespace {

static inline uint64_t checksum_bytes(const uint8_t* data, size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < size; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  size_t packed_size = 0;
  for (const std::string& frame : frames) {
    packed_size += 2 + frame.size();
  }

  std::vector<uint8_t> out;
  out.resize(packed_size);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* p = out.data();
    for (const std::string& frame : frames) {
      const uint16_t len = static_cast<uint16_t>(frame.size());
      p[0] = static_cast<uint8_t>(len);
      p[1] = static_cast<uint8_t>(len >> 8);
      p += 2;

      const size_t n = frame.size();
      if (n != 0) {
        std::memcpy(p, frame.data(), n);
        p += n;
      }
    }
    hash = checksum_bytes(out.data(), out.size());
  }
  return hash;
}
