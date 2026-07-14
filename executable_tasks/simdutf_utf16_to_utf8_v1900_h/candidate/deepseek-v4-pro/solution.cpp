#include "interface.h"
#include <simdutf.h>

namespace {
uint64_t checksum_bytes(const std::string& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}
} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
    std::string out = simdutf::convert_utf16le_to_utf8(input);
    return checksum_bytes(out);
}