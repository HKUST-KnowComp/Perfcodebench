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
  // Precompute total required size to avoid any vector reallocations
  size_t total_packed_size = 0;
  for (const std::string& frame : frames) {
    total_packed_size += 2 + frame.size();
  }

  // Prepack all frames exactly once into a preallocated buffer
  std::vector<uint8_t> packed;
  packed.reserve(total_packed_size);
  for (const std::string& frame : frames) {
    write_len(packed, static_cast<uint16_t>(frame.size()));
    packed.insert(packed.end(), frame.begin(), frame.end());
  }

  // Only run checksum repeatedly, no redundant packing work per iteration
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = checksum_bytes(packed);
  }
  return hash;
}