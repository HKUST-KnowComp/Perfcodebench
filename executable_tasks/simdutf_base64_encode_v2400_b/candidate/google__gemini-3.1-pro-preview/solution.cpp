#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>

namespace {

/**
 * Optimized FNV-1a hash for checksumming the base64 output.
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

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  if (input.empty()) {
    return 1469598103934665603ULL;
  }

  // Calculate required base64 length
  size_t expected_len = simdutf::base64_length_from_binary(input.size());
  
  // Use a stack-allocated buffer for small inputs to reduce latency, 
  // or a vector for larger inputs.
  char stack_buf[4096];
  char* out_ptr;
  std::vector<char> heap_buf;

  if (expected_len <= sizeof(stack_buf)) {
    out_ptr = stack_buf;
  } else {
    heap_buf.resize(expected_len);
    out_ptr = heap_buf.data();
  }

  // Perform SIMD-accelerated base64 encoding
  size_t actual_len = simdutf::binary_to_base64(input.data(), input.size(), out_ptr);

  // Calculate checksum over the resulting base64 bytes
  return checksum_bytes(out_ptr, actual_len);
}