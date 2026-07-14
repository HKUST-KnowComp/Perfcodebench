#include "interface.h"

#include <cstdint>
#include <cstring>
#include <vector>
#include <string>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  // Calculate total size needed
  size_t total_size = 0;
  for (const std::string& frame : frames) {
    total_size += 2 + frame.size();  // 2 bytes for length prefix
  }

  // Pre-allocate buffer once
  std::vector<uint8_t> out(total_size);
  uint8_t* const out_data = out.data();

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Compute checksum inline while packing
    uint64_t h = 1469598103934665603ULL;
    uint8_t* ptr = out_data;

    for (const std::string& frame : frames) {
      uint16_t len = static_cast<uint16_t>(frame.size());
      uint8_t lo = static_cast<uint8_t>(len);
      uint8_t hi = static_cast<uint8_t>(len >> 8);

      // Write length prefix
      *ptr++ = lo;
      *ptr++ = hi;

      // Update hash for length bytes
      h ^= static_cast<uint64_t>(lo);
      h *= 1099511628211ULL;
      h ^= static_cast<uint64_t>(hi);
      h *= 1099511628211ULL;

      // Copy frame data and update hash
      const char* src = frame.data();
      size_t sz = frame.size();
      
      // Process frame bytes
      for (size_t i = 0; i < sz; ++i) {
        uint8_t b = static_cast<uint8_t>(src[i]);
        *ptr++ = b;
        h ^= static_cast<uint64_t>(b);
        h *= 1099511628211ULL;
      }
    }

    hash = h;
  }

  return hash;
}