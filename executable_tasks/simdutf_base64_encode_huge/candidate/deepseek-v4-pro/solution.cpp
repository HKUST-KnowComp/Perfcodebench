#include "interface.h"

#include <simdutf.h>
#include <cstdint>
#include <cstddef>
#include <memory>

namespace {

constexpr uint64_t kInit = 1469598103934665603ULL;
constexpr uint64_t kMul  = 1099511628211ULL;

uint64_t checksum_chars(const char* data, std::size_t len) {
  uint64_t hash = kInit;
  for (std::size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= kMul;
  }
  return hash;
}

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  const std::size_t in_len = input.size();
  if (in_len == 0) {
    return kInit;
  }

  const std::size_t out_len = simdutf::base64_length(in_len);
  std::unique_ptr<char[]> buf(new char[out_len]);

  auto result = simdutf::binary_to_base64(input.data(), in_len, buf.get());
  // We assume success; on failure the behaviour matches the original's crash.

  return checksum_chars(buf.get(), out_len);
}