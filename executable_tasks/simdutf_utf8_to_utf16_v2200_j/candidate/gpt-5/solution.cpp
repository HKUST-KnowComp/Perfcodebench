#include "interface.h"

#include <vector>
#include <cstddef>
#include "simdutf.h"

namespace {

inline uint64_t checksum_utf16(const char16_t* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis
  const char16_t* end = data + len;
  while (data < end) {
    hash ^= static_cast<uint64_t>(*data++);
    hash *= 1099511628211ULL; // FNV-1a 64-bit prime
  }
  return hash;
}

} // namespace

uint64_t transcode_checksum(const std::string& input) {
  // Fast path for empty input: checksum over empty sequence
  if (input.empty()) {
    return 1469598103934665603ULL;
  }

  // Allocate maximum needed UTF-16 code units (<= input.size()).
  std::vector<char16_t> out;
  out.resize(input.size());

  // Transcode using simdutf (valid UTF-8 is assumed by the task).
  size_t written = simdutf::utf8_to_utf16le(input.data(), input.size(), out.data());

  // Compute checksum over the exact UTF-16 code unit sequence produced.
  return checksum_utf16(out.data(), written);
}
