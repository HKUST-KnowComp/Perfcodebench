#include "interface.h"
#include <simdutf.h>
#include <string>

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

uint64_t decode_checksum(const std::string& input) {
  const size_t max_out_len = simdutf::base64_maximum_length(input.size());
  std::string out(max_out_len, '\0');
  const size_t actual_out_len = simdutf::decode_base64(input.data(), input.size(), out.data());
  out.resize(actual_out_len);
  return checksum_bytes(out);
}