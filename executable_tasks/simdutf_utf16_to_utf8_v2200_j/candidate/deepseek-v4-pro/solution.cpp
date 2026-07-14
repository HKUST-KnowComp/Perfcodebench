#include "interface.h"

#include <cstdint>
#include <string>
#include <simdutf.h>

namespace {

uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  std::string out;
  out.resize(input.size() * 3);
  size_t out_len = simdutf::convert_valid_utf16le_to_utf8(input.data(), input.size(), &out[0]);
  return checksum_bytes(out.data(), out_len);
}