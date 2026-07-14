#include "interface.h"
#include <cstdint>
#include <string>
#include <simdutf.h>

uint64_t transcode_checksum(const std::u16string& input) {
  const size_t len = input.size();
  if (len == 0) {
    return 1469598103934665603ULL; // FNV offset basis for empty input
  }

  // Maximum UTF-8 output length: each UTF-16 code unit can expand to at most 3 bytes
  std::string out;
  out.resize(len * 3);

  auto result = simdutf::convert_utf16le_to_utf8(input.data(), len, &out[0]);
  size_t out_len = result.count;

  // FNV-1a 64-bit hash
  uint64_t hash = 1469598103934665603ULL;
  const unsigned char* data = reinterpret_cast<const unsigned char*>(out.data());
  for (size_t i = 0; i < out_len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}