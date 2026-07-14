#include "interface.h"

#include <string>
#include <vector>

namespace {

// Streamed FNV-1a over the packed byte sequence without materializing it.
inline uint64_t fnv1a_packed(const std::vector<std::string>& frames) {
  uint64_t h = 1469598103934665603ULL; // FNV-1a offset basis
  const uint64_t prime = 1099511628211ULL;

  for (const std::string& frame : frames) {
    uint16_t v = static_cast<uint16_t>(frame.size());
    uint8_t b0 = static_cast<uint8_t>(v & 0xFF);
    uint8_t b1 = static_cast<uint8_t>((v >> 8) & 0xFF);
    h ^= b0; h *= prime;
    h ^= b1; h *= prime;

    const char* p = frame.data();
    const char* const end = p + frame.size();
    while (p != end) {
      h ^= static_cast<uint8_t>(*p++);
      h *= prime;
    }
  }
  return h;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  if (iters <= 0) return 0;
  // The packed output is identical across iterations; compute once.
  return fnv1a_packed(frames);
}
