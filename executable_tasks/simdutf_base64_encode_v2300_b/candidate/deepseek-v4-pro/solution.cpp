#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <string>
#include <vector>

namespace {

uint64_t checksum(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

} // namespace

uint64_t encode_checksum(const std::string& input) {
  size_t out_len = ((input.size() + 2) / 3) * 4;
  std::vector<char> out(out_len);
  size_t written = simdutf::base64_encode(input.data(), input.size(), out.data(), false);
  return checksum(out.data(), written);
}
