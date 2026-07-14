#include "interface.h"
#include "simdutf.h"

#include <vector>
#include <cstdint>

namespace {

static thread_local std::vector<char16_t> buffer;

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  buffer.reserve(input.size());
  buffer.resize(input.size());
  const auto actual_len = simdutf::convert_valid_utf8_to_utf16le(
      input.data(), input.size(), buffer.data());

  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < actual_len; ++i) {
    hash ^= static_cast<uint64_t>(buffer[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}
