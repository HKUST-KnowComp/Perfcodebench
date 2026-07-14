#include "interface.h"
#include "simdutf.h"
#include <cstdint>
#include <string>

namespace {

inline uint64_t hash_u64(uint64_t h, uint16_t v) {
  h ^= v;
  h *= 1099511628211ULL;
  return h;
}

} // namespace

uint64_t transcode_checksum(const std::string& input) {
  const char* src = input.data();
  size_t src_len = input.size();
  uint64_t hash = 1469598103934665603ULL;

  constexpr size_t BUFCHARS = 256;      // 512 bytes
  alignas(64) char16_t buffer[BUFCHARS];

  while (src_len) {
    size_t produce = (src_len >= BUFCHARS) ? BUFCHARS : src_len;
    size_t consumed = 0;
    size_t out_len = 0;

    simdutf::result res = simdutf::convert_utf8_to_utf16le(
        src, produce, reinterpret_cast<char16_t*>(buffer), out_len);
    if (res.error) break;             // should never happen for valid input

    for (size_t i = 0; i < out_len; ++i)
      hash = hash_u64(hash, buffer[i]);

    consumed = res.count;
    src += consumed;
    src_len -= consumed;
  }
  return hash;
}