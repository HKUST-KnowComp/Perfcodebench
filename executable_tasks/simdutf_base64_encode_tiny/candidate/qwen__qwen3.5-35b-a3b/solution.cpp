#include "interface.h"

#include <string>
#include <simdutf.h>

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
  std::string out;
  size_t out_size = ((input.size() + 2) / 3) * 4;
  out.resize(out_size);
  simdutf::base64::encode_base64(input.data(), input.size(), out.data(), out_size);
  return checksum_bytes(out);
}