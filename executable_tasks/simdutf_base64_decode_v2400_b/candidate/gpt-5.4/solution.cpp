#include "interface.h"

#include <simdutf.h>

#include <cstdint>
#include <string>

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
  const size_t in_len = input.size();
  if (in_len == 0) {
    return 1469598103934665603ULL;
  }

  std::string decoded;
  decoded.resize(simdutf::maximal_binary_length_from_base64(input.data(), in_len));

  simdutf::result r = simdutf::base64_to_binary(
      input.data(),
      in_len,
      decoded.data(),
      simdutf::base64_default);

  decoded.resize(r.count);
  return checksum_bytes(decoded.data(), decoded.size());
}
