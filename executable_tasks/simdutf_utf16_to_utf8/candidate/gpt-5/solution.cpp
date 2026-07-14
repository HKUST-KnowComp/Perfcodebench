#include "interface.h"

#include "simdutf.h"

#include <cstddef>
#include <cstdint>

uint64_t transcode_checksum(const std::u16string& input) {
  const size_t in_len = input.size();
  // FNV-1a 64-bit offset basis
  uint64_t hash = 1469598103934665603ULL;
  if (in_len == 0) {
    return hash; // checksum over empty output
  }

  // Maximum UTF-8 bytes per char16_t is 3 (BMP: up to 3 bytes; surrogate pair: 4 bytes for 2 units => 2 per unit).
  const size_t cap = in_len * 3;
  char* out = new char[cap];

  const size_t encoded = simdutf::convert_utf16_to_utf8(input.data(), in_len, out);

  // Compute FNV-1a over the exact UTF-8 byte sequence.
  const unsigned char* p = reinterpret_cast<const unsigned char*>(out);
  for (size_t i = 0; i < encoded; ++i) {
    hash ^= p[i];
    hash *= 1099511628211ULL; // FNV prime
  }

  delete[] out;
  return hash;
}
