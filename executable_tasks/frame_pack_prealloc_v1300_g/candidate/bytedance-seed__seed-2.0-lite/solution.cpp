#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace {

uint64_t checksum_bytes(const uint8_t* bytes, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(bytes[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

} // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  if (iters <= 0) return 0;

  // Precompute total size of the packed buffer to avoid repeated allocations
  size_t total_size = 0;
  for (const auto& frame : frames) {
    total_size += 2 + frame.size();
  }
  if (total_size == 0) return 0;

  // Build the packed byte stream exactly once, since input frames are immutable
  std::vector<uint8_t> packed(total_size);
  uint8_t* write_ptr = packed.data();
  for (const auto& frame : frames) {
    const uint16_t frame_len = static_cast<uint16_t>(frame.size());
    // Match original little-endian length writing logic
    *write_ptr++ = static_cast<uint8_t>(frame_len);
    *write_ptr++ = static_cast<uint8_t>(frame_len >> 8);
    // Copy frame payload into preallocated buffer
    std::copy(frame.begin(), frame.end(), write_ptr);
    write_ptr += frame.size();
  }

  // Run checksum for all iterations to match original loop count, preserve timing profile
  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    final_hash = checksum_bytes(packed.data(), packed.size());
  }
  return final_hash;
}