#include "interface.h"

#include <vector>
#include <cstring>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  size_t total_size = 0;
  for (const auto& f : frames) {
    total_size += 2 + f.size();
  }

  std::vector<uint8_t> out(total_size);
  uint8_t* ptr = out.data();
  for (const auto& frame : frames) {
    uint16_t len = static_cast<uint16_t>(frame.size());
    ptr[0] = static_cast<uint8_t>(len);
    ptr[1] = static_cast<uint8_t>(len >> 8);
    ptr += 2;
    std::memcpy(ptr, frame.data(), frame.size());
    ptr += frame.size();
  }

  uint64_t hash = 0;
  const uint8_t* data = out.data();
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    for (size_t i = 0; i < total_size; ++i) {
      h ^= data[i];
      h *= 1099511628211ULL;
    }
    hash = h;
  }
  return hash;
}
