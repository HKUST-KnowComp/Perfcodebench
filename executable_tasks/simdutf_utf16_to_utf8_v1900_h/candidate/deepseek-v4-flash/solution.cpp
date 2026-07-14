#include "interface.h"
#include <simdutf.h>
#include <string>
#include <cstdint>

namespace {
uint64_t checksum_bytes(const std::string& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}
}  // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  if (input.empty()) {
    return 1469598103934665603ULL;
  }
  std::string out;
  out.resize(input.size() * 3);  // worst-case UTF-8 expansion
  size_t written = simdutf::convert_valid_utf16le_to_utf8(input.data(), input.size(), &out[0]);
  if (written == 0) {
    // fallback for safety (should not happen with valid input)
    written = simdutf::convert_utf16le_to_utf8(input.data(), input.size(), &out[0]);
  }
  out.resize(written);
  return checksum_bytes(out);
}
