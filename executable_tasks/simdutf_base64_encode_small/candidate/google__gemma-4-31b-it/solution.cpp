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
  size_t output_len = simdutf::base64_encode_size(input_len);
  
  // Use a stack buffer for small inputs to avoid heap allocation
  char buffer[1024];
  char* out_ptr = buffer;
  std::vector<char> heap_buffer;

  if (output_len > sizeof(buffer)) {
    heap_buffer.resize(output_len);
    out_ptr = heap_buffer.data();
  }

  // Perform SIMD-accelerated base64 encoding
  simdutf::base64_encode(input.data(), input_len, out_ptr);

  return checksum_bytes(out_ptr, output_len);
}