#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>

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
  if (input.empty()) return checksum_bytes("");

  // Calculate required size for base64 output
  size_t input_len = input.size();
  size_t output_len = 4 * ((input_len + 2) / 3);
  
  std::string out;
  out.resize(output_len);

  // Use simdutf for high-performance base64 encoding
  // base64_encode returns the number of bytes written
  simdutf::base64_encode(input.data(), input_len, out.data());

  return checksum_bytes(out);
}