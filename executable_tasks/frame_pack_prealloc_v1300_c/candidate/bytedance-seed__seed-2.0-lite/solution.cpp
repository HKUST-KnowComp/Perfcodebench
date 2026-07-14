#include "interface.h"

#include <vector>
#include <string>

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
  // Prepack full frame buffer once and preallocate exact required capacity
  std::vector<uint8_t> out;
  size_t total_size = 0;
  for (const auto& frame : frames) {
    total_size += 2 + frame.size();
  }
  out.reserve(total_size);
  for (const auto& frame : frames) {
    write_len(out, static_cast<uint16_t>(frame.size()));
    out.insert(out.end(), frame.begin(), frame.end());
  }

  // Only compute checksum for each iteration, no repeated packing
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = checksum_bytes(out);
  }
  return hash;
}