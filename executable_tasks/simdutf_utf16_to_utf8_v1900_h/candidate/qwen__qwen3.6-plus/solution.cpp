#include "interface.h"
#include <simdutf.h>
#include <string>
#include <cstdint>

namespace {

uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
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
  out.resize(input.size() * 3);
  size_t len = simdutf::convert_utf16le_to_utf8(input.data(), input.size(), out.data());
  out.resize(len);
  return checksum_bytes(out.data(), len);
}
