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

uint64_t decode_checksum(const std::string& input) {
  const size_t output_max_size = simdutf::base64_length_from_message_size(input.size());
  std::string out(output_max_size, '\0');
  const size_t actual_size = simdutf::decode_base64(input.data(), input.size(), out.data());
  out.resize(actual_size);
  return checksum_bytes(out);
}