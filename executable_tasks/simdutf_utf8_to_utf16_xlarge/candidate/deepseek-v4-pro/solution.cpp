#include "interface.h"

#include <cstddef>
#include <vector>
#include <simdutf.h>

namespace {

uint64_t checksum_utf16(const std::vector<char16_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  const char16_t* ptr = data.data();
  const char16_t* end = ptr + data.size();
  // Unroll 8 iterations to reduce loop overhead.
  while (ptr + 8 <= end) {
    hash ^= static_cast<uint64_t>(ptr[0]); hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[1]); hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[2]); hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[3]); hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[4]); hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[5]); hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[6]); hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[7]); hash *= 1099511628211ULL;
    ptr += 8;
  }
  while (ptr < end) {
    hash ^= static_cast<uint64_t>(*ptr++);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  const size_t len = simdutf::utf16_length_from_utf8(input.data(), input.size());
  std::vector<char16_t> out(len);
  // Convert valid UTF-8 to UTF-16LE
  simdutf::convert_utf8_to_utf16le(input.data(), input.size(), out.data());
  return checksum_utf16(out);
}