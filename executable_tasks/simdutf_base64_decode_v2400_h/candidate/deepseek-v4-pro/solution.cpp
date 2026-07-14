#include "interface.h"
#include <simdutf.h>
#include <memory>
#include <cstddef>

namespace {

uint64_t checksum_raw(const char* data, std::size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
  const char* input_data = input.data();
  std::size_t input_len = input.size();

  // Maximum possible decoded length (no padding analysis, just fast estimation)
  std::size_t max_out = simdutf::maximal_binary_length_from_base64(input_data, input_len);

  // Allocate uninitialized buffer to avoid zero-fill overhead
  auto buffer = std::make_unique<char[]>(max_out);

  // Decode base64 (input assumed valid)
  auto result = simdutf::base64_decode(input_data, input_len, buffer.get());

  // Compute checksum directly over the decoded bytes
  return checksum_raw(buffer.get(), result.count);
}
