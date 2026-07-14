#include "interface.h"

#include <simdutf.h>
#include <string>
#include <cstdint>

namespace {
inline uint64_t fnv1a64(const uint8_t* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis
  const uint8_t* p = data;
  const uint8_t* end = data + len;
  const uint64_t prime = 1099511628211ULL; // FNV-1a 64-bit prime
  while (p + 8 <= end) {
    hash ^= p[0]; hash *= prime;
    hash ^= p[1]; hash *= prime;
    hash ^= p[2]; hash *= prime;
    hash ^= p[3]; hash *= prime;
    hash ^= p[4]; hash *= prime;
    hash ^= p[5]; hash *= prime;
    hash ^= p[6]; hash *= prime;
    hash ^= p[7]; hash *= prime;
    p += 8;
  }
  while (p < end) {
    hash ^= *p++;
    hash *= prime;
  }
  return hash;
}
} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  const char16_t* in = input.data();
  const size_t in_len = input.size();
  // Fast path: empty input -> FNV-1a of empty sequence
  if (in_len == 0) {
    return 1469598103934665603ULL;
  }

  // Compute exact required UTF-8 length and allocate once
  const size_t out_len = simdutf::utf8_length_from_utf16(in, in_len);
  std::string out(out_len, '\0');

  // Convert assuming valid UTF-16 input (per task contract)
  size_t written = simdutf::convert_valid_utf16_to_utf8(in, in_len, out.data());
  (void)written; // written should equal out_len on valid input

  // Compute FNV-1a checksum over the exact UTF-8 bytes
  return fnv1a64(reinterpret_cast<const uint8_t*>(out.data()), out_len);
}
