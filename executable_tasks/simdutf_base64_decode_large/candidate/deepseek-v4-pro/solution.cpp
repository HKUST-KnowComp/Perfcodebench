#include "interface.h"

#include <simdutf.h>
#include <cstdint>
#include <cstddef>
#include <vector>

namespace {

uint64_t checksum_bytes(const char* data, std::size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t mul = 1099511628211ULL;
  for (std::size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= mul;
  }
  return hash;
}

}  // namespace

uint64_t decode_checksum(const std::string& input) {
  // Maximum possible output size (4 base64 chars -> 3 binary bytes)
  const std::size_t max_out = ((input.size() + 3) / 4) * 3;
  std::vector<char> out(max_out);

  // Use SIMD-accelerated base64 decoding (standard with optional padding)
  auto result = simdutf::base64_to_binary(input.data(), input.size(), out.data());
  const std::size_t written = result.count;  // valid input: result.error == simdutf::SUCCESS

  return checksum_bytes(out.data(), written);
}
