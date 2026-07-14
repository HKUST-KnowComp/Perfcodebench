#include "interface.h"
#include "simdutf.h"

#include <string>
#include <cstdint>

static inline uint64_t fnv1a64(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis
  const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
  const unsigned char* end = p + len;
  while (p != end) {
    hash ^= static_cast<uint64_t>(*p++);
    hash *= 1099511628211ULL; // FNV-1a 64-bit prime
  }
  return hash;
}

uint64_t transcode_checksum(const std::u16string& input) {
  const char16_t* in = input.data();
  const size_t in_len = input.size();

  // Compute exact UTF-8 output length for valid UTF-16 input
  const size_t out_len = simdutf::utf16_to_utf8_length(in, in_len);

  // Allocate output buffer once and perform SIMD-accelerated conversion
  std::string out;
  out.resize(out_len);
  char* outptr = out_len ? &out[0] : nullptr;
  const size_t written = simdutf::convert_valid_utf16_to_utf8(in, in_len, outptr);

  // Compute checksum over the exact UTF-8 byte sequence
  return fnv1a64(out.data(), written);
}
