#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>

namespace {

// FNV-1a hash implementation to match the original checksum logic
uint64_t checksum_bytes(const std::string& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  if (input.empty()) {
    return checksum_bytes("");
  }

  // simdutf::convert::utf16to8 is highly optimized using SIMD instructions.
  // It handles surrogate pairs and produces a std::string.
  // We use the version that returns a std::string for convenience and correctness.
  std::string utf8_output = simdutf::convert::utf16to8(reinterpret_cast<const char16_t*>(input.data()), input.size());

  return checksum_bytes(utf8_output);
}
