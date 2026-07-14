#include "interface.h"
#include <simdutf.h>
#include <string>
#include <vector>

namespace {
uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const unsigned char* udata = reinterpret_cast<const unsigned char*>(data);
  for (size_t i = 0; i < len; ++i) {
    hash ^= udata[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}
}  // namespace

uint64_t encode_checksum(const std::string& input) {
  size_t expected_len = ((input.size() + 2) / 3) * 4;
  if (expected_len <= 8192) {
    char buf[8192];
    size_t out_len = simdutf::binary_to_base64(input.data(), input.size(), buf);
    return checksum_bytes(buf, out_len);
  } else {
    thread_local std::vector<char> buf;
    if (buf.size() < expected_len) {
      buf.resize(expected_len);
    }
    size_t out_len = simdutf::binary_to_base64(input.data(), input.size(), buf.data());
    return checksum_bytes(buf.data(), out_len);
  }
}
