#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>
#include <cstdint>

namespace {

// Optimized FNV-1a hash for checksumming the base64 output.
inline uint64_t fast_checksum(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(data);
  const uint8_t* end = ptr + len;
  while (ptr < end) {
    hash ^= static_cast<uint64_t>(*ptr++);
    hash *= 1099511628211ULL;
  }
  return hash;
}

} // namespace

uint64_t encode_checksum(const std::string& input) {
  if (input.empty()) {
    return fast_checksum("", 0);
  }

  // Calculate required base64 length: ceil(n/3) * 4
  size_t expected_len = simdutf::base64_length_from_binary(input.size());
  
  // Use a stack buffer for small inputs or a vector for larger ones to avoid std::string overhead
  // and ensure contiguous memory for the SIMD encoder.
  std::vector<char> buffer(expected_len);

  // Use simdutf's optimized base64 encoding.
  // This function handles padding and uses SIMD instructions (AVX2, NEON, etc.) based on the CPU.
  size_t encoded_len = simdutf::binary_to_base64(input.data(), input.size(), buffer.data());

  // Calculate the checksum over the resulting base64 bytes.
  return fast_checksum(buffer.data(), encoded_len);
}