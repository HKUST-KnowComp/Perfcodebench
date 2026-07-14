#include "interface.h"

#include <cstring>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  // Pre-compute total size needed
  size_t total_size = 0;
  for (const std::string& frame : frames) {
    total_size += 2 + frame.size();  // 2 bytes for length prefix
  }

  // Pre-allocate buffer once
  std::vector<uint8_t> out(total_size);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Pack frames into buffer
    uint8_t* ptr = out.data();
    for (const std::string& frame : frames) {
      uint16_t len = static_cast<uint16_t>(frame.size());
      ptr[0] = static_cast<uint8_t>(len);
      ptr[1] = static_cast<uint8_t>(len >> 8);
      ptr += 2;
      std::memcpy(ptr, frame.data(), frame.size());
      ptr += frame.size();
    }

    // Compute FNV-1a checksum
    constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;
    hash = FNV_OFFSET;
    
    const uint8_t* data = out.data();
    size_t len = total_size;
    size_t i = 0;
    
    // Process 8 bytes at a time with unrolling
    size_t len8 = len & ~size_t(7);
    for (; i < len8; i += 8) {
      hash ^= static_cast<uint64_t>(data[i]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(data[i + 1]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(data[i + 2]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(data[i + 3]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(data[i + 4]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(data[i + 5]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(data[i + 6]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(data[i + 7]);
      hash *= FNV_PRIME;
    }
    
    // Handle remaining bytes
    for (; i < len; ++i) {
      hash ^= static_cast<uint64_t>(data[i]);
      hash *= FNV_PRIME;
    }
  }

  return hash;
}