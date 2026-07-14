#include "interface.h"
#include "simdutf.h"

namespace {

uint64_t checksum_bytes(const char* data, size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < size; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  size_t utf8_size = simdutf::latin1_to_utf8_size(input.size());
  std::string out(utf8_size, '\0');
  simdutf::latin1_to_utf8(input.data(), input.size(), &out[0]);
  return checksum_bytes(out.data(), out.size());
}