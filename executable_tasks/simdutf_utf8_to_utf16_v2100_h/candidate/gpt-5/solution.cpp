#include "interface.h"
#include "simdutf.h"

#include <vector>
#include <cstdint>
#include <cstddef>

namespace {

inline uint64_t checksum_utf16(const char16_t* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL; // FNV-1a prime
  }
  return hash;
}

} // namespace

uint64_t transcode_checksum(const std::string& input) {
  if (input.empty()) {
    return 1469598103934665603ULL; // checksum over empty UTF-16 sequence
  }

  // Maximum possible UTF-16 code units is bounded by input.size().
  std::vector<char16_t> out;
  out.resize(input.size());

  const size_t written = simdutf::utf8_to_utf16(input.data(), input.size(), out.data());

  return checksum_utf16(out.data(), written);
}
