#include "interface.h"
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>

namespace {
uint64_t checksum_bytes(const std::vector<uint8_t>& bytes) {
  uint64_t hash = 1469598103934665603ULL;
  const uint8_t* p = bytes.data();
  const uint8_t* end = p + bytes.size();
  while (p != end) {
    hash ^= static_cast<uint64_t>(*p++);
    hash *= 1099511628211ULL;
  }
  return hash;
}
}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  size_t total_size = 0;
  for (const auto& f : frames) {
    total_size += f.size() + 2;
  }

  std::vector<uint8_t> out;
  out.resize(total_size);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    size_t pos = 0;
    for (const auto& frame : frames) {
      uint16_t len = static_cast<uint16_t>(frame.size());
      out[pos++] = static_cast<uint8_t>(len);
      out[pos++] = static_cast<uint8_t>(len >> 8);
      if (len > 0) {
        std::memcpy(&out[pos], frame.data(), len);
        pos += len;
      }
    }
    hash = checksum_bytes(out);
  }
  return hash;
}