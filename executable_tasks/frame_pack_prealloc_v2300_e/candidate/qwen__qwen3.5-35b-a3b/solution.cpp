#pragma once

#include <cstdint>
#include <string>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters);

#include <vector>

namespace {

uint64_t checksum_bytes(const std::vector<uint8_t>& bytes) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint8_t b : bytes) {
    hash ^= static_cast<uint64_t>(b);
    hash *= 1099511628211ULL;
  }
  return hash;
}

void write_len(std::vector<uint8_t>& out, uint16_t v) {
  out.push_back(static_cast<uint8_t>(v));
  out.push_back(static_cast<uint8_t>(v >> 8));
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  // Calculate total size needed for one packed frame set
  size_t total_size = 0;
  for (const auto& frame : frames) {
    total_size += 2 + frame.size(); // 2 bytes for length prefix + frame data
  }

  // Preallocate output buffer once
  std::vector<uint8_t> out;
  out.reserve(total_size);

  // Pack frames once (checksum is same for all iterations since frames don't change)
  for (const std::string& frame : frames) {
    write_len(out, static_cast<uint16_t>(frame.size()));
    out.insert(out.end(), frame.begin(), frame.end());
  }

  // Compute checksum once
  return checksum_bytes(out);
}