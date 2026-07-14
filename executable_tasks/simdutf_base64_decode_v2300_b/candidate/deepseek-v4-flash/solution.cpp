#include "interface.h"

#include <cstdint>
#include <string>
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

}  // namespace

uint64_t decode_checksum(const std::string& input) {
  size_t max_out = simdutf::base64::max_decoded_length(input.size());
  std::string out(max_out, '\0');
  auto result = simdutf::base64::decode(input.data(), input.size(), out.data());
  out.resize(result.count);
  return checksum_bytes(out);
}
