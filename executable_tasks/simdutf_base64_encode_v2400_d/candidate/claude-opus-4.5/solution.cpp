#include "interface.h"

#include <simdutf.h>
#include <string>
#include <cstdint>

namespace {

inline uint64_t checksum_bytes(const char* data, std::size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const char* end = data + len;
  while (data != end) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(*data++));
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  if (input.empty()) {
    return 1469598103934665603ULL;  // hash of empty string
  }
  
  // Calculate required output size for base64 encoding
  std::size_t encoded_len = simdutf::base64_length_from_binary(input.size());
  
  // Allocate output buffer
  std::string out;
  out.resize(encoded_len);
  
  // Use simdutf's optimized SIMD base64 encoder
  std::size_t actual_len = simdutf::binary_to_base64(
      input.data(),
      input.size(),
      out.data());
  
  // Compute checksum over the encoded output
  return checksum_bytes(out.data(), actual_len);
}