#include "interface.h"

#include <simdutf.h>
#include <string>
#include <vector>

namespace {

uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t decode_checksum(const std::string& input) {
  size_t max_size = simdutf::maximal_binary_length_from_base64(input.data(), input.size());
  std::vector<char> out(max_size);
  
  simdutf::result r = simdutf::base64_to_binary(input.data(), input.size(), out.data());
  
  if (r.error != simdutf::error_code::SUCCESS) {
    return 0;
  }
  
  return checksum_bytes(out.data(), r.count);
}