#include "interface.h"
#include <simdutf.h>
#include <string>
#include <vector>

uint64_t decode_checksum(const std::string& input) {
  size_t max_out = input.size() * 3 / 4 + 4;
  std::vector<char> out(max_out);
  size_t decoded_len = simdutf::base64_decode(input.data(), input.size(), out.data());
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < decoded_len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(out[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}
