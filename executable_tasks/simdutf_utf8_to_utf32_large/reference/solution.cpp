#include "interface.h"

#include "simdutf.h"

#include <vector>

namespace {

uint64_t checksum_utf32(const std::vector<char32_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (char32_t ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  std::vector<char32_t> out(simdutf::utf32_length_from_utf8(input.data(), input.size()));
  const size_t n = simdutf::convert_utf8_to_utf32(input.data(), input.size(), out.data());
  out.resize(n);
  return checksum_utf32(out);
}
