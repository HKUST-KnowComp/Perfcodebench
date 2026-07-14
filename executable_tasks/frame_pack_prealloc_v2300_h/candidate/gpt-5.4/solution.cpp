#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

static inline uint64_t fnv1a_update(uint64_t hash, const uint8_t* data, size_t len) {
  constexpr uint64_t kPrime = 1099511628211ULL;
  for (size_t i = 0; i < len; ++i) {
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

  std::vector<uint8_t> out(total_size);

  uint8_t* dst = out.data();
  for (const std::string& frame : frames) {
    const uint16_t len = static_cast<uint16_t>(frame.size());
    *dst++ = static_cast<uint8_t>(len);
    *dst++ = static_cast<uint8_t>(len >> 8);

    const size_t n = frame.size();
    const char* src = frame.data();
    for (size_t i = 0; i < n; ++i) {
      dst[i] = static_cast<uint8_t>(src[i]);
    }
    dst += n;
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = fnv1a_update(1469598103934665603ULL, out.data(), out.size());
  }
  return hash;
}
