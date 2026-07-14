#include "interface.h"

#include <vector>

namespace {

uint64_t fnv1a_hash(const std::vector<uint8_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint8_t b : data) {
    hash ^= static_cast<uint64_t>(b);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // anonymous namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  // Precompute the packed byte stream (length-prefixed frames) once.
  std::vector<uint8_t> packed;
  packed.reserve(frames.size() * 2 + 64);  // rough estimate
  for (const std::string& frame : frames) {
    uint16_t len = static_cast<uint16_t>(frame.size());
    packed.push_back(static_cast<uint8_t>(len));
    packed.push_back(static_cast<uint8_t>(len >> 8));
    packed.insert(packed.end(), frame.begin(), frame.end());
  }

  // Repeatedly compute checksum of the same precomputed packed stream.
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = fnv1a_hash(packed);
  }
  return hash;
}
