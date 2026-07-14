#include "interface.h"
#include <simdutf.h>
#include <string>
#include <cstdint>

namespace {

constexpr uint64_t kHashInit = 1469598103934665603ULL;
constexpr uint64_t kHashMultiplier = 1099511628211ULL;

uint64_t checksum(const char* data, size_t len) {
  uint64_t hash = kHashInit;
  for (size_t i = 0; i < len; ++i) {
    unsigned char ch = static_cast<unsigned char>(data[i]);
    hash ^= static_cast<uint64_t>(ch);
    hash *= kHashMultiplier;
  }
  return hash;
}

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  size_t n = input.size();
  if (n == 0) {
    return checksum(nullptr, 0);
  }
  size_t out_len = ((n + 2) / 3) * 4;
  std::string out;
  out.resize(out_len);
  size_t written = simdutf::binary_to_base64(input.data(), n, out.data());
  return checksum(out.data(), written);
}