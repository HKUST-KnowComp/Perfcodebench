#include "interface.h"
#include <simdutf.h>
#include <string>
#include <cstdint>

namespace {

inline uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const char* end = data + len;
  while (data < end) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(*data++));
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  size_t encoded_len = simdutf::base64_length_from_binary(input.size());
  std::string out(encoded_len, '\0');
  simdutf::binary_to_base64(input.data(), input.size(), out.data());
  return checksum_bytes(out.data(), encoded_len);
}