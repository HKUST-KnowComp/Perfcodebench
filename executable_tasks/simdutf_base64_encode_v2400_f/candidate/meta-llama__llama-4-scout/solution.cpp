#include "interface.h"
#include "simdutf.h"

#include <string>
#include <cstdint>

namespace {

constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

uint64_t checksum_bytes(const std::string& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  simdutf::result encoded = simdutf::base64_encode(input);
  if (!encoded.is_ok()) {
    // Handle error
    return 0;
  }
  return checksum_bytes(encoded.get());
}
