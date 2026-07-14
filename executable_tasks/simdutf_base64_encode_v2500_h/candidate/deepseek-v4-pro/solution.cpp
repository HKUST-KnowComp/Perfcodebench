#include "interface.h"

#include <simdutf.h>
#include <string>
#include <cstdint>

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
  const size_t out_len = ((input.size() + 2) / 3) * 4;
  std::string out(out_len, '\0');
  size_t actual_len = simdutf::binary_to_base64(
      input.data(), input.size(), &out[0]);
  (void)actual_len;  // Should equal out_len
  return checksum_bytes(out);
}
