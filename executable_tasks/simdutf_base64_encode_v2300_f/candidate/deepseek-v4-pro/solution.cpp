#include "interface.h"
#include "simdutf.h"

#include <cstdint>
#include <string>

namespace {

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
  const std::size_t out_len = ((input.size() + 2) / 3) * 4;
  std::string out;
  out.resize(out_len);
  simdutf::encode_base64(input.data(), input.size(), out.data());
  return checksum_bytes(out);
}