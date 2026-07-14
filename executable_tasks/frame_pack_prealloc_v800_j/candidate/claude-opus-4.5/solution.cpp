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
  uint8_t* const out_data = out.data();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = out_data;
    
    // Pack all frames
    for (const std::string& frame : frames) {
      uint16_t len = static_cast<uint16_t>(frame.size());
      // Write length prefix (little-endian)
      ptr[0] = static_cast<uint8_t>(len);
      ptr[1] = static_cast<uint8_t>(len >> 8);
      ptr += 2;
      // Copy frame data
      std::memcpy(ptr, frame.data(), len);
      ptr += len;
    }

    // Compute FNV-1a checksum inline
    hash = 1469598103934665603ULL;
    const uint8_t* p = out_data;
    const uint8_t* const end = out_data + total_size;
    
    // Process 8 bytes at a time for better throughput
    while (p + 8 <= end) {
      hash ^= static_cast<uint64_t>(p[0]);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(p[1]);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(p[2]);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(p[3]);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(p[4]);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(p[5]);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(p[6]);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(p[7]);
      hash *= 1099511628211ULL;
      p += 8;
    }
    
    // Handle remaining bytes
    while (p < end) {
      hash ^= static_cast<uint64_t>(*p++);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}