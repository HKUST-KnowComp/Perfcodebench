#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <string>
#include <vector>

namespace {

uint64_t checksum_utf16(const char16_t* data, size_t length) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < length; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  // Pre-allocate maximum possible UTF-16 code units (each byte can produce one code unit for ASCII).
  std::vector<char16_t> out(input.size());
  // Use simdutf for fast, valid UTF-8 to UTF-16LE conversion.
  size_t code_units = simdutf::utf8_to_utf16le(input.data(), input.size(), out.data());
  return checksum_utf16(out.data(), code_units);
}