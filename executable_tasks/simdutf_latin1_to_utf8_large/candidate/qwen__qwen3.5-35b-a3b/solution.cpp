#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>
#include "simdutf.h"

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
  if (input.empty()) {
    return checksum_bytes(nullptr, 0);
  }
  std::vector<char> out(input.size() * 2);
  size_t converted = simdutf::latin1_to_utf8(input.data(), input.size(), out.data());
  return checksum_bytes(out.data(), converted);
}