#include "interface.h"

#include <simdutf.h>
#include <cstdint>
#include <memory>

uint64_t transcode_checksum(const std::string& input) {
  // FNV-1a 64-bit offset basis
  constexpr uint64_t fnv_offset = 1469598103934665603ULL;
  constexpr uint64_t fnv_prime  = 1099511628211ULL;

  const size_t in_size = input.size();
  if (in_size == 0) {
    return fnv_offset;
  }

  // Upper bound: number of UTF-16 code units is <= number of UTF-8 bytes
  std::unique_ptr<char16_t[]> buffer(new char16_t[in_size]);

  // Input is guaranteed valid per task; use the fast valid converter
  size_t written = simdutf::convert_valid_utf8_to_utf16le(input.data(), in_size, buffer.get());

  // Compute FNV-1a over the exact UTF-16 code unit sequence
  uint64_t hash = fnv_offset;
  const char16_t* out = buffer.get();
  for (size_t i = 0; i < written; ++i) {
    hash ^= static_cast<uint64_t>(out[i]);
    hash *= fnv_prime;
  }
  return hash;
}
