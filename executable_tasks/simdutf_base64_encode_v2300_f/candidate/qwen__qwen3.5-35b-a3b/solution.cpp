#include "interface.h"

#include <string>
#include <simdutf.h>

namespace {

constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

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
  size_t encoded_size = simdutf::base64::encoded_length(input.size());
  std::string output(encoded_size, '\0');
  simdutf::base64::encode(input.data(), input.size(), output.data());
  return checksum_bytes(output);
}