#include "interface.h"

#include <cstring>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  // Pre-compute total size needed: 2 bytes length prefix per frame + frame data
  size_t total_size = 0;
  for (const std::string& frame : frames) {
    total_size += 2 + frame.size();
  }

  // Pre-allocate buffer once
  std::vector<uint8_t> out(total_size);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = out.data();

    // Pack frames with direct memory writes
    for (const std::string& frame : frames) {
      uint16_t len = static_cast<uint16_t>(frame.size());
      // Write length prefix (little-endian)
      ptr[0] = static_cast<uint8_t>(len);
      ptr[1] = static_cast<uint8_t>(len >> 8);
      ptr += 2;
      // Copy frame data
      std::memcpy(ptr, frame.data(), frame.size());
      ptr += frame.size();
    }

    // Compute FNV-1a checksum with pointer arithmetic
    hash = 1469598103934665603ULL;
    const uint8_t* p = out.data();
    const uint8_t* end = p + total_size;
    while (p != end) {
      hash ^= static_cast<uint64_t>(*p++);
      hash *= 1099511628211ULL;
    }
  }

  return hash;
}