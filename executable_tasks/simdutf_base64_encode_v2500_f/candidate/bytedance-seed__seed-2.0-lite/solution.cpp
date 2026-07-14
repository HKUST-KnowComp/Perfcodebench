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

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  const size_t input_len = input.size();
  const size_t output_len = simdutf::base64_length_from_binary(input_len);
  std::string out(output_len, '\0');
  simdutf::encode_base64(input.data(), input_len, out.data());
  return checksum_bytes(out);
}