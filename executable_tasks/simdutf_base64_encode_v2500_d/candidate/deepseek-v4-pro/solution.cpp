#include "interface.h"
#include "simdutf.h"
#include <cstdint>
#include <memory>
#include <string>

namespace {

constexpr uint64_t kInitialHash = 1469598103934665603ULL;
constexpr uint64_t kMul = 1099511628211ULL;

uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = kInitialHash;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= kMul;
  }
  return hash;
}

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  const size_t len = input.size();
  if (len == 0) {
    return kInitialHash;
  }
  const size_t out_capacity = ((len + 2) / 3) * 4;
  auto out_buf = std::make_unique<char[]>(out_capacity);
  size_t out_len = simdutf::binary_to_base64(input.data(), len, out_buf.get());
  // out_len should equal out_capacity; simdutf guarantees correct padding
  return checksum_bytes(out_buf.get(), out_len);
}
