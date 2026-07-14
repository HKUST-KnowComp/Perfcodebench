#include "interface.h"

#include <vector>
#include "simdutf.h"

namespace {

inline uint64_t checksum_utf16(const char16_t* data, size_t length) {
  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis
  const char16_t* end = data + length;
  for (const char16_t* p = data; p != end; ++p) {
    hash ^= static_cast<uint64_t>(*p);
    hash *= 1099511628211ULL; // FNV prime
  }
  return hash;
}

} // namespace

uint64_t transcode_checksum(const std::string& input) {
  if (input.empty()) {
    // FNV-1a hash of empty sequence
    return 1469598103934665603ULL;
  }

  // Allocate a buffer large enough: max UTF-16 code units <= input bytes
  std::vector<char16_t> out(input.size());

  // Convert using simdutf (valid UTF-8 is assumed by task contract)
  size_t written = simdutf::convert_utf8_to_utf16(input.data(), input.size(), out.data());

  // Compute checksum over the exact number of written code units
  return checksum_utf16(out.data(), written);
}