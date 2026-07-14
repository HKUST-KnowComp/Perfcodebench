#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

constexpr uint64_t kFnvOffset = 1469598103934665603ULL;
constexpr uint64_t kFnvPrime = 1099511628211ULL;

// Inline helper to hash a single byte.
inline void hash_byte(uint64_t& h, uint8_t b) noexcept {
  h ^= static_cast<uint64_t>(b);
  h *= kFnvPrime;
}

}  // anonymous namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  // Handle the degenerate case where iters is 0.
  if (iters <= 0) {
    return 0ULL;
  }

  // Compute the checksum of the packed stream once; it is identical every iteration.
  // FNV-1a hash over the concatenation of (little-endian 16-bit length, frame bytes).
  uint64_t hash = kFnvOffset;
  for (const std::string& frame : frames) {
    const uint16_t len = static_cast<uint16_t>(frame.size());
    hash_byte(hash, static_cast<uint8_t>(len & 0xFF));
    hash_byte(hash, static_cast<uint8_t>((len >> 8) & 0xFF));
    // Hash frame bytes.
    const char* data = frame.data();
    for (size_t i = 0; i < len; ++i) {
      hash_byte(hash, static_cast<uint8_t>(data[i]));
    }
  }
  return hash;
}
