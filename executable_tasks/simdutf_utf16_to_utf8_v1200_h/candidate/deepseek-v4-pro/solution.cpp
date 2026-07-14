#include "interface.h"

#include <simdutf.h>
#include <cstdint>
#include <memory>

namespace {

uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598104663935603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  const size_t max_utf8_bytes = input.size() * 3;
  auto buffer = std::make_unique<char[]>(max_utf8_bytes);
  const size_t actual_bytes = simdutf::convert_utf16le_to_utf8(input.data(), input.size(), buffer.get());
  return checksum_bytes(buffer.get(), actual_bytes);
}
