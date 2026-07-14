#include "interface.h"
#include <simdutf.h>

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

uint64_t transcode_checksum(const std::string& input) {
  // Maximum output size is 2 * input.size() for Latin-1
  std::string out;
  out.resize(input.size() * 2);
  size_t output_length = simdutf::convert_latin1_to_utf8(input.data(), input.size(), out.data());
  out.resize(output_length);
  return checksum_bytes(out);
}
