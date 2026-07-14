#include "interface.h"
#include <simdutf.h>
#include <vector>

namespace {

uint64_t checksum_utf16(const std::vector<char16_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (char16_t ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  const simdutf::implementation* simd = simdutf::get_active_implementation();
  const size_t max_utf16_units = simd->estimate_utf16_length_from_utf8(input.size());
  std::vector<char16_t> out(max_utf16_units);
  const size_t actual_utf16_units = simd->convert_utf8_to_utf16(input.data(), input.size(), out.data());
  out.resize(actual_utf16_units);
  return checksum_utf16(out);
}