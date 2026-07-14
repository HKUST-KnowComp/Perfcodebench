#include "interface.h"

#include <simdutf.h>

#include <cstdint>
#include <string>

namespace {

static inline uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t decode_checksum(const std::string& input) {
  const size_t in_len = input.size();
  const size_t max_out = simdutf::maximal_binary_length_from_base64(input.data(), in_len);

  std::string out;
  out.resize(max_out);

  simdutf::result r = simdutf::base64_to_binary(input.data(), in_len, out.data());
  const size_t out_len = (r.error == simdutf::error_code::SUCCESS) ? r.count : 0;

  return checksum_bytes(out.data(), out_len);
}
