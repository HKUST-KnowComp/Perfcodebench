#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>

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
  if (input.empty()) return 1469598103934665603ULL;

  // Calculate required size for base64 output
  std::size_t input_len = input.size();
  std::size_t output_len = simdutf::base64_encode_size(input_len);
  
  std::string out;
  out.resize(output_len);

  // Use simdutf for high-performance encoding
  simdutf::base64_encode(input.data(), input_len, out.data());

  return checksum_bytes(out.data(), out.size());
}