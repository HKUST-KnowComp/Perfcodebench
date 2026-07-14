#include "interface.h"
#include "simdutf.h"

#include <string>

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
  const std::size_t input_len = input.size();
  if (input_len == 0) {
    return checksum_bytes(std::string{});
  }
  // Compute exact output length for padded base64
  const std::size_t output_len = simdutf::base64_encode_length_from_binary_size(input_len, true);
  std::string out;
  out.resize(output_len);
  // Perform encoding using SIMD-accelerated routine
  const std::size_t written = simdutf::base64_encode(input.data(), input_len, out.data(), true);
  out.resize(written);  // ensure correct size (should equal output_len)
  return checksum_bytes(out);
}
