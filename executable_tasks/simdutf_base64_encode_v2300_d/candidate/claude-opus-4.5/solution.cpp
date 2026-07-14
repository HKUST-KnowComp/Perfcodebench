#include "interface.h"

#include <simdutf.h>
#include <string>
#include <cstdint>

namespace {

uint64_t checksum_bytes(const char* data, std::size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  if (input.empty()) {
    return 1469598103934665603ULL;
  }
  
  // Calculate required output size for base64 encoding
  std::size_t output_len = simdutf::base64_length_from_binary(input.size());
  
  // Allocate output buffer
  std::string out(output_len, '\0');
  
  // Use simdutf's SIMD-accelerated base64 encoding
  std::size_t written = simdutf::binary_to_base64(
      input.data(),
      input.size(),
      &out[0]
  );
  
  // Compute checksum over the encoded output
  return checksum_bytes(out.data(), written);
}
