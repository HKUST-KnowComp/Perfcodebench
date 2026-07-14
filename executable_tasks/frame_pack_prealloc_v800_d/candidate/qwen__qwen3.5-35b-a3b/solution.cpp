#pragma once

#include <cstdint>
#include <string>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  uint64_t hash = 0;
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET;
    for (const std::string& frame : frames) {
      uint16_t len = static_cast<uint16_t>(frame.size());
      // Process length bytes (little-endian)
      hash ^= static_cast<uint64_t>(len & 0xFF);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>((len >> 8) & 0xFF);
      hash *= FNV_PRIME;
      // Process frame bytes
      for (char c : frame) {
        hash ^= static_cast<uint64_t>(static_cast<uint8_t>(c));
        hash *= FNV_PRIME;
      }
    }
  }
  return hash;
}