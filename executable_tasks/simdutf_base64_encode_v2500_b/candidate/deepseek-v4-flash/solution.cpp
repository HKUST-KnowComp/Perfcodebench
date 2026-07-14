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
  // Use simdutf for fast base64 encoding
  std::string encoded = simdutf::base64_encode(input);
  return checksum_bytes(encoded);
}