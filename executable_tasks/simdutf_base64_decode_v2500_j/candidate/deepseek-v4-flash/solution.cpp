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

uint64_t decode_checksum(const std::string& input) {
  if (input.empty())
    return checksum_bytes(input);  // FNV-1a of empty string

  const std::size_t max_decoded_size = (input.size() / 4) * 3;
  std::string out(max_decoded_size, '\0');
  const std::size_t decoded_length = simdutf::base64::decode(
      input.data(), input.size(), out.data());
  out.resize(decoded_length);
  return checksum_bytes(out);
}
