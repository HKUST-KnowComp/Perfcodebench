#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>

namespace {

uint64_t checksum_bytes(const char* data, size_t length) {
  uint64_t hash = 1469598103934665603ULL;
  const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
  for (size_t i = 0; i < length; ++i) {
    hash ^= static_cast<uint64_t>(p[i]);
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
  size_t output_len = ((input_len + 2) / 3) * 4;
  
  std::string out;
  out.resize(output_len);

  // Use simdutf for high-performance base64 encoding
  simdutf::base64_encode(input.data(), input_len, out.data());

  return checksum_bytes(out.data(), out.size());
}