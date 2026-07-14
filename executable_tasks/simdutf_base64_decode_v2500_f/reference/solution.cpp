#include "interface.h"

#include "simdutf.h"

#include <string>
#include <vector>

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

uint64_t decode_checksum(const std::string& input) {
  std::vector<char> out(simdutf::maximal_binary_length_from_base64(input.data(), input.size()));
  simdutf::result r = simdutf::base64_to_binary(input.data(), input.size(), out.data());
  if (r.error != simdutf::error_code::SUCCESS) {
    return 0;
  }
  return checksum_bytes(std::string(out.data(), r.count));
}
