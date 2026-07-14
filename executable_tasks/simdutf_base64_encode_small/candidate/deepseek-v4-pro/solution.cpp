#include "interface.h"
#include <simdutf.h>
#include <string>

namespace {
constexpr uint64_t kFnvOffset = 1469598103934665603ULL;
constexpr uint64_t kFnvPrime  = 1099511628211ULL;

uint64_t checksum_bytes(const char* data, size_t size) {
  uint64_t hash = kFnvOffset;
  for (size_t i = 0; i < size; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= kFnvPrime;
  }
  return hash;
}
}  // namespace

uint64_t encode_checksum(const std::string& input) {
  const size_t input_len = input.size();
  const size_t output_len = simdutf::base64_length_from_binary(input_len);
  std::string out(output_len, '\0');
  simdutf::binary_to_base64(input.data(), input_len, &out[0]);
  return checksum_bytes(out.data(), output_len);
}
