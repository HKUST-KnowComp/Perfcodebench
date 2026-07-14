#include "interface.h"
#include <string>
#include <simdutf.h>

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

uint64_t decode_checksum(const std::string& input) {
  if (input.empty()) {
    return 0;
  }

  std::size_t max_output_size = (input.size() / 4) * 3;
  std::string out;
  out.resize(max_output_size);

  auto decoded_size = simdutf::base64_decode(input.data(), input.size(), out.data());

  return checksum_bytes(out.data(), decoded_size);
}