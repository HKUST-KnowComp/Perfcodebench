#include "interface.h"
#include <simdutf.h>
#include <string>

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
  const size_t output_size = simdutf::base64_encode_length(input.size());
  std::string out;
  out.resize(output_size);
  simdutf::base64_encode(input.data(), input.size(), out.data());
  return checksum_bytes(out);
}