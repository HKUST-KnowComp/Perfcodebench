#include "interface.h"

#include <simdutf.h>
#include <vector>
#include <cstdint>
#include <cstddef>

namespace {

uint64_t checksum_bytes(const char* data, std::size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < len; ++i) {
    uint8_t ch = static_cast<uint8_t>(data[i]);
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  const std::size_t output_len = ((input.size() + 2) / 3) * 4;
  std::vector<char> output(output_len);
  std::size_t written = simdutf::base64_encode(input.data(), input.size(), output.data());
  // written should equal output_len; using it as the actual length is safe.
  return checksum_bytes(output.data(), written);
}