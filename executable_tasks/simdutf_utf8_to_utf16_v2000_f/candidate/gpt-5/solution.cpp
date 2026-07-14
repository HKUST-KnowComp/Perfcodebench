#include "interface.h"
#include "simdutf.h"

#include <cstdint>
#include <cstddef>
#include <memory>

namespace {
inline uint64_t checksum_utf16(const char16_t* data, size_t length) {
  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis
  const char16_t* p = data;
  const char16_t* end = data + length;
  while (p < end) {
    hash ^= static_cast<uint64_t>(*p++);
    hash *= 1099511628211ULL; // FNV-1a 64-bit prime
  }
  return hash;
}
} // namespace

uint64_t transcode_checksum(const std::string& input) {
  const char* src = input.data();
  const size_t len = input.size();
  if (len == 0) {
    return 1469598103934665603ULL; // checksum of empty sequence
  }

  // Worst-case: number of UTF-16 code units is <= number of input bytes.
  const size_t max_out = len;
  std::unique_ptr<char16_t[]> out(new char16_t[max_out]);

  // Input is specified to be valid UTF-8, so we can use the fast path.
  size_t written = simdutf::convert_valid_utf8_to_utf16le(src, len, out.get());

  return checksum_utf16(out.get(), written);
}
