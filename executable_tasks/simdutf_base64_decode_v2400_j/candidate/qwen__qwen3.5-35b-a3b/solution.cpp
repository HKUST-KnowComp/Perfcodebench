#include "interface.h"
#include <simdutf.h>
#include <string>
#include <cstdint>

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

uint64_t decode_checksum(const std::string& input) {
  std::string out;
  size_t max_output_size = (input.size() / 4) * 3;
  out.resize(max_output_size);
  size_t decoded_size = simdutf::base64_decode(input.data(), input.size(), out.data(), max_output_size);
  out.resize(decoded_size);
  return checksum_bytes(out);
}