#include "interface.h"
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>

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
  if (iters <= 0) return 0;

  size_t total_size = frames.size() * 2;
  for (const auto& f : frames) {
    total_size += f.size();
  }

  std::vector<uint8_t> out;
  out.resize(total_size);

  uint8_t* ptr = out.data();
  for (const auto& f : frames) {
    uint16_t len = static_cast<uint16_t>(f.size());
    *ptr++ = static_cast<uint8_t>(len);
    *ptr++ = static_cast<uint8_t>(len >> 8);
    if (len > 0) {
      std::memcpy(ptr, f.data(), len);
      ptr += len;
    }
  }

  return checksum_bytes(out.data(), out.size());
}