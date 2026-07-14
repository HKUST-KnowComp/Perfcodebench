#include "interface.h"
#include <simdutf.h>
#include <string>
#include <cstdint>

namespace {

constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

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
  size_t out_size = ((input.size() + 2) / 3) * 4;
  std::string out;
  out.resize(out_size);
  simdutf::base64::encode(input.data(), input.size(), out.data(), out.size());
  return checksum_bytes(out);
}