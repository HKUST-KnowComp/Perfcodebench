#include "interface.h"
#include <vector>
#include <cstring>

namespace {

inline uint64_t compute_hash(const uint8_t* data, size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < size; ++i) {
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

  std::vector<uint8_t> out(total_size);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = out.data();
    for (const std::string& frame : frames) {
      uint16_t len = static_cast<uint16_t>(frame.size());
      ptr[0] = static_cast<uint8_t>(len);
      ptr[1] = static_cast<uint8_t>(len >> 8);
      std::memcpy(ptr + 2, frame.data(), frame.size());
      ptr += 2 + frame.size();
    }
    hash = compute_hash(out.data(), total_size);
  }
  return hash;
}