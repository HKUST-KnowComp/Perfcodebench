#include "interface.h"

#include <simdutf.h>
#include <cstddef>
#include <cstdint>
#include <vector>

uint64_t decode_checksum(const std::string& input) {
  if (input.empty()) {
    return 1469598103934665603ULL;
  }
  std::vector<char> buffer((input.size() / 4) * 3 + 4);
  size_t decoded_len = simdutf::base64_decode(input.data(), input.size(), buffer.data());
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < decoded_len; ++i) {
    unsigned char ch = static_cast<unsigned char>(buffer[i]);
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}