#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>

namespace {

uint64_t checksum_bytes(const char* data, size_t length) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < length; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  if (input.empty()) return 1469598103934665603ULL;

  // Calculate required size for base64 output
  size_t input_size = input.size();
  size_t output_size = simdutf::base64_encode_size(input_size);
  
  std::string out;
  out.resize(output_size);

  // Use simdutf for high-performance encoding
  simdutf::base64_encode(input.data(), input_size, out.data());

  return checksum_bytes(out.data(), out.size());
}