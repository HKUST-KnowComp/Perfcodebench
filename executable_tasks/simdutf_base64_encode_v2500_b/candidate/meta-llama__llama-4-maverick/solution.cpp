#include "interface.h"
#include "simdutf.h"

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
  size_t encoded_length = simdutf::base64_length_from_binary_length(input.size());
  std::string out(encoded_length, '\0');
  simdutf::base64_encode(input.data(), input.size(), out.data(), encoded_length);
  return checksum_bytes(out);
}