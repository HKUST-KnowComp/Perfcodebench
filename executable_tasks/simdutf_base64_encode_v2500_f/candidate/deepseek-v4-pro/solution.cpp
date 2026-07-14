#include "interface.h"
#include <simdutf.h>
#include <cstdint>
#include <string>
#include <memory>

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

uint64_t encode_checksum(const std::string& input) {
  const std::size_t input_len = input.size();
  if (input_len == 0) {
    return checksum_bytes(nullptr, 0);
  }

  const std::size_t out_len = ((input_len + 2) / 3) * 4;
  auto out_buffer = std::make_unique<char[]>(out_len);

  const std::size_t encoded_len =
      simdutf::base64::encode(input.data(), input_len, out_buffer.get());

  return checksum_bytes(out_buffer.get(), encoded_len);
}