#include "interface.h"
#include "simdutf.h"

#include <string>
#include <cstddef>
#include <cstdint>

uint64_t transcode_checksum(const std::u16string& input) {
  // FNV-1a 64-bit offset basis and prime
  const uint64_t fnv_offset_basis = 1469598103934665603ULL;
  const uint64_t fnv_prime = 1099511628211ULL;

  const size_t in_len = input.size();
  // Fast path for empty input
  if (in_len == 0) {
    return fnv_offset_basis;
  }

  // Compute exact UTF-8 length and allocate once
  const size_t utf8_len = simdutf::utf16_to_utf8_length(input.data(), in_len);
  if (utf8_len == 0) {
    // Valid input per task contract; still guard in case of unexpected behavior
    return fnv_offset_basis;
  }

  std::string out;
  out.resize(utf8_len);

  // Convert using simdutf (high-performance, vectorized)
  size_t written = simdutf::utf16_to_utf8(input.data(), in_len, &out[0]);
  // Compute checksum over the exact number of bytes produced
  uint64_t hash = fnv_offset_basis;
  const unsigned char* ptr = reinterpret_cast<const unsigned char*>(out.data());
  for (size_t i = 0; i < written; i++) {
    hash ^= static_cast<uint64_t>(ptr[i]);
    hash *= fnv_prime;
  }
  return hash;
}
