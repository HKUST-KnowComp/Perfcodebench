#include "interface.h"

#include <simdutf.h>
#include <vector>
#include <cstdint>

namespace {

inline uint64_t checksum_utf16(const char16_t* data, size_t length) {
  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis
  const char16_t* end = data + length;
  for (const char16_t* p = data; p != end; ++p) {
    hash ^= static_cast<uint64_t>(*p);
    hash *= 1099511628211ULL; // FNV-1a 64-bit prime
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  // Maximum number of UTF-16 code units is bounded by input.size()
  // (ASCII: 1 byte -> 1 code unit; 4-byte UTF-8 -> 2 code units).
  std::vector<char16_t> out(input.size());

  // Use simdutf for fast conversion to UTF-16LE.
  size_t written = simdutf::convert_utf8_to_utf16le(
      input.data(), input.size(), out.data());

  // Compute checksum over exactly the produced UTF-16 code unit sequence.
  return checksum_utf16(out.data(), written);
}
