#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

static inline uint64_t checksum_bytes_ptr(const uint8_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  const uint8_t* end = data + n;
  while (data != end) {
    hash ^= static_cast<uint64_t>(*data++);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  size_t total_size = 0;
  for (const std::string& frame : frames) {
    total_size += 2u + frame.size();
  }

  std::vector<uint8_t> out(total_size);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* p = out.data();
    for (const std::string& frame : frames) {
      const uint16_t len = static_cast<uint16_t>(frame.size());
      *p++ = static_cast<uint8_t>(len);
      *p++ = static_cast<uint8_t>(len >> 8);

      const size_t n = frame.size();
      if (n != 0) {
        const char* src = frame.data();
        for (size_t i = 0; i < n; ++i) {
          p[i] = static_cast<uint8_t>(src[i]);
        }
        p += n;
      }
    }
    hash = checksum_bytes_ptr(out.data(), out.size());
  }

  return hash;
}
