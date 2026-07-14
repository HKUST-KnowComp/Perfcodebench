#include "interface.h"
#include "simdutf.h"

#include <cstdint>
#include <vector>

namespace {

inline uint64_t checksum_utf16(const char16_t* data, size_t count) {
  uint64_t hash = 1469598103934665603ULL; // FNV-1a offset basis
  for (size_t i = 0; i < count; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL; // FNV-1a prime
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  const char* src = input.data();
  const size_t len = input.size();

  // Allocate worst-case number of UTF-16 code units (<= input.size()).
  std::vector<char16_t> out(len);
  size_t written = simdutf::convert_utf8_to_utf16le(src, len, out.data());

  return checksum_utf16(out.data(), written);
}
