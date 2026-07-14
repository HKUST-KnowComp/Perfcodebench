#include "interface.h"
#include <vector>
#include <cstring>

namespace {

uint64_t checksum_bytes(const uint8_t* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const uint8_t* end = data + len;
  while (data < end) {
    hash ^= *data++;
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  size_t total_len = 0;
  for (const auto& f : frames) {
    total_len += f.size() + 2;
  }

  std::vector<uint8_t> out;
  out.resize(total_len);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = out.data();
    for (const auto& frame : frames) {
      uint16_t len = static_cast<uint16_t>(frame.size());
      ptr[0] = static_cast<uint8_t>(len);
      ptr[1] = static_cast<uint8_t>(len >> 8);
      ptr += 2;
      std::memcpy(ptr, frame.data(), len);
      ptr += len;
    }
    hash = checksum_bytes(out.data(), total_len);
  }
  return hash;
}
