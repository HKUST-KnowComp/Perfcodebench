#include "interface.h"
#include <simdutf.h>
#include <vector>

namespace {

uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  size_t expected_len = simdutf::utf8_length_from_latin1(input.data(), input.size());
  std::vector<char> out(expected_len);
  
  size_t actual_len = simdutf::convert_latin1_to_utf8(input.data(), input.size(), out.data());
  
  return checksum_bytes(out.data(), actual_len);
}
