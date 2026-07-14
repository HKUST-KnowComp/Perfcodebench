#include "interface.h"

#include <vector>
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

} // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  // Precalculate total buffer size once to avoid repeated allocations
  size_t total_size = 0;
  for (const auto& frame : frames) {
    total_size += 2 + frame.size();
  }
  
  // Preallocate output buffer once for all iterations
  std::vector<uint8_t> out(total_size);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* write_ptr = out.data();
    for (const auto& frame : frames) {
      const uint16_t frame_len = static_cast<uint16_t>(frame.size());
      // Write little-endian length matching original implementation
      *write_ptr++ = static_cast<uint8_t>(frame_len);
      *write_ptr++ = static_cast<uint8_t>(frame_len >> 8);
      // Copy frame data directly with optimized memcpy
      memcpy(write_ptr, frame.data(), frame_len);
      write_ptr += frame_len;
    }
    final_hash = checksum_bytes(out.data(), total_size);
  }
  return final_hash;
}