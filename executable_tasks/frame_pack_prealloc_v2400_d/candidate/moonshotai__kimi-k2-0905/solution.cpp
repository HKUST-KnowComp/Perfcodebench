#include "interface.h"

#include <vector>
#include <cstring>

namespace {

uint64_t checksum_bytes(const std::vector<uint8_t>& bytes) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint8_t b : bytes) {
    hash ^= static_cast<uint64_t>(b);
    hash *= 1099511628211ULL;
  }
  return hash;
}

inline void write_len(uint8_t* dst, uint16_t v) {
  dst[0] = static_cast<uint8_t>(v);
  dst[1] = static_cast<uint8_t>(v >> 8);
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  // compute total size once
  size_t total = 0;
  for (const auto& f : frames) total += 2 + f.size();

  std::vector<uint8_t> out;
  out.reserve(total);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    out.resize(total);          // reuse buffer, set exact size
    uint8_t* ptr = out.data();

    for (const auto& frame : frames) {
      uint16_t len = static_cast<uint16_t>(frame.size());
      write_len(ptr, len);
      ptr += 2;
      std::memcpy(ptr, frame.data(), len);
      ptr += len;
    }

    hash = checksum_bytes(out);
  }
  return hash;
}