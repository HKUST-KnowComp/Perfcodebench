#include "interface.h"
#include <simdutf.h>
#include <string>
#include <cstdint>

namespace {

inline uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
  const unsigned char* end = p + len;
  while (p < end) {
    hash ^= static_cast<uint64_t>(*p++);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  if (input.empty()) {
    return 1469598103934665603ULL;  // FNV offset basis for empty input
  }
  
  size_t output_len = simdutf::base64_length_from_binary(input.size());
  std::string out(output_len, '\0');
  
  simdutf::binary_to_base64(input.data(), input.size(), out.data());
  
  return checksum_bytes(out.data(), output_len);
}