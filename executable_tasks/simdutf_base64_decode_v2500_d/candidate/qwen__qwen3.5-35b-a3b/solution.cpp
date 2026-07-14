#include "interface.h"
#include <string>
#include <simdutf.h>

uint64_t decode_checksum(const std::string& input) {
  size_t output_size = (input.size() / 4) * 3;
  std::string out;
  out.resize(output_size);
  size_t decoded_size = simdutf::base64::decode(input.data(), input.size(), out.data(), output_size);
  
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < decoded_size; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(out[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}