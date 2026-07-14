#include "interface.h"
#include <simdutf.h>
#include <vector>

uint64_t transcode_checksum(const std::string& input) {
  std::vector<char32_t> out(input.size());
  const size_t in_len = input.size();
  size_t out_len = simdutf::convert_utf8_to_utf32(input.data(), in_len, out.data());
  out.resize(out_len);

  uint64_t hash = 1469598103934665603ULL;
  for (const char32_t ch : out) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}