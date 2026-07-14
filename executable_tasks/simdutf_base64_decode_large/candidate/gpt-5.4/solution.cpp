#include "interface.h"

#include <simdutf.h>

#include <cstdint>
#include <string>

uint64_t decode_checksum(const std::string& input) {
  const size_t max_len = simdutf::maximal_binary_length_from_base64(input.data(), input.size());
  std::string decoded;
  decoded.resize(max_len);

  size_t written = 0;
  simdutf::result r = simdutf::base64_to_binary(
      input.data(), input.size(), decoded.data(), simdutf::base64_default, written);

  if (r.error != simdutf::error_code::SUCCESS) {
    return 0;
  }

  decoded.resize(written);

  uint64_t hash = 1469598103934665603ULL;
  const unsigned char* p = reinterpret_cast<const unsigned char*>(decoded.data());
  const unsigned char* end = p + written;
  for (; p != end; ++p) {
    hash ^= static_cast<uint64_t>(*p);
    hash *= 1099511628211ULL;
  }
  return hash;
}
