#include "interface.h"
#include <string>
#include "simdutf.h"

namespace {

uint64_t checksum_bytes(const char* data, std::size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t decode_checksum(const std::string& input) {
  if (input.empty()) {
    return checksum_bytes("", 0);
  }
  // maximum possible decoded size: 3 bytes per 4 base64 chars
  std::size_t max_out = (input.size() / 4) * 3;
  std::string out(max_out, '\0');
  std::size_t actual_len = simdutf::base64_decode(input.data(), input.size(), out.data());
  return checksum_bytes(out.data(), actual_len);
}