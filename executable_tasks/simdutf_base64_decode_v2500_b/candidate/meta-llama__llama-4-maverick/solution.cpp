#include "interface.h"
#include "simdutf.h"

uint64_t checksum_bytes(const std::string& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t decode_checksum(const std::string& input) {
  std::string out;
  out.resize(simdutf::base64_decode_bound(input.size(), simdutf::base64_default));
  size_t decoded_size = simdutf::base64_decode(input.data(), input.size(), out.data(), simdutf::base64_default);
  out.resize(decoded_size);
  return checksum_bytes(out);
}