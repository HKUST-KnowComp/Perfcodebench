#include "interface.h"

#include <cstdint>
#include <string>
#include <simdutf.h>

namespace {

static inline uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<unsigned char>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t decode_checksum(const std::string& input) {
  const size_t n = input.size();
  if (n == 0) {
    return 1469598103934665603ULL;
  }

  size_t max_len = simdutf::maximal_binary_length_from_base64(input.data(), n);
  std::string decoded;
  decoded.resize(max_len);

  simdutf::result r = simdutf::base64_to_binary(
      input.data(), n, decoded.data(), simdutf::base64_default);

  if (r.error != simdutf::error_code::SUCCESS) {
    return 1469598103934665603ULL;
  }

  decoded.resize(r.count);
  return checksum_bytes(decoded.data(), decoded.size());
}
