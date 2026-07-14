#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>

namespace {

uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
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

  // Calculate required size for base64 output
  size_t input_len = input.size();
  size_t output_len = simdutf::base64_encoded_size(input_len);
  
  // Use a vector to avoid potential std::string overhead/reallocations
  // and to provide a contiguous buffer for simdutf
  std::vector<char> buffer(output_len);
  
  // Use simdutf for high-performance SIMD-accelerated encoding
  simdutf::base64_encode(input.data(), input_len, buffer.data());

  return checksum_bytes(buffer.data(), output_len);
}