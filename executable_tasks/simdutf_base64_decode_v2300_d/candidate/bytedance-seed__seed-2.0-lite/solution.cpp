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

uint64_t decode_checksum(const std::string& input) {
  const size_t max_decoded = simdutf::base64_maximum_decoded_length(input.size());
  std::string out;
  out.resize(max_decoded);
  const size_t actual_written = simdutf::decode_base64(input.data(), input.size(), out.data());
  out.resize(actual_written);
  return checksum_bytes(out);
}