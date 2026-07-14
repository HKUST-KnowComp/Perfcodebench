#include "interface.h"
#include "simdutf.h"

#include <cstdint>
#include <cstddef>
#include <limits>
#include <string>

uint64_t transcode_checksum(const std::u16string& input) {
  // FNV-1a 64-bit
  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  const char16_t* src = input.data();
  const size_t len16 = input.size();

  uint64_t hash = FNV_OFFSET_BASIS;
  if (len16 == 0) {
    return hash;
  }

  // Allocate a safe upper bound for UTF-8 output: 3 bytes per UTF-16 code unit.
  // For extremely large inputs where multiplication might overflow, fall back to exact length computation.
  size_t capacity;
  if (len16 <= std::numeric_limits<size_t>::max() / 3) {
    capacity = len16 * 3;
  } else {
    capacity = simdutf::utf16_to_utf8_length(src, len16);
  }

  std::string out;
  out.resize(capacity);

  size_t written = simdutf::utf16_to_utf8(src, len16, out.data());

  const unsigned char* bytes = reinterpret_cast<const unsigned char*>(out.data());
  for (size_t i = 0; i < written; ++i) {
    hash ^= static_cast<uint64_t>(bytes[i]);
    hash *= FNV_PRIME;
  }
  return hash;
}
