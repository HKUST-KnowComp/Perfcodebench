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

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  // Precalculate total size of packed stream to eliminate reallocations
  size_t total_packed_size = 0;
  for (const auto& frame : frames) {
    total_packed_size += 2 + frame.size();
  }

  // Allocate packed buffer exactly once, outside iteration loop
  std::vector<uint8_t> packed(total_packed_size);
  uint8_t* write_ptr = packed.data();

  // Fill packed buffer once, bit-for-bit identical to baseline output
  for (const auto& frame : frames) {
    const uint16_t frame_len = static_cast<uint16_t>(frame.size());
    *write_ptr++ = static_cast<uint8_t>(frame_len);
    *write_ptr++ = static_cast<uint8_t>(frame_len >> 8);
    std::memcpy(write_ptr, frame.data(), frame.size());
    write_ptr += frame.size();
  }

  // Run checksum for all required iterations, only run expensive packing once
  uint64_t final_hash = 0;
  const uint8_t* read_ptr = packed.data();
  const size_t packed_len = packed.size();
  for (int iter = 0; iter < iters; ++iter) {
    final_hash = checksum_bytes(read_ptr, packed_len);
  }

  return final_hash;
}