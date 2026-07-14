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

uint64_t encode_checksum(const std::string& input) {
  const size_t in_len = input.size();
  const size_t out_len = simdutf::base64_length_from_binary(in_len);

  std::string out;
  out.resize(out_len);

  const size_t written = simdutf::binary_to_base64(input.data(), in_len, out.data());
  return checksum_bytes(out.data(), written);
}
