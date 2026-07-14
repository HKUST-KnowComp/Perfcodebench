#include "interface.h"
#include <string>
#include <vector>
#include "simdutf.h"

namespace {

uint64_t checksum_bytes(const char* data, std::size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < size; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  if (input.empty()) {
    return checksum_bytes("", 0);
  }

  std::size_t max_size = input.size() * 4;
  std::vector<char> buffer(max_size);

  auto result = simdutf::utf16_to_utf8(input.data(), input.size(), buffer.data());
  if (result.error()) {
    return 0;
  }
  std::size_t utf8_size = result.value();

  return checksum_bytes(buffer.data(), utf8_size);
}