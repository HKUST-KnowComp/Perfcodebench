#include "interface.h"
#include <string>
#include <simdutf.h>

uint64_t decode_checksum(const std::string& input) {
  size_t max_output_size = (input.size() / 4) * 3 + 4;
  std::string output;
  output.resize(max_output_size);

  size_t decoded_size = simdutf::base64::decode(input, output.data(), output.size());
  output.resize(decoded_size);

  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char ch : output) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}