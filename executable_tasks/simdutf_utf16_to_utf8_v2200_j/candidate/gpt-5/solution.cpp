#include "interface.h"

#include <simdutf.h>
#include <cstdint>
#include <cstddef>
#include <memory>

namespace {

inline uint64_t fnv1a_hash(const char* data, size_t len) {
  const uint8_t* p = reinterpret_cast<const uint8_t*>(data);
  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis

  // Unroll by 8 for throughput
  while (len >= 8) {
    hash ^= static_cast<uint64_t>(p[0]); hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(p[1]); hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(p[2]); hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(p[3]); hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(p[4]); hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(p[5]); hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(p[6]); hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(p[7]); hash *= 1099511628211ULL;
    p += 8;
    len -= 8;
  }
  while (len != 0) {
    hash ^= static_cast<uint64_t>(*p++);
    hash *= 1099511628211ULL; // FNV prime
    --len;
  }
  return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  const size_t in_len = input.size();
  if (in_len == 0) {
    return 1469598103934665603ULL; // FNV-1a over empty output
  }

  // Safe upper bound: 3 UTF-8 bytes per UTF-16 code unit (surrogates produce 4 per pair)
  const size_t cap = in_len * 3u;
  std::unique_ptr<char[]> out(new char[cap > 0 ? cap : 1]);

  // Input is valid per task; use the faster valid-path conversion
  const size_t written = simdutf::convert_valid_utf16_to_utf8(
      input.data(), in_len, out.get());

  return fnv1a_hash(out.get(), written);
}
