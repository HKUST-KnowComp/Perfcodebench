#include "interface.h"

#include <vector>
#include <string>

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
  // Precompute total required buffer size to eliminate dynamic reallocations
  size_t total_size = 0;
  for (const auto& frame : frames) {
    total_size += 2 + frame.size();
  }

  // Pack frames once into a preallocated buffer (run once, not per iteration)
  std::vector<uint8_t> packed;
  packed.reserve(total_size);
  for (const auto& frame : frames) {
    const uint16_t frame_len = static_cast<uint16_t>(frame.size());
    packed.push_back(static_cast<uint8_t>(frame_len));
    packed.push_back(static_cast<uint8_t>(frame_len >> 8));
    packed.insert(packed.end(), frame.begin(), frame.end());
  }

  // Execute checksum across all required iterations, reusing the static packed buffer
  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    final_hash = checksum_bytes(packed.data(), packed.size());
  }
  return final_hash;
}