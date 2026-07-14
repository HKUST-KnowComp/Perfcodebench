#include "interface.h"

#include <simdutf.h>
#include <cstdint>
#include <string>
#include <vector>

namespace {

uint64_t checksum_bytes(const unsigned char* data, size_t length) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < length; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t decode_checksum(const std::string& input) {
  if (input.empty()) return checksum_bytes(nullptr, 0);
  // Allocate sufficient buffer (max output size: ceil(input.size() / 4) * 3)
  const size_t max_out = ((input.size() + 3) / 4) * 3;
  std::vector<unsigned char> buf(max_out);
  const size_t out_len = simdutf::base64_decode(input.data(), input.size(), reinterpret_cast<char*>(buf.data()));
  return checksum_bytes(buf.data(), out_len);
}