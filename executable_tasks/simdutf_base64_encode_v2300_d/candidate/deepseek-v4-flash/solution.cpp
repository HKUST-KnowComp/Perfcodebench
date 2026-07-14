#include "interface.h"
#include <string>
#include <cstdint>
#include <cstddef>
#include <simdutf.h>

uint64_t encode_checksum(const std::string& input) {
  // Use simdutf library for fast base64 encoding
  std::string out = simdutf::base64::encode(input.data(), input.size());
  // Compute FNV-1a checksum over the exact base64 output bytes
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char ch : out) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}
