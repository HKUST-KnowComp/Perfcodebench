#include "interface.h"

#include <cstring>
#include <vector>

namespace {

uint64_t checksum_bytes(const std::vector<uint8_t>& bytes) {
  uint64_t hash = 1469598103934665603ULL;
  const uint8_t* data = bytes.data();
  size_t n = bytes.size();
  for (size_t i = 0; i < n; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  size_t total_size = 0;
  for (const auto& f : frames) {
    total_size += 2 + f.size();
  }
  std::vector<uint8_t> packed(total_size);
  uint8_t* ptr = packed.data();
  for (const auto& f : frames) {
    uint16_t len = static_cast<uint16_t>(f.size());
    *ptr++ = static_cast<uint8_t>(len);
    *ptr++ = static_cast<uint8_t>(len >> 8);
    std::memcpy(ptr, f.data(), f.size());
    ptr += f.size();
  }
  uint64_t hash = 0;
  for (int i = 0; i < iters; ++i) {
    hash = checksum_bytes(packed);
  }
  return hash;
}