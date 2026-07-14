#include "interface.h"
#include <cstring>
#include <vector>

namespace {

uint64_t checksum_bytes(const uint8_t* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
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

  std::vector<uint8_t> out(total_size);
  uint8_t* buf = out.data();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* p = buf;
    for (const auto& f : frames) {
      uint16_t len = static_cast<uint16_t>(f.size());
      p[0] = static_cast<uint8_t>(len);
      p[1] = static_cast<uint8_t>(len >> 8);
      p += 2;
      std::memcpy(p, f.data(), len);
      p += len;
    }
    hash = checksum_bytes(buf, total_size);
  }
  return hash;
}
