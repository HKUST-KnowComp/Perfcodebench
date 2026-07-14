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
  size_t out_len = simdutf::base64::decode_length(input.data(), input.size());
  std::string out(out_len, '\0');
  simdutf::base64::decode(input.data(), input.size(), out.data());
  return checksum_bytes(out);
}
