#include "interface.h"
#include <simdutf.h>
#include <string>
#include <cstdint>

uint64_t transcode_checksum(const std::u16string& input) {
  std::string out;
  out.reserve(input.size() * 3);
  size_t written = simdutf::utf16_to_utf8(input.data(), input.size(), &out[0]);
  out.resize(written);
  
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char ch : out) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}