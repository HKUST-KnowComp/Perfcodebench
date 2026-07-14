#include "interface.h"

#include <cstdint>
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
  uint8_t* const base = out.data();

  uint64_t hash = 0;
  constexpr uint64_t FNV_PRIME = 1099511628211ULL;
  constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    // Pack frames into buffer
    uint8_t* ptr = base;
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

    // Compute FNV-1a checksum with unrolling
    hash = FNV_OFFSET;
    const uint8_t* p = base;
    const uint8_t* end = base + total_size;
    const uint8_t* end8 = base + (total_size & ~size_t(7));

    while (p < end8) {
      hash ^= static_cast<uint64_t>(p[0]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(p[1]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(p[2]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(p[3]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(p[4]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(p[5]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(p[6]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(p[7]);
      hash *= FNV_PRIME;
      p += 8;
    }

    while (p < end) {
      hash ^= static_cast<uint64_t>(*p++);
      hash *= FNV_PRIME;
    }
  }

  return hash;
}
