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
  std::size_t base64_length = simdutf::base64_length_from_binary(input.size());
  std::string out(base64_length, '\0');
  simdutf::binary_to_base64(input.data(), input.size(), out.data());
  return checksum_bytes(out);
}
