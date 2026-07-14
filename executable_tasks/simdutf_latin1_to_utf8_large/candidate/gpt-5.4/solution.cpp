#include "interface.h"
#include "simdutf.h"

#include <cstdint>
#include <string>

namespace {

static inline uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<unsigned char>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  const size_t n = input.size();
  if (n == 0) {
    return 1469598103934665603ULL;
  }

  const char* in = input.data();

  size_t non_ascii = 0;
  for (size_t i = 0; i < n; ++i) {
    non_ascii += static_cast<unsigned char>(in[i]) >> 7;
  }
  const size_t out_len = n + non_ascii;

  std::string out;
  out.resize(out_len);

  const size_t written = simdutf::convert_latin1_to_utf8(in, n, out.data());
  return checksum_bytes(out.data(), written);
}
