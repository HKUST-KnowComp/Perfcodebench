#include "interface.h"

#include <cstdint>
#include <cstring>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  if (iters <= 0) {
    return 1469598103934665603ULL;
  }

  // Calculate total size needed
  size_t total_size = 0;
  for (const std::string& frame : frames) {
    total_size += 2 + frame.size();  // 2 bytes for length prefix
  }

  // Pre-allocate buffer
  std::vector<uint8_t> out(total_size);
  uint8_t* ptr = out.data();

  // Pack frames with length prefixes
  for (const std::string& frame : frames) {
    uint16_t len = static_cast<uint16_t>(frame.size());
    ptr[0] = static_cast<uint8_t>(len);
    ptr[1] = static_cast<uint8_t>(len >> 8);
    ptr += 2;
    if (!frame.empty()) {
      std::memcpy(ptr, frame.data(), frame.size());
      ptr += frame.size();
    }
  }

  // Compute checksum for all iterations
  uint64_t hash = 0;
  const uint8_t* data = out.data();
  const size_t size = out.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < size; ++i) {
      hash ^= static_cast<uint64_t>(data[i]);
      hash *= 1099511628211ULL;
    }
  }

  return hash;
}