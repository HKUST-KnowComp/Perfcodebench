#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>
#include <cstdint>

namespace {

/**
 * FNV-1a 64-bit hash implementation.
 */
inline uint64_t checksum_bytes(const char* data, size_t len) {
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
    return 1469598103934665603ULL;
  }

  // Calculate required base64 output length
  size_t out_len = simdutf::base64_length_from_binary(input.size());
  
  // Use a stack-allocated buffer for small inputs or a vector for larger ones
  // to avoid frequent small heap allocations if possible, though for simplicity
  // and safety with simdutf, we'll use a vector here.
  std::vector<char> buffer(out_len);

  // Use simdutf to perform the base64 encoding. 
  // This uses SIMD (AVX2, NEON, etc.) depending on the hardware.
  size_t written = simdutf::binary_to_base64(input.data(), input.size(), buffer.data());

  // Calculate the checksum over the encoded bytes.
  return checksum_bytes(buffer.data(), written);
}