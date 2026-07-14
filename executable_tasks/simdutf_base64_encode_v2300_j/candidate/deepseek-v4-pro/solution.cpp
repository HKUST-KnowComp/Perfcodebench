#include "interface.h"
#include <simdutf.h>
#include <cstddef>
#include <cstdint>

namespace {

constexpr uint64_t kHashSeed = UINT64_C(1469598103934665603);
constexpr uint64_t kHashMult = UINT64_C(1099511628211);

uint64_t checksum_bytes(const unsigned char* data, std::size_t length) {
  uint64_t hash = kHashSeed;
  for (std::size_t i = 0; i < length; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= kHashMult;
  }
  return hash;
}

} // anonymous namespace

uint64_t encode_checksum(const std::string& input) {
  const std::size_t encoded_len = simdutf::base64_length_from_binary(input.size());
  std::string encoded(encoded_len, '\0');
  (void)simdutf::binary_to_base64(input.data(), input.size(), encoded.data());
  return checksum_bytes(
      reinterpret_cast<const unsigned char*>(encoded.data()),
      encoded_len);
}
