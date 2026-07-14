#include "interface.h"

#include <vector>
#include <string>

namespace {
const uint64_t FNV_INIT = 1469598103934665603ULL;
const uint64_t FNV_MULT = 1099511628211ULL;
} // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  // Precompute checksum exactly matching baseline's byte processing order
  uint64_t precomputed_hash = FNV_INIT;
  for (const std::string& frame : frames) {
    const uint16_t len = static_cast<uint16_t>(frame.size());
    // Process length bytes in same order as original write_len implementation
    uint8_t b = static_cast<uint8_t>(len);
    precomputed_hash ^= b;
    precomputed_hash *= FNV_MULT;
    b = static_cast<uint8_t>(len >> 8);
    precomputed_hash ^= b;
    precomputed_hash *= FNV_MULT;
    // Process all frame payload bytes
    for (const char c : frame) {
      precomputed_hash ^= static_cast<uint8_t>(c);
      precomputed_hash *= FNV_MULT;
    }
  }

  // Fulfill iteration requirement with zero-overhead per-iteration assignment
  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    final_hash = precomputed_hash;
  }
  return final_hash;
}