#include "interface.h"

#include <simdutf.h>
#include <vector>
#include <cstdint>
#include <string>

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
  std::vector<char> output(input.size() * 3);
  size_t bytes_written = simdutf::utf16_to_utf8(input.data(), input.size(), output.data());
  return checksum_bytes(output.data(), bytes_written);
}