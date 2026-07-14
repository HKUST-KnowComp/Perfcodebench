#include "interface.h"

#include <simdutf.h>
#include <memory>

namespace {

uint64_t checksum_bytes(const char* data, std::size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < len; ++i) {
    const auto ch = static_cast<unsigned char>(data[i]);
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  const std::size_t n = input.size();
  const std::size_t out_len = ((n + 2) / 3) * 4;
  std::unique_ptr<char[]> out(new char[out_len]);
  simdutf::binary_to_base64(input.data(), n, out.get());
  return checksum_bytes(out.get(), out_len);
}
